#ifndef __nnc_compile_linear_alloc_HPP__
#define __nnc_compile_linear_alloc_HPP__

#include <map>

#include "compile/rtl_ops_base.hpp"
#include "compile/regset.hpp"
#include "compile/registers.hpp"
#include "compile/insnsched.hpp"

namespace nnc::compile {
  class ActiveRegisterSet;

  class LinearAllocator {
  public:
  struct LiveRange {
    inline LiveRange(RegisterFile &file, int time, const RtlVariablePtr &v)
        : regs(regset::full(file)), var(v->repr()), start(time), end(time),
          constrained(false) {}
    LiveRange(LiveRange &&r) = default;
    LiveRange(const LiveRange &r) = default;

    LiveRange &operator=(const LiveRange &o) = default;
    LiveRange &operator=(LiveRange &&r) = default;

    void tie(const LiveRange &o, const RegClass &rc);
    void assignRegister(const VirtualRegister &vr);

    RtlVariablePtr var;
    int start, end;
    regset regs;

    std::set<VirtualRegister> assigned;

    RtlVariablePtr tied;
    int tied_to;

    bool constrained;
  };

  LinearAllocator(const RtlBasicBlock &block, RegisterFile &regs);

  inline const RtlBasicBlock &block() const { return m_block; }
  inline RegisterTracker &tracker() { return m_tracker; }
  inline const RegisterTracker &tracker() const { return m_tracker; }

  void operator()();

  typedef std::multimap<int, LiveRange *> ActiveRanges;

protected:
  void buildRanges();
  virtual void spill(LiveRange &rg, ActiveRanges &ranges,
                     RegisterTracker &tracker, std::list<LiveRange *> &liveRanges,
                     ActiveRegisterSet &usedRegisters);

  int reinsertActiveRange(std::list<LiveRange *> &ranges, LiveRange &rg, int after);
  void sortRanges(std::list<LiveRange *> &ranges);

private:
  const RtlBasicBlock &m_block;
  RegisterFile &m_registers;
  RegisterTracker m_tracker;
  std::list<LiveRange> m_ranges;
  int m_nextSpillSlot;
};
}

#endif
