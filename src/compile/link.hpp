#ifndef __nnc_compile_link_HPP__
#define __nnc_compile_link_HPP__

#include <iostream>
#include <cstdint>
#include <memory>

#include "compile/rtl.hpp"
#include "compile/registers.hpp"
#include "compile/regalloc.hpp"
#include "compile/encoding.hpp"

namespace nnc::compile {
  class Linker {
  public:
    Linker(const InsnEncoder &encoder);

    inline const InsnEncoder &encoder() const { return m_encoder; }

    inline std::uint64_t blockPosition(const RtlBasicBlock &block) const {
      return blockPosition(block.function(), block.name());
    }

    std::uint64_t blockPosition(const RtlFunction &fn, const RtlBlockName &nm) const;

    void placeBlock(const RtlBasicBlock &block, RegisterAllocatorBase &trackers);
    void finish();

    void dump(std::ostream &out, std::uintptr_t base_addr = 0) const;

    void writeSection(BytecodeInfo::Section section, const std::uint8_t *d, std::size_t sz);
    std::uintptr_t sectionOffset(BytecodeInfo::Section section) const;

    inline size_t size() const { return m_totalSize; }

    class EmitBytes {
    public:
      virtual ~EmitBytes() { }

      virtual void emit(std::ostream &out, std::uintptr_t base_addr) const =0;
      virtual std::size_t size() const =0;

      virtual void fixup(BytecodeInfo &info, std::uint8_t *buf, std::size_t sz, std::uintptr_t base_addr);
    };

  private:
    class Fixup {
    public:
      Fixup(std::uint64_t jumpPos, const RtlBasicBlock &destination,
            std::unique_ptr<JumpInsn> &&insn);

      inline std::uint64_t position() const { return m_position; }
      inline const RtlBasicBlock &destination() const { return m_destination; }
      inline JumpInsn &jump() const { return *m_insn; }

    private:
      std::uint64_t m_position;
      const RtlBasicBlock &m_destination;
      std::unique_ptr<JumpInsn> m_insn;
    };

    class JumpRewrite {
    public:
      JumpRewrite(JumpRewrite &&r) =default;
      JumpRewrite(const InsnEncoder &e,
                  const RtlJump &jump,
                  std::uint64_t pos,
                  const std::multimap<RtlVariablePtr, VirtualRegister> &actual,
                  const RtlRegisterMapper &expected,
                  const RtlBasicBlock &destination);

      void setJumpTarget(std::uint64_t tgt, std::uint64_t rewritePos) const;
      void setJump(std::unique_ptr<JumpInsn> &&insn);

      inline JumpInsn &jump() const { return *m_jumpInsn; }
      inline std::uint64_t jumpPos() const { return m_jumpPos; }

      inline bool needed() const { return !m_body.empty(); }
      inline std::size_t size() const { return m_body.size(); }
      inline const std::string &data() const { return m_body; }

      inline const RtlBasicBlock &destination() const { return m_destination; }

      void finish(std::int64_t jumprel);

    private:
      std::unique_ptr<JumpInsn> m_jumpInsn, m_finalJump;
      std::uint64_t m_jumpPos;
      const RtlBasicBlock &m_destination;

      std::string m_body;
    };

    template<typename T, typename... Args>
    T &plan(BytecodeInfo::Section section, Args&&... args);

    void dumpSection(BytecodeInfo::Section section, std::ostream &out, std::uintptr_t &base_addr) const;
    void fixupSection(BytecodeInfo &info,
                      BytecodeInfo::Section section,
                      std::uint8_t *base) const;

    void discardConditionalJump(const RtlBasicBlock &to, const RegisterTracker &tracker);
    void insertConditionalJump(const RtlBasicBlock &to, const RegisterTracker &tracker);

    friend class EmitJumpRewrite;
    friend class EmitFixup;

    const InsnEncoder &m_encoder;
    std::uint64_t m_position, m_totalSize;
    std::map< std::pair<const RtlFunction *, RtlBlockName>, std::uint64_t > m_positions;
    std::list<Fixup> m_fixups;
    std::list<JumpRewrite> m_jumpRewrites;
    std::map<BytecodeInfo::Section, std::list<std::unique_ptr<EmitBytes>>> m_plan;

    struct ConditionalJump {
      ConditionalJump(const RtlBasicBlock &block,
                      const RtlBasicBlock &d,
                      const RegisterTracker &tracker,
                      const RtlJump &jump,
                      std::unique_ptr<JumpInsn> &&insn);

      RtlFunction *function;
      const RtlJump &jump;
      const RtlBasicBlock &dest;
      std::unique_ptr<JumpInsn> insn;
      std::multimap<RtlVariablePtr, VirtualRegister> args;
    };
    std::optional<ConditionalJump> m_conditionalJump;
  };
}

#endif
