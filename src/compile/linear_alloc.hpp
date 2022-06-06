#ifndef __nnc_compile_linear_alloc_HPP__
#define __nnc_compile_linear_alloc_HPP__

#include "compile/rtl_ops_base.hpp"
#include "compile/regset.hpp"

namespace nnc::compile {
  class GenericScheduler;

  class LinearAllocator {
  public:
    struct LiveRange {
      inline LiveRange(RegisterFile &file) : regs(file) { }
      LiveRange(LiveRange &&r) =default;
      LiveRange(const LiveRange &r) =default;

      LiveRange &operator=(const LiveRange &o) =default;
      LiveRange &operator=(LiveRange &&r) =default;

      void tie(const LiveRange &o, const RegClass &rc);
      void assignRegister(const VirtualRegister &vr);

      RtlVariablePtr var;
      int start, end;
      regset regs;

      std::set<VirtualRegister> assigned;

      RtlVariablePtr tied;
      int tied_to;
    };

    LinearAllocator(GenericScheduler &sched);

    inline GenericScheduler &scheduler() const { return m_sched; }
    inline RtlBasicBlock &block() const { return *m_sched.destBlock(); }

    void operator() ();

  protected:
    void buildRanges();

  private:
    GenericScheduler &m_sched;
    std::vector<LiveRange> m_ranges;
  };
}

#endif
