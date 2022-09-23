"#include "compile/genericschedule.hpp"
#include "compile/encoding.hpp"
#include "compile/arbitrary.hpp"
// #include "compile/reorder.hpp"
#include "graph/repr.hpp"
#include "graph/dfs.hpp"
#include "graph/toposort.hpp"

#include <deque>
#include <iostream>
#include <sstream>

namespace nnc {
  namespace exception {
    class OperationNotFound : public std::exception {
    public:
      OperationNotFound(compile::RtlOp *op) : m_op(op) {
      }
      virtual ~OperationNotFound() {
      }

      virtual const char *what() const noexcept { return "operation not found"; }

      inline compile::RtlOp *op() const { return m_op; }

    private:
      compile::RtlOp *m_op;
    };

    class BlockNotScheduled : public std::exception {
    public:
      BlockNotScheduled(const compile::RtlBlockName &nm) : m_name(nm) {
      }
      virtual ~BlockNotScheduled() { }

      virtual const char *what() const noexcept override { return "block not scheduled"; }

      inline compile::RtlBlockName name() const { return m_name; }

    private:
      compile::RtlBlockName m_name;
    };
  }

  namespace compile {
    template<typename RegisterAssignments>
    void dumpAssignments(const RegisterAssignments &assigns, std::ostream &out) {
      bool comma(false);
      if ( assigns.empty() )
        out << "unassigned";

      for ( const auto &assign: assigns ) {
        if ( comma ) out << ", ";
        out << assign;
        comma = true;
      }
    }

    class RegistersOperandPrinter : public virtual OperandPrinter {
    public:
      RegistersOperandPrinter(RtlFunction &fn, RegisterTracker &t,
                              int cycle, std::ostream &out)
        : RtlOperandVisitor(fn), OperandPrinter(fn, out), m_regs(t), m_cycle(cycle) {
      }
      virtual ~RegistersOperandPrinter() {
      }

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) override {
        OperandPrinter::operand(name, var, input, output);
        m_out << "[";
        dumpAssignments(m_regs.assignment(m_cycle, var), m_out);
        m_out << "]";
      }

    protected:
      const RegisterTracker &m_regs;
      int m_cycle;
    };

    void RtlOpSelector::arbitrary(const RtlVariablePtr &v, const RegClass &rc) {
    }

    class InsnCostCalcer : public RtlOpMarker, public RtlOpSelector {
    public:
      InsnCostCalcer(RtlFunction &fn) : m_function(fn), m_insnCost(0), m_consumedOpCount(0) {

      }
      virtual ~InsnCostCalcer() {
      }

      void calc(SelectedInsn &op) {
        // Calc how many ops there are
        op.consumedOperations(*this);
        op.build(*this);
      }

      virtual void matchOp(RtlOp *op) {
        m_consumedOpCount ++;
      }

      double cost() const { return m_insnCost / m_consumedOpCount; }

      virtual void alias(const std::shared_ptr<RtlVariable> &l,
                         const std::shared_ptr<RtlVariable> &r ) {
      }

      virtual void op(std::unique_ptr<RtlOp> &&o) {
        auto costed(dynamic_cast<RtlCostedOp *>(o.get()));
        if ( costed )
          m_insnCost += costed->cost();
        else
          m_insnCost += 1000.0;
      }

      virtual void clear() {
        m_insnCost = 0;
      }

      virtual RtlFunction &function() const { return  m_function; }

    private:
      RtlFunction &m_function;
      double m_insnCost;
      int m_consumedOpCount;
    };

    class OpTranslator : public RtlOperandVisitor {
    public:
      OpTranslator(GenericScheduler &sched, RtlOp &o)
        : RtlOperandVisitor(sched.sourceBlock()->function()), m_sched(sched), m_op(o) {
      }
      virtual ~OpTranslator() {
      }

      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
      }
      virtual void operand(const std::string &name, const RtlBlockName &dest) {
      }
      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
        std::shared_ptr<RtlVariable> newVar(m_sched.translateVariable(var));
        if ( newVar )
          m_op.operand(name, newVar);
        else
          newVar = var;
      }

    private:
      GenericScheduler &m_sched;
      RtlOp &m_op;
    };

    class TranslatingOpBuilder : public RtlOpSelector {
    public:
      TranslatingOpBuilder(GenericScheduler &sched, RtlOpBuilder &blk)
        : m_sched(sched), m_block(blk) {
      }
      virtual ~TranslatingOpBuilder() {
        m_block.addOps(std::begin(m_ops), std::end(m_ops));
      }

      virtual RtlFunction &function() const { return m_block.function(); }

      virtual void op(std::unique_ptr<RtlOp> &&o) {
        // Translate each op
        OpTranslator t(m_sched, *o);
        o->operands(t);

        m_ops.emplace_back(std::move(o));
      }

      virtual void clear() {
        m_ops.clear();
      }

      virtual void alias(const std::shared_ptr<RtlVariable> &left, const std::shared_ptr<RtlVariable> &right) {
        // TODO we need to figure out what's the input and what's the output
        m_ops.emplace_back(std::make_unique<RtlAliasOp>(translate(left), translate(right)));
      }

      virtual void arbitrary(const RtlVariablePtr &v, const RegClass &rc) override {
        m_ops.emplace_back(std::make_unique<RtlArbitraryOp>(translate(v), rc));
      }

    private:
      std::shared_ptr<RtlVariable> translate(const std::shared_ptr<RtlVariable> &v) {
        auto r(m_sched.translateVariable(v));
        if ( r ) return r;
        else return v;
      }

      GenericScheduler &m_sched;
      RtlOpBuilder &m_block;
      std::list<std::unique_ptr<RtlOp>> m_ops;
    };

    class ReadinessTracker;
    class OpReadiness : public RtlOpSelector, public RtlOperandVisitor {
    public:
      OpReadiness(ReadinessTracker &t, SelectedInsn &i);
      virtual ~OpReadiness() {
      };

      inline SelectedInsn &insn() const { return m_insn; }
      inline ReadinessTracker &tracker() const { return m_tracker; }
      inline bool ready() const { return m_ready; }

      void mark();
      void addOverlaps();

      void check() {
        m_insn.build(*this);
      }

      virtual void arbitrary(const RtlVariablePtr &v, const RegClass &rc) override {
        markVar(v);
      }

      virtual void alias(const std::shared_ptr<RtlVariable> &left,
                         const std::shared_ptr<RtlVariable> &right) {
        //        std::cerr << "Alias " << left->repr()->name() << " with " << right->repr()->name() << std::endl;
        if ( checkVar(left) ) markVar(right);
      }

      virtual void clear() {
        m_ready = true;
      }

      virtual RtlFunction &function() const;
      virtual void op(std::unique_ptr<RtlOp> &&op);

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false);
      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
      }
      virtual void operand(const std::string &name, const RtlBlockName &dest) {
      }

    private:
      bool checkVar(const std::shared_ptr<RtlVariable> &v);
      void markVar(const std::shared_ptr<RtlVariable> &v) {
        m_madeReady.insert(v->repr());
      }

      ReadinessTracker &m_tracker;
      SelectedInsn &m_insn;
      bool m_ready;

      std::set<std::shared_ptr<RtlVariable>> m_madeReady;
    };

    class OverlapInserter : public RtlOpMarker {
    public:
      OverlapInserter(ReadinessTracker &t)
        : m_tracker(t) {
      }
      virtual ~OverlapInserter();

      virtual void matchOp(RtlOp *o) {
        m_ops.insert(o);
      }

    private:
      ReadinessTracker &m_tracker;
      std::set<RtlOp *> m_ops;
    };

    class InterferenceChecker : public RtlOpMarker {
    public:
      InterferenceChecker(const ReadinessTracker &t)
        : m_tracker(t) {
      }

      virtual ~InterferenceChecker() {
      }

      bool interferes() const;

      virtual void matchOp(RtlOp *o);

    private:
      const ReadinessTracker &m_tracker;
      std::set<RtlOp *> m_ops;
    };

    class ReadinessMarker : public RtlOpBuilder, public RtlOperandVisitor {
    public:
      ReadinessMarker(ReadinessTracker &t, RtlOpBuilder &b)
        : RtlOperandVisitor(b.function()), m_tracker(t), m_underlying(b) {
      }
      virtual ~ReadinessMarker();

      virtual void clear() { m_madeReady.clear(); m_underlying.clear(); }
      virtual RtlFunction &function() const;

      virtual void op(std::unique_ptr<RtlOp> &&op);

      inline ReadinessTracker &tracker() const { return m_tracker; }

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false);
      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
      }
      virtual void operand(const std::string &name, const RtlBlockName &dest) {
      }

    private:
      ReadinessTracker &m_tracker;
      RtlOpBuilder &m_underlying;
      std::set<std::shared_ptr<RtlVariable>> m_madeReady;
    };

    class ReadinessTracker {
    public:
      ReadinessTracker(GenericScheduler &s) : m_sched(s) { }

      ReadinessMarker builder(RtlOpBuilder &b) {
        return ReadinessMarker(*this, b);
      }

      bool isReady(const std::shared_ptr<RtlVariable> &v) const { return m_ready.find(v->repr()) != m_ready.end(); }
      void markReady(const std::shared_ptr<RtlVariable> &v) { m_ready.insert(v->repr()); }
      void markReady(const std::set<std::shared_ptr<RtlVariable>> &vs) {
        for ( const auto &v: vs )
          markReady(v);
      }

      void blacklist(RtlOp *o, int minInsns) {
        auto it(m_blacklist.find(o));
        if ( it == m_blacklist.end() )
          m_blacklist.insert(std::make_pair(o, minInsns));
        else
          it->second = std::max(minInsns, it->second);
      }
      bool isBlacklisted(RtlOp *o, int insnCount) const {
        auto it(m_blacklist.find(o));
        if ( it == m_blacklist.end() ) return false;
        else if ( insnCount < it->second ) return true;
        else return false;
      }
      void clearBlacklist() { m_blacklist.clear(); }

      OpReadiness check(SelectedInsn &insn) {
        OpReadiness r(*this, insn);
        r.check();
        return r;
      }

      bool interferes(SelectedInsn &insn) const {
        InterferenceChecker r(*this);
        insn.consumedOperations(r);
        return r.interferes();
      }

      inline RtlFunction &function() const { return m_sched.function(); }

      std::shared_ptr<RtlVariable> translateVariable(const std::shared_ptr<RtlVariable> &v) {
        return m_sched.translateVariable(v);
      }

    private:
      GenericScheduler &m_sched;

      std::set<std::shared_ptr<RtlVariable>> m_ready;
      std::map<RtlOp *, int> m_blacklist;
    };

    class EmitBytes {
    public:
      virtual ~EmitBytes() { }

      virtual void emit(std::ostream &out) const =0;
    };

    class EmitString : public EmitBytes {
    public:
      EmitString(const std::string &s)
        : m_data(s) {
      }
      virtual ~EmitString() {
      }

      virtual void emit(std::ostream &out) const {
        out.write(m_data.data(), m_data.size());
      }

    private:
      std::string m_data;
    };

    class JumpRewrite {
    public:
      JumpRewrite(JumpRewrite &&r) =default;
      JumpRewrite(const InsnEncoder &e,
                  const RtlJump &jump,
                  std::uint64_t pos,
                  const RegisterTrackerMapper &inputs,
                  GenericScheduler &dest)
        : m_finalJump(e.generic_jump(std::nullopt)),
          m_jumpPos(pos),
          m_destination(dest) {
        auto argPos(dest.registers().mapper(0));
        std::cerr << "For call to " << dest.destBlock()->name().index() << std::endl;
        for ( auto [ argval, arg ] = std::tuple { jump.arguments().begin(), dest.destBlock()->inputs().begin() };
              argval != jump.arguments().end();
              ++argval, ++arg ) {
          std::cerr << "  For arg " << (*arg)->name() << std::endl;

          auto actual(inputs.lookupVar(*argval));
          auto expected(argPos.lookupVar(*arg));

          std::cerr << "    Actual: ";

          std::sort(std::begin(actual), std::end(actual));
          std::sort(std::begin(expected), std::end(expected));

          std::copy(std::begin(actual), std::end(actual), std::ostream_iterator<VirtualRegister>(std::cerr, " "));
          std::cerr << std::endl;
          std::cerr << "    Expected: ";
          std::copy(std::begin(expected), std::end(expected), std::ostream_iterator<VirtualRegister>(std::cerr, " " ));
          std::cerr << std::endl;

          std::deque<VirtualRegister> nonExistent;
          std::set_difference(std::begin(expected), std::end(expected),
                              std::begin(actual), std::end(actual),
                              std::back_inserter(nonExistent));

          // TODO handle clobbering
          // Must move from actual to nonExistent
          for ( const auto &reg: nonExistent ) {
            e.copy(m_body, inputs, *argval, reg);
          }
        }
      }
      virtual ~JumpRewrite() {
      }

      void setPosition(std::uint64_t pos) const {
        std::int64_t diff(pos - m_jumpPos - (m_jumpInsn ? m_jumpInsn->size() : 0));
        if ( diff > m_jumpInsn->max_distance() )
          throw std::runtime_error("Jump rewrite is too far");

        m_jumpInsn->relative(diff);
      }

      void setJump(std::unique_ptr<JumpInsn> &&insn) {
        m_jumpInsn = std::move(insn);
      }
      inline JumpInsn &jump() const { return *m_jumpInsn; }

      inline bool needed() const { return !m_body.str().empty(); }

      inline std::size_t size() const { return m_body.str().size(); }
      inline std::string data() const { return m_body.str(); }
      inline RtlBlockName destination() const { return m_destination.destBlock()->name(); }

      void finish(std::int64_t jumprel) {
        m_finalJump->relative(jumprel);
        m_finalJump->output(m_body);
      }

    private:
      std::unique_ptr<JumpInsn> m_jumpInsn, m_finalJump;
      std::uint64_t m_jumpPos;
      GenericScheduler &m_destination;

      std::stringstream m_body;
    };

    class Fixup {
    public:
      Fixup(std::uint64_t jumpPos, const RtlBlockName &destination,
            std::unique_ptr<JumpInsn> &&insn)
        : m_position(jumpPos), m_destination(destination),
          m_insn(std::move(insn)) {
      }

      std::uint64_t position() const { return m_position; }
      const RtlBlockName &destination() const { return m_destination; }
      JumpInsn &jump() const { return *m_insn; }

    private:
      std::uint64_t m_position;
      RtlBlockName m_destination;
      std::unique_ptr<JumpInsn> m_insn;
    };


    class EmitFixup : public EmitBytes {
    public:
      EmitFixup(Fixup &fu)
        : m_fixup(fu) {
      }

      virtual ~EmitFixup() {
      }

      virtual void emit(std::ostream &out) const {
        m_fixup.jump().output(out);
      }

    private:
      Fixup &m_fixup;
    };

    class EmitJumpRewrite : public EmitBytes {
    public:
      EmitJumpRewrite(JumpRewrite &rewrite)
        : m_rewrite(rewrite) {
      }
      virtual ~EmitJumpRewrite() {
      }

      virtual void emit(std::ostream &out) const {
        m_rewrite.jump().output(out);
      }

    private:
      JumpRewrite &m_rewrite;
    };

    class EmitJump : public EmitBytes {
    public:
      EmitJump(std::unique_ptr<JumpInsn> &&jump)
        : m_jump(std::move(jump)) {
      }
      virtual ~EmitJump() { }

      virtual void emit(std::ostream &out) const {
        m_jump->output(out);
      }

    private:
      std::unique_ptr<JumpInsn> m_jump;
    };

    class EmitBlock : public EmitBytes {
    public:
      EmitBlock(const std::string &bytes)
        : m_bytes(bytes) {
      }

      virtual ~EmitBlock() {
      }

      virtual void emit(std::ostream &out) const {
        out.write(m_bytes.data(), m_bytes.size());
      }

    private:
      const std::string &m_bytes;
    };

    class EmitPostamble : public EmitBytes {
    public:
      EmitPostamble(const InsnEncoder &e) {
        e.postamble(m_postamble);
        e.funret(m_postamble);
      }
      virtual ~EmitPostamble() {
      }

      virtual void emit(std::ostream &out) const {
        out.write(m_postamble.str().data(),
                  m_postamble.str().size());
      }

      inline std::size_t size() const { return m_postamble.str().size(); }

    private:
      std::stringstream m_postamble;
    };

    OpReadiness::OpReadiness(ReadinessTracker &t, SelectedInsn &i)
      : RtlOperandVisitor(t.function()), m_tracker(t), m_insn(i), m_ready(true) {
    }

    RtlFunction &OpReadiness::function() const {
      return tracker().function();
    }

    void OpReadiness::mark() {
      tracker().markReady(m_madeReady);
    }

    void OpReadiness::addOverlaps() {
      OverlapInserter t(tracker());
      m_insn.consumedOperations(t);
    }

    OverlapInserter::~OverlapInserter() {
      for ( RtlOp *op: m_ops ) {
        m_tracker.blacklist(op, m_ops.size());
      }
    }

    void OpReadiness::operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
      if ( input )
        checkVar(var);
      if ( output )
        markVar(var);
    }

    bool OpReadiness::checkVar(const std::shared_ptr<RtlVariable> &rawv) {
      auto v(tracker().translateVariable(rawv));
      if ( !v ) v = rawv;

      if ( m_madeReady.find(v->repr()) != m_madeReady.end() ) return true;
      else if ( tracker().isReady(v) ) return true;
      else {
        //        if ( m_ready ) std::cerr << "Disqualify because " << v->repr()->name() << " not ready" << std::endl;
        m_ready = false;
        return true;
      }
    }

    void OpReadiness::op(std::unique_ptr<RtlOp> &&op) {
      op->operands(*this);
    }

    void InterferenceChecker::matchOp(RtlOp *o) {
      m_ops.insert(o);
    }

    bool InterferenceChecker::interferes() const {
      return std::any_of(m_ops.begin(), m_ops.end(),
                         [this](RtlOp *op) {
                           return m_tracker.isBlacklisted(op, m_ops.size());
                         });
    }

    RtlFunction &ReadinessMarker::function() const {
      return tracker().function();
    }

    ReadinessMarker::~ReadinessMarker() {
      tracker().markReady(m_madeReady);
    }

    void ReadinessMarker::op(std::unique_ptr<RtlOp> &&op) {
      op->operands(*this);

      m_underlying.op(std::move(op));
    }

    void ReadinessMarker::operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
      if ( output )
        m_madeReady.insert(var);
    }

    GenericScheduler::GenericScheduler(GenericFunctionScheduler &dest, std::shared_ptr<RtlBasicBlock> block)
      : m_fn(dest), m_regs(block), m_isEntryPoint(false), m_srcBlock(block), m_destBlock(dest.function().block()) {

      for ( const auto &op : *block )
        m_ops.insert(op.get());

      // Copy over arguments
      for ( const auto &input: m_srcBlock->inputs() ) {
        auto newArg(m_destBlock->addInput(input->namePrefix(), input->type()));
        m_inputMap.insert(std::make_pair(input, newArg));
      }

      // Copy over jumps
      for ( const auto &jump : m_srcBlock->jumps() ) {
      }
    }

    GenericScheduler::~GenericScheduler() {
    }

    std::shared_ptr<RtlVariable> GenericScheduler::translateVariable(const std::shared_ptr<RtlVariable> &v) {
      auto it(m_inputMap.find(v->repr()));
      if ( it == m_inputMap.end() ) return nullptr;
      else return it->second->repr();
    }

    void GenericScheduler::restart() {
      m_selected.clear();
      m_patterns.clear();

      buildPatterns(*this);
    }

    void GenericScheduler::clearSchedulable() {
      m_selected.clear();
    }

    void GenericScheduler::generatePossibilities() {
      std::deque< std::unique_ptr<InsnSelRule> > thesePatterns;
      std::swap(m_patterns, thesePatterns);

      // Match rules until all rules are exhausted
      while ( !thesePatterns.empty() ) {
        auto rule(std::move(thesePatterns.front()));
        thesePatterns.pop_front();

        for ( RtlOp *op : m_ops ) {
          rule->apply(*this, *op);
        }
      }

      // Now, we should have schedulable instructions
    }

    void GenericScheduler::scheduleInsns() {
      ReadinessTracker ready(*this);
      for ( const auto &input: m_destBlock->inputs() )
        ready.markReady(input);

      while ( !m_ops.empty() ) {
        restart();
        ready.clearBlacklist();

        while ( !m_patterns.empty() )
          generatePossibilities();

        // Eliminate any selected instructions that are not ready, or
        // overlap with ones that are not ready, but are smaller
        decltype(m_selected) newSelected;
        for ( auto &selected: m_selected ) {
          auto opCheck(ready.check(*selected));
          if ( opCheck.ready() ) {
            newSelected.emplace_back(std::move(selected));
          } else {
            opCheck.addOverlaps();
          }
        }
        m_selected.clear();

        std::cerr << "After readiness cull: " << newSelected.size() << std::endl;

        for ( auto &selected: newSelected ) {
          // Now eliminate any operations that would overlap with non-ready ops
          if ( !ready.interferes(*selected) )
            m_selected.emplace_back(std::move(selected));
        }

        // TODO trerible
        if ( !newSelected.empty() && m_selected.empty() )
          m_selected = std::move(newSelected);

        // Find the best costing instruction
        std::unique_ptr<SelectedInsn> nextInsn;
        auto cost(std::numeric_limits<double>::max());
        std::cerr << "Got selected: " << m_selected.size() << std::endl;
        while ( !m_selected.empty() ) {
          std::unique_ptr<SelectedInsn> curInsn(std::move(m_selected.front()));
          m_selected.pop_front();

          InsnCostCalcer coster(function());
          coster.calc(*curInsn);

          double nextCost(coster.cost());
          std::cerr << "Cost is " << nextCost << " (cur: " << cost << ")" << std::endl;
          if ( nextCost < cost || !nextInsn ) {
            nextInsn = std::move(curInsn);
            cost = nextCost;
          }
        }

        if ( !nextInsn) {
          // TODO exception
          std::cerr << "Could not schedule block " << m_srcBlock->name().index() << std::endl;
          std::cerr << "Remaining ops were:" << std::endl;
          for ( auto op : m_ops ) {
            dumpOp(*op, m_srcBlock->function(), std::cerr);
            std::cerr << std::endl;
          }
          std::cerr << "Ops done" << std::endl;

          throw exception::BlockNotScheduled(m_srcBlock->name());
        }

        nextInsn->consumedOperations(*this);

        auto readinessMarker(ready.builder(*m_destBlock));
        TranslatingOpBuilder builder(*this, readinessMarker);
        nextInsn->build(builder);
        //        ready.markReady
      }

      //      reorderDataDeps(*m_destBlock);
    }

    RtlFunction &GenericScheduler::function() const {
      return m_srcBlock->function();
    }

    void GenericScheduler::addRule(std::unique_ptr<InsnSelRule> &&rule) {
      m_patterns.emplace_back(std::move(rule));
    }

    void GenericScheduler::markInsnSchedulable(std::unique_ptr<SelectedInsn> &&insn) {
      m_selected.emplace_back(std::move(insn));
    }

    void GenericScheduler::matchOp(RtlOp *op) {
      auto it(m_ops.find(op));
      if ( it == m_ops.end() ) {
        throw exception::OperationNotFound(op);
      }

      m_ops.erase(it);
    }

    /* Function schedule */
    GenericFunctionScheduler::GenericFunctionScheduler(RtlFunction &src)
      : m_function(src) {
    }

    GenericFunctionScheduler::~GenericFunctionScheduler() {
    }

    std::string GenericFunctionScheduler::allocName(const std::string &base) const {
      decltype(m_entries)::const_iterator it(m_entries.find(base));

      if ( it == m_entries.end() ) return base;

      for ( int i = 0; ; ++i ) {
        std::stringstream newName;
        newName << base << i;

        it = m_entries.find(newName.str());

        if ( it == m_entries.end() ) return newName.str();
      }

      return base;
    }

    void GenericFunctionScheduler::scheduleEntryPoint(std::shared_ptr<RtlBasicBlock> block, CallingConvention &cconv, const std::string &name) {
      std::string realName(allocName(name));
      std::list<GenericScheduler *> newBlocks;
      auto existing(m_blocks.find(block->name()));
      if ( existing != m_blocks.end() ) {
        std::cerr << "WARNING: entry point already exists" << std::endl;
        return;
      }

      std::unique_ptr<GenericScheduler> sched(makeBlockScheduler(block));
      sched->setEntryPoint();
      cconv(*sched->destBlock());

      sched->registers().assign(-1, cconv);

      auto dblockNm(sched->destBlock()->name());
      m_entries.emplace(realName, dblockNm);

      auto inserted(m_blocks.insert(std::make_pair(block->name(), std::move(sched))));
      m_destBlocks.emplace(dblockNm, inserted.first);

      scheduleBlockAndDeps(block, newBlocks);

      // Now, for each new block, copy over jumps
      for ( const auto &blk: newBlocks ) {
        for ( const auto &jump: blk->sourceBlock()->jumps() ) {
          auto newBlock(m_blocks.find(jump.second.to()));
          if ( newBlock == m_blocks.end() ) continue; // TODO

          auto &newJump(blk->destBlock()->jump(jump.first, newBlock->second->destBlock()->name()));
          for ( const auto &arg: jump.second.arguments() ) {
            auto newarg(blk->translateVariable(arg));
            if ( !newarg ) newarg = arg;

            newJump.push_argument(newarg);
          }
        }
      }
    }

    void GenericFunctionScheduler::scheduleEntryPoint(std::shared_ptr<RtlBasicBlock> block, const std::string &name) {
      auto cconv(defaultCallingConvention());
      scheduleEntryPoint(block, *cconv, name);
    }

    GenericScheduler &GenericFunctionScheduler::scheduleBlockAndDeps(std::shared_ptr<RtlBasicBlock> block,
                                                   std::list<GenericScheduler *> &newBlocks) {
      auto existing(std::find_if(newBlocks.begin(), newBlocks.end(),
                                 [&block](GenericScheduler *s) {
                                   return s->sourceBlock()->name() == block->name();
                                 }));
      GenericScheduler *ret;

      if ( existing == newBlocks.end() ) {
        auto it(m_blocks.find(block->name()));

        if ( it != m_blocks.end() ) {
          ret = it->second.get();
          ret->scheduleInsns();
        } else {
          auto newIt(m_blocks.insert(std::make_pair(block->name(), makeBlockScheduler(block))));
          m_destBlocks.emplace(newIt.first->second->destBlock()->name(), newIt.first);
          newIt.first->second->scheduleInsns();
          ret = newIt.first->second.get();
        }
      } else
        return **existing;

      newBlocks.push_back(ret);
      for ( const auto &succ : block->jumps() ) {
        scheduleBlockAndDeps(function().block(succ.second.to()), newBlocks);
      }

      return *ret;
    }

    GenericFunctionScheduler::iterator GenericFunctionScheduler::begin() {
      return iterator(m_blocks.begin());
    }

    GenericFunctionScheduler::iterator GenericFunctionScheduler::end() {
      return iterator(m_blocks.end());
    }

    void GenericFunctionScheduler::dump(std::ostream &out) const {
      for ( const auto &blk: m_blocks ) {
        out << "block" << blk.second->destBlock()->name().index() << "(";
        bool comma(false);
        RegisterTracker &regs(blk.second->registers());
        for ( const auto &i: blk.second->destBlock()->inputs() ) {
          if ( comma ) out << ", ";
          out << "(";
          i->type()->output(out);
          out << ") " << i->name() << "[";
          dumpAssignments(regs.assignment(-1, i), out);
          out << "]";
          comma = true;
        }
        out << "):" << std::endl;

        int lastCycle(0);
        for ( auto [it, i] = std::tuple { blk.second->destBlock()->begin(), 0 };
              it != blk.second->destBlock()->end();
              ++it, ++i) {
          out << "  " << i << ": ";
          RegistersOperandPrinter p(function(), blk.second->registers(), i, out);
          out << (*it)->mnemonic() << "(";
          (*it)->operands(p);
          out << ")" << std::endl;
          lastCycle = i;
        }

        for ( const auto &jump : blk.second->destBlock()->jumps() ) {
          if ( jump.first ) {
            out << "  call block" << jump.second.to().index() << "(";
            bool first(true);
            for ( const auto &arg: jump.second.arguments() ) {
              if ( !first ) out << ", ";
              out << "(";
              arg->type()->output(out);
              out << ") " << arg->name() << "[";
              dumpAssignments(regs.assignment(lastCycle, arg), out);
              out << "]";
              first = false;
            }
            out << ") when " << jump.first->name() << std::endl;
          }
        }

        if ( blk.second->destBlock()->has_jump() ) {
          const auto &jump(blk.second->destBlock()->jump());
          out << "  call block" << jump.to().index() << "(";
          bool first(true);
          for ( const auto &arg: jump.arguments() ) {
            if ( !first ) out << ", ";
            out << "(";
            arg->type()->output(out);
            out << ") " << arg->name() << "[";
            dumpAssignments(regs.assignment(lastCycle, arg), out);
            out << "]";
            first = false;
          }
          out << ")" << std::endl;
        } else
          out << "  return" << std::endl;
      }
    }

    GenericScheduler &GenericFunctionScheduler::blockScheduler(const RtlBlockName &nm) const {
      auto it(m_blocks.find(nm));
      if ( it == m_blocks.end() ) {
        auto dit(m_destBlocks.find(nm));
        if ( dit == m_destBlocks.end() ) {
          std::cerr << "Block " << nm.index() << " not scheduled" << std::endl;
          throw ::nnc::exception::BlockNotScheduled(nm);
        }

        return *(dit->second->second);
      }

      return *it->second;
    }

    std::deque<RtlBlockName> orderBlocksNoLoops(GenericFunctionScheduler &sched) {
      // Call graph without loops
      std::deque<RtlBlockName> ret;

      std::set<std::pair<RtlBlockName, RtlBlockName>> dagCallGraph;
      std::set<RtlBlockName> dagNodes;

      ::nnc::graph::dfs<GenericFunctionScheduler, ::nnc::graph::ParentTracker<GenericScheduler *>> dfs(sched);

      for ( auto &block: sched )
        if ( block.isEntryPoint() ) {
          dfs.enqueue(&block);
          dagNodes.insert(block.destBlock()->name());
        }

      while ( !dfs.empty() ) {
        auto h(dfs.next());

        if ( h.has_parent() ) {
          auto par(h.parent()->destBlock());
          auto me(h.child()->destBlock());

          dagCallGraph.emplace(par->name(), me->name());
          dagNodes.insert(par->name());
          dagNodes.insert(me->name());
        }
      }

      auto cgr(::nnc::graph::adjList(dagCallGraph));
      return ::nnc::graph::toposort< std::deque<RtlBlockName> > (cgr);
    }

    void GenericFunctionScheduler::encode(std::ostream &out) {
      std::map<RtlBlockName, std::ptrdiff_t> entries;
      encode(out, entries);
    }

    void GenericFunctionScheduler::encode(std::ostream &out, std::map<RtlBlockName, std::ptrdiff_t> &entries) {
      std::map<RtlBlockName, std::string> blocks;
      const InsnEncoder &e(defaultInsnEncoder());

      // TODO, make sure to return entry point addresses

      for ( const auto &sched: *this ) {
        std::stringstream encoded;
        encodeBasicBlock(encoded, *sched.destBlock(), sched.registers(), e);
        blocks.emplace(sched.destBlock()->name(), encoded.str());
      }

      // Where existing blocks have been scheduled already
      std::map<RtlBlockName, std::uint64_t> positions;
      std::list<Fixup> fixups;
      std::list<JumpRewrite> jumpRewrites;
      std::list<std::unique_ptr<EmitBytes>> emits;
      std::uint64_t pos(0);

      for ( iterator i(begin());
            i != end();
            ++i ) {
        const auto &sched(*i);

        auto it(blocks.find(sched.destBlock()->name()));
        if ( it == blocks.end() ) continue;

        positions.emplace(sched.destBlock()->name(), pos);

        // Now calculate the jump length, based on conditions. For
        // each jump, if the block already exists, then we just do the
        // jump. Otherwise, we emit a 'fixup', where we go back and do
        // the jump.

        emits.emplace_back(std::make_unique<EmitBlock>(it->second));
        pos += it->second.size();

        bool had_default(false);

        for ( const auto &jump: sched.destBlock()->jumps() ) {
          auto target(blocks.find(jump.second.to()));
          if ( target == blocks.end() ) continue;

          if ( !jump.first )
            had_default = true;

          std::optional<Register> cond;
          auto mapper(sched.registers().mapper(sched.destBlock()->endTime()));
          if ( jump.first ) {
            cond = chooseRegister(mapper, e.jump_cond_class(), jump.first);
          }

          auto targetPos(positions.find(jump.second.to()))g + r10
          // Mapper contains the positions of the last variables
          GenericScheduler &destination(blockScheduler(jump.second.to()));
          JumpRewrite rewrites(e, jump.second, pos, mapper, destination);
          if ( rewrites.needed() ) {
            auto generic_jump(e.generic_jump(cond));
            if ( cond ) {
              auto &rewrite(jumpRewrites.emplace_back(std::move(rewrites)));

              std::size_t jumpsz(generic_jump->size());
              pos += jumpsz;

              rewrite.setJump(std::move(generic_jump));
              emits.emplace_back(std::make_unique<EmitJumpRewrite>(rewrite));
              continue;
            } else {
              // Emit rewrite right here
              pos += rewrites.size();
              emits.emplace_back(std::make_unique<EmitString>(rewrites.data()));
            }
          }

          if ( (i + 1) != end() &&
               !jump.first && jump.second.to() == (i + 1)->destBlock()->name() ) {
            // No jump needed
          } else if ( targetPos == positions.end() ) {
            // Target position not found, emit the largest jump
            auto generic_jump(e.generic_jump(cond));

            std::size_t jumpsz(generic_jump->size());
            pos += jumpsz;

            auto &fixup(fixups.emplace_back(pos, jump.second.to(), std::move(generic_jump)));
            emits.emplace_back(std::make_unique<EmitFixup>(fixup));
          } else {
            auto rel_jump(e.jump_relative(targetPos->second - pos, cond));

            pos += rel_jump->size();
            emits.emplace_back(std::make_unique<EmitJump>(std::move(rel_jump)));
          }
        }

        if ( !had_default ) {
          auto postamble(std::make_unique<EmitPostamble>(e));
          pos += postamble->size();
          emits.emplace_back(std::move(postamble));
        }
      }

      for ( const auto &fixup: fixups ) {
        auto position(positions.find(fixup.destination()));
        if ( position == positions.end() )
          throw std::runtime_error("Could not find jump target");

        std::int64_t diff(position->second - fixup.position());
        if ( diff > fixup.jump().max_distance() )
          throw std::runtime_error("Jump is too far to work");

        fixup.jump().relative(diff);
      }

      for ( auto &rewrite: jumpRewrites ) {
        auto position(positions.find(rewrite.destination()));
        if ( position == positions.end() )
          throw std::runtime_error("Could not find jump target");

        rewrite.setPosition(pos);
        std::int64_t diff(position->second - pos - rewrite.size());
        rewrite.finish(diff);

        pos += rewrite.size();
        emits.emplace_back(std::make_unique<EmitString>(rewrite.data()));
      }

      e.preamble(out);
      for ( const auto &emit: emits )
        emit->emit(out);

      std::copy(std::begin(positions), std::end(positions),
                std::inserter(entries, entries.begin()));
    }
  }
}
