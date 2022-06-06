#ifndef __nnc_compile_cconv_HPP__
#define __nnc_compile_cconv_HPP__

#include <list>
#include "compile/registers.hpp"
#include "compile/rtl.hpp"

namespace nnc::compile {
  class CallingConvention {
  public:
    virtual ~CallingConvention();

    /**
     * Apply this calling convention to the given basic block in the
     * given tracker
     */
    void operator() (RtlBasicBlock &b);

    typedef std::list<std::pair<RtlVariablePtr, VirtualRegister>> slots_type;
    /**
     * Get the sequence of virtual regs necessary to call this
     * function
     */
    inline const slots_type &argumentSlots() const { return m_args; };

    VirtualRegister regForArg(const RtlVariablePtr &v);

  protected:
    /*
     * Retrieve the register assigned by the calling convention for this variable.
     *
     * This should track how many arguments have been included in the
     * signature and assign the variable appropriately
     */
    virtual VirtualRegister assignRegToArg(const std::shared_ptr<RtlVariable> &v) =0;

    /**
     * Build a preamble for the given basic block
     */
    virtual void buildPreamble(RtlOpBuilder &b) const;

    /**
     * Build a postamble for the given basic block
     */
    virtual void buildPostamble(RtlOpBuilder &b) const;

  private:
    slots_type m_args;
  };
}

#endif
