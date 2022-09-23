#include "compile/link.hpp"

#include <sstream>

namespace nnc::compile {
  class CustomRegisterMapper : public RtlRegisterMapper {
  public:
    CustomRegisterMapper(const std::multimap<RtlVariablePtr, VirtualRegister> &vars)
      : m_vars(vars) {
    }
    virtual ~CustomRegisterMapper() { }

    virtual std::vector<VirtualRegister> lookupVar(const RtlVariablePtr &v) const override {
      auto [begin, end] = m_vars.equal_range(v);
      std::vector<VirtualRegister> ret;
      std::transform(begin, end, std::back_inserter(ret),
                     [](const auto &p) { return p.second; });
      return ret;
    }

  private:
    const std::multimap<RtlVariablePtr, VirtualRegister> m_vars;
  };

  class NoFixupEmitter : public BytecodeEmitter {
  public:
    virtual void write_bytes(BytecodeInfo::Section section,
                             const std::uint8_t *d, std::size_t sz) {
      if ( section != BytecodeInfo::CodeSection )
        throw std::runtime_error("Cannot write to code section in jump rewriter");
      m_out.write((const char *)d, sz);
    }

    virtual void write_fixup(BytecodeInfo::Section section,
                             std::unique_ptr<BytecodeFixup> &&f) {
      throw std::runtime_error("Cannot write fixup in jump rewriter");
    }

    virtual std::uintptr_t allocate_constant(const std::span<std::uint8_t> &c) {
      throw std::runtime_error("Cannot allocate constant in jump rewriter");
    }

    inline std::string str() const { return m_out.str(); }

  private:
    std::stringstream m_out;
  };

  class LinkerInfo : public BytecodeInfo {
  public:
    LinkerInfo(std::uintptr_t code_base, std::uintptr_t data_base)
        : m_code_base(code_base), m_data_base(data_base) {}

    virtual ~LinkerInfo() {}

    virtual std::uintptr_t section_start(Section section) const override {
      switch (section) {
      case CodeSection:
        return m_code_base;
      case DataSection:
        return m_data_base;
      default:
        throw std::runtime_error("Bad section given");
      }
    }

  private:
    std::uintptr_t m_code_base, m_data_base;
  };

  void
  Linker::EmitBytes::fixup(BytecodeInfo &info, std::uint8_t *buf,
                           std::size_t sz, std::uintptr_t base_addr) {
  }

  class EmitConstant : public Linker::EmitBytes {
  public:
    virtual ~EmitConstant() { };

    virtual void emit(std::ostream &out, std::uintptr_t base_addr) const override {
      out.write(m_constant.data(), m_constant.size());
    }

    virtual std::size_t size() const { return m_constant.size(); }

  protected:
    std::string m_constant;
  };

  class EmitString : public EmitConstant {
  public:
    EmitString(const std::string &s) {
      m_constant = s;
    }
    EmitString(std::string &&s) { m_constant = std::move(s); }
    virtual ~EmitString() { };
  };

  class EmitWithFixups : public Linker::EmitBytes, public BytecodeEmitter {
  public:
    EmitWithFixups(Linker &l) : m_linker(l) {}
    virtual ~EmitWithFixups() {}

    inline Linker &linker() const { return m_linker; }

    virtual void write_bytes(BytecodeInfo::Section section,
                             const std::uint8_t *d, std::size_t sz) override {
      if (section == BytecodeInfo::CodeSection) {
        auto pos(m_buf.size());
        m_buf.resize(m_buf.size() + sz);
        std::copy(d, d + sz, m_buf.begin() + pos);
      } else {
        linker().writeSection(section, d, sz);
      }
    }

    virtual void write_fixup(BytecodeInfo::Section section,
                             std::unique_ptr<BytecodeFixup> &&f) override {
      if (section != BytecodeInfo::CodeSection) {
        throw std::runtime_error("Fixups only allowed in code section");
      }

      auto pos(m_buf.size());
      m_buf.resize(pos + f->size());
      std::fill(m_buf.begin() + pos, m_buf.end(), 0);

      m_fixups.emplace_back(pos, std::move(f));
    }

    virtual std::uintptr_t allocate_constant(const std::span<std::uint8_t> &c) {
      auto ret(linker().sectionOffset(BytecodeInfo::DataSection));
      linker().writeSection(BytecodeInfo::DataSection, c.data(), c.size());
      return ret;
    }


    virtual void emit(std::ostream &out, std::uintptr_t base_addr) const override {
      out.write((const char *)m_buf.data(), m_buf.size());
    }

    virtual void fixup(BytecodeInfo & info,
                       std::uint8_t *buf, std::size_t sz,
                       std::uintptr_t base_addr) override {
      for (const auto &f : m_fixups) {
        f.second->fixup(info, buf + f.first, base_addr + f.first);
      }
    }

    virtual std::size_t size() const override { return m_buf.size(); }

  private:
    Linker &m_linker;

    std::vector<std::uint8_t> m_buf;
    std::list<std::pair<std::size_t, std::unique_ptr<BytecodeFixup>>> m_fixups;
  };

  class EmitPreamble : public EmitWithFixups {
  public:
    EmitPreamble(Linker &l, const InsnEncoder &e)
      : EmitWithFixups(l) {
      e.preamble(*this);
    }
    virtual ~EmitPreamble() {};
  };

  class EmitPostamble : public EmitWithFixups {
  public:
    EmitPostamble(Linker &l, const InsnEncoder &e)
      : EmitWithFixups(l) {
      e.postamble(*this);
      e.funret(*this);
    }
    virtual ~EmitPostamble() { }
  };

  class EmitBlock : public EmitWithFixups {
  public:
    EmitBlock(Linker &l, const RtlBasicBlock &block, const RegisterTracker &tracker, const InsnEncoder &e)
      : EmitWithFixups(l) {
      encodeBasicBlock(*this, block, tracker, e);
    }
    virtual ~EmitBlock() { }
  };

  class EmitJump : public Linker::EmitBytes {
  public:
    EmitJump(std::unique_ptr<JumpInsn> &&jump)
      : m_jump(std::move(jump)) {
    }
    virtual ~EmitJump() { }

    virtual void emit(std::ostream &out, std::uintptr_t base_addr) const override {
      return m_jump->output(out);
    }

    virtual std::size_t size() const { return m_jump->size(); }

  private:
    std::unique_ptr<JumpInsn> m_jump;
  };

  class EmitFixup : public Linker::EmitBytes {
  public:
    EmitFixup(Linker::Fixup &fu)
      : m_fixup(fu) { }
    virtual ~EmitFixup() { }

    virtual void emit(std::ostream &out, std::uintptr_t base_addr) const {
      m_fixup.jump().output(out);
    }

    virtual std::size_t size() const { return m_fixup.jump().size(); }

  private:
    Linker::Fixup &m_fixup;
  };

  class EmitJumpRewrite : public Linker::EmitBytes {
  public:
    EmitJumpRewrite(Linker::JumpRewrite &rewrite)
      : m_rewrite(rewrite) {
    }
    virtual ~EmitJumpRewrite() {
    }

    virtual void emit(std::ostream &out, std::uintptr_t base_addr) const {
      m_rewrite.jump().output(out);
    }

    virtual std::size_t size() const {
      return m_rewrite.jump().size();
    }

  private:
    Linker::JumpRewrite &m_rewrite;
  };

  Linker::Linker(const InsnEncoder &encoder)
    : m_encoder(encoder), m_position(0), m_totalSize(0){
  }

  template<typename T, typename... Args>
  T &Linker::plan(BytecodeInfo::Section section, Args&&... args) {
    auto p(std::make_unique<T>(std::forward<Args>(args)...));
    if ( section == BytecodeInfo::CodeSection )
      m_position += p->size();
    m_totalSize += p->size();
    std::unique_ptr<EmitBytes> &bytes(m_plan[section].emplace_back(std::move(p)));
    return dynamic_cast<T &>(*bytes);
  }

  void Linker::discardConditionalJump(const RtlBasicBlock &to, const RegisterTracker &tracker) {
    JumpRewrite rewriter(encoder(), m_conditionalJump->jump, m_position,
                         m_conditionalJump->args, tracker.mapper(-1), to);
    if ( rewriter.needed() ) {
      plan<EmitString>(BytecodeInfo::CodeSection, rewriter.data());
    }
    m_conditionalJump.reset();
  }

  void Linker::insertConditionalJump(const RtlBasicBlock &skipped, const RegisterTracker &tracker) {
    JumpRewrite rewriter(encoder(), m_conditionalJump->jump, m_position,
                         m_conditionalJump->args, tracker.mapper(-1), m_conditionalJump->dest);
    if ( rewriter.needed() ) {
      JumpRewrite &perm(m_jumpRewrites.emplace_back(std::move(rewriter)));

      perm.setJump(std::move(m_conditionalJump->insn));
      plan<EmitJumpRewrite>(BytecodeInfo::CodeSection, perm);
    } else {
      auto &fixup(m_fixups.emplace_back(m_position, m_conditionalJump->dest,
                                        std::move(m_conditionalJump->insn)));
      plan<EmitFixup>(BytecodeInfo::CodeSection, fixup);
    }
    m_conditionalJump.reset();
  }

  void Linker::writeSection(BytecodeInfo::Section section,
                            const std::uint8_t *d, std::size_t sz) {
    std::string data(d, d + sz);
    plan<EmitString>(section, std::move(data));
  }

  std::uintptr_t Linker::sectionOffset(BytecodeInfo::Section section) const {
    std::uintptr_t ret(0);
    auto it(m_plan.find(section));
    if (it != m_plan.end()) {
      for (const auto &emit : it->second)
        ret += emit->size();
    }

    return ret;
  }

  void Linker::placeBlock(const RtlBasicBlock &block, RegisterAllocatorBase &trackers) {
    auto &tracker(trackers.registersFor(block.name()));
    if ( blockPosition(block) != (-1) ) return;

    if ( m_conditionalJump ) {
      if ( m_conditionalJump->function == &block.function() && m_conditionalJump->jump.to() == block.name() ) {
        // No need for this jump
        discardConditionalJump(block, tracker);
      } else {
        insertConditionalJump(block, tracker);
      }
    }

    std::cerr << "Place block " << block.name().index() << " @ " << std::hex << "0x" << m_position << std::dec << std::endl;
    m_positions.emplace(std::make_pair(&block.function(), block.name()), m_position);

    std::uint64_t newBlockPosition(m_position);
    if ( block.isEntryPoint() ) {
      plan<EmitPreamble>(BytecodeInfo::CodeSection, *this, encoder());
    }

    EmitBlock &blockBytes(plan<EmitBlock>(BytecodeInfo::CodeSection, *this, block, tracker, encoder()));

    bool had_default_jump(false);
    for ( const auto &jump: block.jumps() ) {
      std::optional<Register> cond;

      auto mapper(tracker.mapper(block.endTime()));
      if ( jump.first )
        cond = chooseRegister(mapper, encoder().jump_cond_class(), jump.first);
      else
        had_default_jump = true;

      auto &destination(block.function().block(jump.second.to()));
      auto expected(trackers.registersFor(jump.second.to()));

      std::multimap<RtlVariablePtr, VirtualRegister> actualMap;

      for ( const auto &arg: jump.second.arguments() ) {
        auto vregs(mapper.lookupVar(arg));
        std::transform(vregs.begin(), vregs.end(),
                       std::inserter(actualMap, actualMap.begin()),
                       [&arg](const auto &vreg) { return std::make_pair(arg, vreg); });
      }

      JumpRewrite rewrite(encoder(), jump.second, m_position, actualMap, expected.mapper(-1), destination);

      auto pos(blockPosition(block.function(), jump.second.to()));
      if ( pos == -1 ) {
        // Nowhere to jump, so we need a fixup
        std::cerr << std::hex << "Placing a jump at 0x" << m_position << " to " << jump.second.to().index() << std::endl;
        auto jumpInsn(encoder().generic_jump(cond));

        if ( !jump.first )
          m_conditionalJump.emplace(block, block.function().block(jump.second.to()),
                                    tracker, jump.second, std::move(jumpInsn));
        else if ( rewrite.needed() ) {
          auto &perm(m_jumpRewrites.emplace_back(std::move(rewrite)));
          perm.setJump(std::move(jumpInsn));
          plan<EmitJumpRewrite>(BytecodeInfo::CodeSection, perm);
        } else {
          auto &fixup(m_fixups.emplace_back(m_position, block.function().block(jump.second.to()),
                                            std::move(jumpInsn)));
          plan<EmitFixup>(BytecodeInfo::CodeSection, fixup);
        }
      } else if (rewrite.needed()) {
        auto jumpInsn(encoder().generic_jump(cond));
        auto &perm(m_jumpRewrites.emplace_back(std::move(rewrite)));
        perm.setJump(std::move(jumpInsn));
        plan<EmitJumpRewrite>(BytecodeInfo::CodeSection, perm);
      } else {
        auto jump(encoder().jump_relative(pos - m_position, cond));
        EmitJump &emitJump(plan<EmitJump>(BytecodeInfo::CodeSection, std::move(jump)));
      }
    }

    if ( !had_default_jump ) {
      // Needs the postamble
      EmitPostamble &p(plan<EmitPostamble>(BytecodeInfo::CodeSection, *this, encoder()));
    }
  }

  void Linker::finish() {
    if ( m_conditionalJump )
      // There really shouldn't be any conditional jump at this point, but check anyway
      throw std::runtime_error("Conditional jump during linker finish");

    for ( const auto &fixup: m_fixups ) {
      auto position(blockPosition(fixup.destination()));
      if ( position == (-1) )
        throw std::runtime_error("Could not find jump target");

      std::int64_t diff(position - fixup.position() - fixup.jump().size());
      if ( diff > fixup.jump().max_distance() )
        throw std::runtime_error("Jump is too far to work");

      fixup.jump().relative(diff);
    }

    for ( auto &rewrite: m_jumpRewrites ) {
      auto position(blockPosition(rewrite.destination()));
      if ( position == (-1) )
        throw std::runtime_error("Could not find jump target");

      rewrite.setJumpTarget(position, m_position);

      std::int64_t diff(m_position - rewrite.jumpPos());
      rewrite.finish(diff);
      plan<EmitString>(BytecodeInfo::CodeSection, rewrite.data());
    }
  }

  void Linker::dumpSection(BytecodeInfo::Section section, std::ostream &out,
                           std::uintptr_t &base_addr) const {
    auto plan(m_plan.find(section));
    if ( plan == m_plan.end() ) return;

    for (const auto &emit : plan->second) {
      emit->emit(out, base_addr);
      base_addr += emit->size();
    }
  }

  void Linker::fixupSection(BytecodeInfo &info,
                            BytecodeInfo::Section section, std::uint8_t *base) const {
    std::uintptr_t base_addr(info.section_start(section));
    std::uint8_t *section_base(base);
    auto plan(m_plan.find(section));
    if ( plan == m_plan.end() ) return;

    for (const auto &emit : plan->second) {
      std::size_t sz(emit->size());
      emit->fixup(info, section_base, sz, base_addr);
      base_addr += sz;
      section_base += sz;
    }
  }

  void Linker::dump(std::ostream &out, std::uintptr_t base_addr) const {
    // Dump code section first
    std::stringstream buf;

    std::uintptr_t code_base(0), data_base(0);
    dumpSection(BytecodeInfo::CodeSection, buf, data_base);

    std::uintptr_t end_addr(0);
    dumpSection(BytecodeInfo::DataSection, buf, end_addr);

    std::string bufstr(buf.str());
    if (base_addr == 0) {
      base_addr = (std::uintptr_t) bufstr.data();
    }

    LinkerInfo info(base_addr + code_base, base_addr + data_base);
    fixupSection(info, BytecodeInfo::CodeSection, (std::uint8_t *) bufstr.data());
    fixupSection(info, BytecodeInfo::DataSection, (std::uint8_t *) bufstr.data());

    out.write(bufstr.data(), bufstr.size());
  }

  std::uint64_t Linker::blockPosition(const RtlFunction &fn, const RtlBlockName &nm) const {
    auto it(m_positions.find(std::make_pair(&fn, nm)));
    if ( it == m_positions.end() ) return (-1);
    else return it->second;
  }

  Linker::Fixup::Fixup(std::uint64_t jumpPos, const RtlBasicBlock &dest,
                       std::unique_ptr<JumpInsn> &&insn)
    : m_position(jumpPos), m_destination(dest),
      m_insn(std::move(insn)) {
  }

  Linker::JumpRewrite::JumpRewrite(const InsnEncoder &e,
                                   const RtlJump &jump,
                                   std::uint64_t pos,
                                   const std::multimap<RtlVariablePtr, VirtualRegister> &actualMap,
                                   const RtlRegisterMapper &expected,
                                   const RtlBasicBlock &dest)
    : m_finalJump(e.generic_jump(std::nullopt)),
      m_jumpPos(pos),
      m_destination(dest) {
    CustomRegisterMapper actual(actualMap);
    NoFixupEmitter body;

    for ( auto [ argval, arg ] = std::tuple { jump.arguments().begin(), dest.inputs().begin() };
          argval != jump.arguments().end();
          ++argval, ++arg ) {
      auto actualPos(actual.lookupVar(*argval));
      auto expectedPos(expected.lookupVar(*arg));

      std::cerr << "    Actual: ";

      std::sort(std::begin(actualPos), std::end(actualPos));
      std::sort(std::begin(expectedPos), std::end(expectedPos));

      std::copy(std::begin(actualPos), std::end(actualPos), std::ostream_iterator<VirtualRegister>(std::cerr, " "));
      std::cerr << std::endl;
      std::cerr << "    ExpectedPos: ";
      std::copy(std::begin(expectedPos), std::end(expectedPos), std::ostream_iterator<VirtualRegister>(std::cerr, " " ));
      std::cerr << std::endl;

      std::deque<VirtualRegister> nonExistent;
      std::set_difference(std::begin(expectedPos), std::end(expectedPos),
                          std::begin(actualPos), std::end(actualPos),
                          std::back_inserter(nonExistent));

      for ( const auto &reg: nonExistent ) {
       e.copy(body, actual, *argval, reg);
      }
    }

    m_body = body.str();
  }

  void Linker::JumpRewrite::setJumpTarget(std::uint64_t tgt, std::uint64_t rewritePos) const {
    std::int64_t diff(tgt - rewritePos - data().size());
    if ( labs(diff) > m_finalJump->max_distance() )
      throw std::runtime_error("Jump rewrite is too far");

    m_finalJump->relative(diff);
  }

  void Linker::JumpRewrite::setJump(std::unique_ptr<JumpInsn> &&insn) {
    m_jumpInsn = std::move(insn);
  }

  void Linker::JumpRewrite::finish(std::int64_t jumprel) {
    if ( m_jumpInsn )
      m_jumpInsn->relative(jumprel - m_jumpInsn->size());

    std::stringstream final_;
    m_finalJump->output(final_);

    m_body += final_.str();
  }

  Linker::ConditionalJump::ConditionalJump(const RtlBasicBlock &block,
                                           const RtlBasicBlock &d,
                                           const RegisterTracker &tracker,
                                           const RtlJump &j,
                                           std::unique_ptr<JumpInsn> &&jumpinsn)
    : function(&block.function()),
      jump(j),
      dest(d),
      insn(std::move(jumpinsn)) {
    auto mapper(tracker.mapper(block.endTime()));
    for ( const auto &arg: jump.arguments() ) {
      auto locs(mapper.lookupVar(arg));
      std::transform(locs.begin(), locs.end(),
                     std::inserter(args, args.begin()),
                     [&arg](const auto &loc) { return std::make_pair(arg, loc); });
    }
  }
}
