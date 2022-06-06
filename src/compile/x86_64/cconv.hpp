#ifndef __nnc_compile_x86_64_cconv_HPP__
#define __nnc_compile_x86_64_cconv_HPP__

#include "compile/cconv.hpp"
#include "compile/rtl_types.hpp"
#include "compile/rtl.hpp"

namespace nnc::arch::x86_64::cconv {
  // Linux x86-64 System V ABI: https://refspecs.linuxbase.org/elf/x86_64-abi-0.21.pdf
  class ccall : public ::nnc::compile::CallingConvention {
  public:
    ccall();
    virtual ~ccall();

  protected:
    virtual ::nnc::compile::VirtualRegister assignRegToArg(const std::shared_ptr<::nnc::compile::RtlVariable> & v);

    // Taken from ABI document
    enum ArgClass {
      NO_CLASS,
      INTEGER,
      SSE,
      SSEUP,
      X87,
      X87UP,
      MEMORY
    };

    ArgClass classifyArg(const ::nnc::compile::RtlTypeInfo &ty, std::size_t &sz);

    ::nnc::compile::VirtualRegister assignIntReg();
    ::nnc::compile::VirtualRegister assignSseReg();
    ::nnc::compile::VirtualRegister assignStackSlot(int sz);

  private:
    int m_gpreg, m_xmmreg, m_stackslot;
  };
};

#endif
