#ifndef __nnc_compile_unused_HPP__
#define __nnc_compile_unused_HPP__

#include "compile/rtl.hpp"
#include <memory>

namespace nnc::compile {
  class UnusedVarCuller {
  public:
    UnusedVarCuller(RtlFunction &fn);

    void cullBlock(RtlBasicBlock &block);
    void operator() ();

    inline RtlFunction &function() const { return m_function; }

  private:
    RtlFunction &m_function;
  };
}

#endif
