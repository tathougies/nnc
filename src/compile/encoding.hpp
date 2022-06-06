#ifndef __nnc_compile_encoding_HPP__
#define __nnc_compile_encoding_HPP__

#include "compile/rtl.hpp"
#include "compile/registers.hpp"
#include "compile/genericschedule.hpp"

#include <iostream>

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
  class RtlEmitOp {
  public:
    virtual std::ostream &encode(const RtlRegisterMapper &inputs, const RtlRegisterMapper &outputs, std::ostream &out) const =0;
  };

  class JumpInsn {
  public:
    virtual std::int64_t max_distance() const =0;
    virtual std::size_t size() const =0;

    virtual void relative(std::int64_t i) =0;
    virtual void absolute(std::uint64_t i) =0;

    virtual void output(std::ostream &o) const =0;
  };

  // Virtual class for generic instructoin encoding (for spills and jumps)
  class InsnEncoder {
  public:
    virtual std::unique_ptr<JumpInsn> jump_relative(std::int64_t offs, std::optional<Register> condition) const =0;
    virtual std::unique_ptr<JumpInsn> jump_absolute(std::int64_t abs, std::optional<Register> condition) const =0;

    virtual const RegClass &indirect_jump_class() const =0;
    virtual std::unique_ptr<JumpInsn> jump_indirect(Register r, std::optional<Register> condition) const =0;

    virtual std::unique_ptr<JumpInsn> generic_jump(std::optional<Register> condition) const =0;
    virtual void funret(std::ostream &out) const =0;

    virtual const RegClass &jump_cond_class() const =0;

    virtual void preamble(std::ostream &out) const;
    virtual void postamble(std::ostream &out) const;

    virtual void copy(std::ostream &out, const RtlRegisterMapper &inputs, const RtlVariablePtr &var, const VirtualRegister &dest) const =0;
  };

  void encodeBasicBlock(std::ostream &out, const RtlBasicBlock &block, const RegisterTracker &t, const InsnEncoder &e);
  inline void encodeBasicBlock(std::ostream &out, const GenericScheduler &c, const InsnEncoder &e) {
    encodeBasicBlock(out, *c.destBlock(), c.registers(), e);
  }
}

#endif
