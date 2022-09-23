#ifndef __nnc_compile_encoding_HPP__
#define __nnc_compile_encoding_HPP__

#include "compile/rtl.hpp"
#include "compile/registers.hpp"

#include <iostream>
#include <optional>

namespace nnc::exception {
  class InvalidOpcode : public std::exception {
  public:
    InvalidOpcode(const char *msg);
    virtual ~InvalidOpcode();

    virtual const char *what() const noexcept;

  private:
    const char *m_msg;
  };
};

namespace nnc::compile {
  class JumpInsn {
  public:
    virtual std::int64_t max_distance() const =0;
    virtual std::size_t size() const =0;

    virtual void relative(std::int64_t i) =0;
    virtual void absolute(std::uint64_t i) =0;

    virtual void output(std::ostream &o) const =0;
  };

  class BytecodeInfo {
  public:
    enum Section {
      CodeSection,
      DataSection,
    };

    /** Get the start of the given section */
    virtual std::uintptr_t section_start(Section section) const =0;

    /** Where in the bytecode this fixup was found */
    //    virtual std::pair<Section, std::size_t> location() const =0;
  };

  class BytecodeFixup {
  public:
    virtual ~BytecodeFixup();

    virtual void fixup(BytecodeInfo &info, std::uint8_t *buf, std::size_t sz) =0;
    virtual std::size_t size() const =0;
  };

  class BytecodeEmitter {
  public:
    virtual void write_bytes(BytecodeInfo::Section section, const std::uint8_t *d, std::size_t sz) = 0;
    virtual void write_fixup(BytecodeInfo::Section section, std::unique_ptr<BytecodeFixup> &&f) =0;
    virtual std::uintptr_t allocate_constant(const std::span<std::uint8_t> &c) =0;
  };

  // Virtual class for generic instructoin encoding (for spills and jumps)
  class InsnEncoder {
  public:
    virtual std::unique_ptr<JumpInsn> jump_relative(std::int64_t offs, std::optional<Register> condition) const =0;
    virtual std::unique_ptr<JumpInsn> jump_absolute(std::int64_t abs, std::optional<Register> condition) const =0;

    virtual const RegClass &indirect_jump_class() const =0;
    virtual std::unique_ptr<JumpInsn> jump_indirect(Register r, std::optional<Register> condition) const =0;

    virtual std::unique_ptr<JumpInsn> generic_jump(std::optional<Register> condition) const =0;
    virtual void funret(BytecodeEmitter &out) const =0;

    virtual const RegClass &jump_cond_class() const =0;

    virtual void preamble(BytecodeEmitter &out) const;
    virtual void postamble(BytecodeEmitter &out) const;

    virtual void copy(BytecodeEmitter &out, const RtlRegisterMapper &inputs,
                      const RtlVariablePtr &var, const VirtualRegister &dest) const =0;
  };

  class RtlEmitOp {
  public:
    virtual void encode(const RtlRegisterMapper &inputs, const RtlRegisterMapper &outputs, BytecodeEmitter &out) const =0;
  };

  void
  encodeBasicBlock(BytecodeEmitter &out, const RtlBasicBlock &block,
                   const RegisterTracker &t, const InsnEncoder &e);
//  inline void encodeBasicBlock(std::ostream &out, const GenericScheduler &c, const InsnEncoder &e) {
//    encodeBasicBlock(out, *c.destBlock(), c.registers(), e);
//  }
}

#endif
