#include "compile/regalloc.hpp"
#include "compile/cconv.hpp"

#include <iostream>

namespace nnc::compile {
  RegisterTracker::RegisterTracker(const std::shared_ptr<RtlBasicBlock> &b)
    : m_block(b) {
  }

  void RegisterTracker::assign(int time, const RtlVariablePtr &base, const VirtualRegister &reg) {
    auto var (canonicalize(base));

    int minSize(time + 2);
    if ( m_states.size() < minSize )
      m_states.resize(minSize);

    m_states[time + 1].assign(var, reg);
  }

  void RegisterTracker::assign(int time, const CallingConvention &conv) {
    for ( const auto &slot: conv.argumentSlots() ) {
      assign(time, slot.first, slot.second);
    }
  }

  void RegisterTracker::unassign(int time, const RtlVariablePtr &base, const VirtualRegister &reg) {
    int minSize(time + 2);
    if ( m_states.size() < minSize )
      return;
    auto var(canonicalize(base));

    m_states[time + 1].unassign(var, reg);
  }

  RtlVariablePtr RegisterTracker::canonicalize(const RtlVariablePtr &p) const {
    auto it( m_aliases.find(p) );
    if ( it == m_aliases.end() ) return p;
    else return canonicalize(it->second);
  }

  RtlVariablePtr RegisterTracker::canonicalize(const RtlVariablePtr &p) {
    auto it( m_aliases.find(p) );
    if ( it == m_aliases.end() ) return p;
    else {
      auto n(canonicalize(it->second));
      it->second = n;
      return n;
    }
  }

  void RegisterTracker::alias(const RtlVariablePtr &src, const RtlVariablePtr &dest) {
    m_aliases.emplace(dest, src);
  }

  std::vector<VirtualRegister> RegisterTracker::assignment(int time, const RtlVariablePtr &base) const {
    if ( m_states.size() == 0 ) {
      std::vector<VirtualRegister> empty;
      return empty;
    }

    auto var(canonicalize(base));

    auto end(m_states.begin());
    time += 1;
    if ( time >= m_states.size() )
      time = m_states.size() - 1;

    end += time + 1;

    std::set<VirtualRegister> regs;
    for ( auto it(m_states.begin());
          it != end;
          ++it ) {
      auto [unassignedBegin, unassignedEnd] = it->unAssignments.equal_range(var);
      if ( unassignedBegin != it->unAssignments.end() ) {
        for ( auto unassignment(unassignedBegin);
              unassignment != unassignedEnd;
              ++unassignment ) {
          auto it(regs.find(unassignment->second));
          if ( it != regs.end() )
            regs.erase(it);
        }
      }

      auto [ assignBegin, assignEnd ] = it->newAssignments.equal_range(var);
      if ( assignBegin != it->newAssignments.end() ) {
        for ( auto assignment(assignBegin);
              assignment != assignEnd;
              ++assignment ) {
          regs.insert(assignment->second);
        }
      }
    }

    std::vector<VirtualRegister> regsList;
    std::copy(regs.begin(), regs.end(), std::back_inserter(regsList));
    return regsList;
  }

  void RegState::assign(const RtlVariablePtr &var, const VirtualRegister &reg) {
    auto [s, e] = newAssignments.equal_range(var);
    if ( std::find_if(s, e, [reg](const auto &r) { return r.second == reg; }) == e )
      newAssignments.emplace(var, reg);
  }

  void RegState::unassign(const RtlVariablePtr &var, const VirtualRegister &reg) {
    auto [s, e] = unAssignments.equal_range(var);
    if ( std::find_if(s, e, [reg](const auto &r) { return r.second == reg; }) == e )
      unAssignments.emplace(var, reg);
  }

  RegisterTrackerMapper::RegisterTrackerMapper(const RegisterTracker &t, int cycle)
    : m_tracker(t),
      m_cycle(cycle) {
  }

  RegisterTrackerMapper::~RegisterTrackerMapper() {
  }

  std::vector<VirtualRegister> RegisterTrackerMapper::lookupVar(const RtlVariablePtr &v) const {
    return m_tracker.assignment(m_cycle, v);
  }
}
