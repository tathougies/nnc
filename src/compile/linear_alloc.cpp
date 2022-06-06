#include "compile/genericschedule.hpp"
#include "compile/linear_alloc.hpp"
#include "compile/regalloc.hpp"
#include "compile/registers.hpp"

#include <functional>
#include <iostream>

namespace nnc::compile {
  class ActiveRegisterSet {
  public:
    ActiveRegisterSet(RegisterFile &f)
      : m_file(f) {
    }

    void ref(const Register &r) {
      auto it(m_data.find(r));
      if ( it == m_data.end() )
        m_data.emplace(r, 1);
      else
        it->second++;
    }

    void deref(const Register &r) {
      auto it(m_data.find(r));
      if ( it != m_data.end() ) {
        it->second--;
        if ( it->second == 0 )
          m_data.erase(it);
      }
    }

    bool isActive(const Register &r) const {
      auto it(m_data.find(r));
      return it != m_data.end() && it->second > 0;
    }

    std::size_t size() const { return m_data.size(); }

    regset set() const {
      regset r(m_file);
      for ( const auto &it : m_data ) {
        if ( it.second > 0 )
          r.insert(it.first);
      }
      return r;
    }

  private:
    RegisterFile &m_file;
    std::map<Register, int> m_data;
  };

  class RegisterCollector : public RegClassMembers {
  public:
    RegisterCollector(RegisterFile &file): m_regs(file) { }
    virtual ~RegisterCollector() { }

    inline regset &set() { return m_regs; }
    inline const regset &set() const { return m_regs; }

    void operator() (const RegClass &c) {
      c.visitRegisters(*this);
    }

    virtual void reg(Register r) {
      m_regs.insert(r);
    }

  private:
    regset m_regs;
  };

  template<typename Ranges>
  class LifetimeTracker : public RtlOperandVisitor, public RegClassDeclarer  {
  public:
    LifetimeTracker(RtlFunction &fn, RegisterTracker &t, RegisterFile &file, Ranges &ranges)
      : RtlOperandVisitor(fn), m_curTime(-1), m_regFile(file), m_ranges(ranges), m_tracker(t) {
    }

    virtual ~LifetimeTracker() {
      while ( !m_lifetimes.empty() ) {
        auto it(m_lifetimes.begin());
        if ( it->second.start < it->second.end ) {
          m_ranges.push_back(it->second);
        }
        m_lifetimes.erase(it);
      }
    }

    void time(int t) { m_curTime = t; }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
      auto it(m_lifetimes.find(var));
      if ( it == m_lifetimes.end() ) {
        startLifetime(var);
      } else {
        it->second.end = m_curTime;
      }

      auto canon(m_tracker.canonicalize(var));
      std::cerr << "Canonicalize " << var->name() << " to " << canon->name() << " (at " << m_curTime << ")" << std::endl;
      if ( canon != var ) operand(name, canon, input, output); // Extend the lifetime of this variable too
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

    virtual void regclass(const std::string &name, const RtlVariablePtr &var, const RegClass &cls) {
      // Apply the class constraints to the current register set
      auto it(m_lifetimes.find(var));

      std::cerr << "Handle reg class for " << var->name() << std::endl;
      if ( it != m_lifetimes.end() ) {
        RegisterCollector regs(m_regFile);
        regs(cls);
        regset newset(it->second.regs & regs.set());
        if ( newset.empty() ) {
          // End this lifetime early
          if ( it->second.start < it->second.end ) {
            std::cerr << "Cut variable " << var->name() << " short at " << m_curTime << std::endl;
            std::cerr << "Old regs were ";
            std::copy(it->second.regs.begin(), it->second.regs.end(), std::ostream_iterator<Register>(std::cerr, " "));
            std::cerr << std::endl;
            std::cerr << "New ones were ";
            std::copy(regs.set().begin(), regs.set().end(),
                      std::ostream_iterator<Register>(std::cerr, " "));
            std::cerr << std::endl;
            m_ranges.push_back(it->second);
          }

          it->second.start = it->second.end = m_curTime;
          it->second.regs = regs.set();
        } else
          it->second.regs = newset;
      }
    }

    virtual void intersect(const RtlVariablePtr &var1, const RtlVariablePtr &var2, const RegClass &cls) {
      auto lifetime1(m_lifetimes.find(var1));
      auto lifetime2(m_lifetimes.find(var2));
      if ( lifetime1 == m_lifetimes.end() ) {
        startLifetime(var1);
        lifetime1 = m_lifetimes.find(var1);
        lifetime1->second.start = m_curTime - 1;
      }

      if ( lifetime2 == m_lifetimes.end() ) {
        startLifetime(var2);
        lifetime2 = m_lifetimes.find(var2);
        lifetime2->second.start = m_curTime - 1;
      }

      if ( lifetime2->second.start >= lifetime1->second.start )
        lifetime2->second.tie(lifetime1->second, cls);

      if ( lifetime1->second.start >= lifetime2->second.start )
        lifetime1->second.tie(lifetime2->second, cls);
    }

//    void alias(const RtlVariablePtr &src, const RtlVariablePtr &dest) {
//      auto it(m_lifetimes.find(src));
//    }

  private:
    void startLifetime(const RtlVariablePtr &var) {
      using namespace std::placeholders;
      LinearAllocator::LiveRange rg(m_regFile);
      rg.var = var;
      rg.start = rg.end = m_curTime;

      auto existing(m_tracker.mapper(m_curTime).lookupVar(var));
      // These are hard requirements
      std::for_each(std::begin(existing), std::end(existing),
                    std::bind(&LinearAllocator::LiveRange::assignRegister, &rg, _1));

      m_lifetimes.emplace(var, std::move(rg));
    }

    int m_curTime;
    RegisterFile &m_regFile;
    std::map<RtlVariablePtr, LinearAllocator::LiveRange> m_lifetimes;

    Ranges &m_ranges;
    RegisterTracker &m_tracker;
  };

  class RegClassDumper : public RegClassDeclarer {
  public:
    RegClassDumper(RtlOp &op) {
      std::cerr << "Dump " << op.mnemonic() << std::endl;
    }
    virtual ~RegClassDumper() {
    }

    virtual void regclass(const std::string &argNm, const RtlVariablePtr &var, const RegClass &cls) {
      std::cerr << "  Arg " << argNm << ": " << cls.name() << std::endl;
    }
  };

  LinearAllocator::LinearAllocator(GenericScheduler &sched)
    : m_sched(sched) {
  }

  void LinearAllocator::buildRanges() {
    LifetimeTracker tracker(m_sched.function(), m_sched.registers(), m_sched.functionScheduler().registers(), m_ranges);

    for ( const auto &arg: block().inputs() ) {
      tracker.operand(arg->name(), arg, true, false);
    }

    int i(0);
    for ( const auto &op: block() ) {
      tracker.time(i);
      op->operands(tracker);
      try {
        const RtlRegisterOp &regOp(dynamic_cast<const RtlRegisterOp &>(*op));
        regOp.regclasses(tracker);
      } catch ( std::bad_cast &e ) {
        try {
          const RtlAliasOp &aliasOp(dynamic_cast<const RtlAliasOp &>(*op));
          m_sched.registers().alias(aliasOp.src(), aliasOp.dest());
        } catch ( std::bad_cast &e ) {
         }
      }
      i++;
    }

    tracker.time(i);
    for ( const auto &jump: block().jumps() ) {
      if ( jump.first ) {
        tracker.operand(jump.first->name(), jump.first, true, false);
      }
      for ( const auto &arg: jump.second.arguments() ) {
        tracker.operand(arg->name(), arg, true, false);
      }
    }
  }

  void LinearAllocator::LiveRange::tie(const LiveRange &o, const RegClass &rc) {
    tied = o.var;
    tied_to = o.start;

    RegisterCollector rs(rc.registers());
    rs(rc);
    regs = rs.set();
  }

  void LinearAllocator::LiveRange::assignRegister(const VirtualRegister &vr) {
    assigned.insert(vr);
    if ( vr.isRegister() )
      regs.insert(vr.reg());
  }

  void LinearAllocator::operator() () {
    m_ranges.clear();
    buildRanges();

    std::sort(m_ranges.begin(), m_ranges.end(),
              [](const auto &a, const auto &b) {
                if ( a.start == b.start ) {
                  if ( a.assigned.empty() && b.assigned.empty() ) {
                    if ( a.end == b.end ) {
                      return a.var < b.var;
                    } else return a.end < b.end;
                  } else if ( a.assigned.empty() ) {
                    return false;
                  } else if ( b.assigned.empty() ) {
                    return true;
                  } else
                    return a.var < b.var;
                } else
                  return a.start < b.start;
              });

    // Now go over each lifetime. If we already have an allocation in the tracker, assign it
//    for ( auto &rg: m_ranges ) {
//      const auto &assigned(m_sched.registers().assignment(rg.start, rg.var));
//      std::for_each(assigned.begin(), assigned.end(), std::inserter(rg.assigned, rg.assigned.end()));
//    }

    // TODO range generation should be separated from allocatino

    // Now assign all ranges
    //
    // Go over all ranges, push onto end time map
    int nextCycle(0);
    std::map<int, LiveRange*> activeRanges;
    ActiveRegisterSet usedRegisters(m_sched.functionScheduler().registers());

    for ( auto &rg: m_ranges ) {
      std::cerr << "Allocating " << rg.var->name() << std::endl;
      // End any ranges that have ended by the time we start
      auto irrelevant(activeRanges.upper_bound(rg.start - 1));
      for ( auto it(activeRanges.begin());
            it != irrelevant;
            ++it ) {
        for ( const auto &vreg: it->second->assigned ) {
          if ( !vreg.isSpill() &&
               m_sched.registers().canonicalize(it->second->var) == it->second->var ) {
            usedRegisters.deref(vreg.reg());
          }
          m_sched.registers().unassign(it->second->end + 1, it->second->var, vreg);
        }
      }
      nextCycle = rg.start + 1;

      activeRanges.erase(activeRanges.begin(), irrelevant);

      // Add ourselves to the range list
      activeRanges.emplace(rg.end, &rg);

      regset curUsedRegisters(usedRegisters.set());
      std::cerr << "On lifetime " << rg.start << "-" << rg.end << " (" << rg.var->name() << "): ";
      std::copy(curUsedRegisters.begin(), curUsedRegisters.end(), std::ostream_iterator<Register>(std::cerr, " "));
      std::cerr << " (" << curUsedRegisters.size() << " registers)" << std::endl;

      // Evaluate any ties
      decltype(activeRanges)::iterator tied(activeRanges.end());
      if ( rg.tied ) {
        auto tiedreg(m_sched.registers().canonicalize(rg.tied));
        std::cerr << "Evaluate tied variable " << rg.var->name() << std::endl;
        // Look for the range in the active or deactivating Ranges
        auto pred([&rg, this, tiedreg](decltype(activeRanges)::iterator::value_type &i) {
          std::cerr << "Check " << i.second->var->name() << " @ " << i.second->start << " to match " << tiedreg->name() << "@" << rg.tied_to << std::endl;
          return i.second->start <= rg.tied_to && m_sched.registers().canonicalize(i.second->var) == tiedreg;
        });

        tied = std::find_if(activeRanges.begin(), activeRanges.end(), pred);
        if ( tied == activeRanges.end() ) {
          std::cerr << "Could not find tie for " << tiedreg->name() <<  " @ " << rg.tied_to << std::endl;
          goto assignNew;
        }

        std::cerr << "Found tie ending at " << tied->second->end << ", while our range starts at " << rg.start << std::endl;
        std::cerr << "Assigned are: ";
        std::copy(tied->second->assigned.begin(), tied->second->assigned.end(), std::ostream_iterator<VirtualRegister>(std::cerr, " "));
        std::cerr << std::endl;
        // otherwise, evaluate the tie. If the tied range ends now, then just assign ourselves the range
        if ( tied->second->end == rg.start ) {
          // Only assign variables that are not already being used
          std::copy(tied->second->assigned.begin(), tied->second->assigned.end(),
                    std::inserter(rg.assigned, rg.assigned.end()));
          m_sched.registers().assign(rg.start, rg.var, tied->second->assigned.begin(), tied->second->assigned.end());
        }
      }

    assignNew:
      if ( rg.assigned.empty() ) {
        regset possible(rg.regs & ~curUsedRegisters);
        std::cerr << "possible registers for " << rg.var->name() << ": ";
        std::copy(rg.regs.begin(), rg.regs.end(), std::ostream_iterator<Register>(std::cerr, " "));
        std::cerr << std::endl;
        auto newReg(possible.begin());
        if ( newReg == possible.end() ) {
          std::cerr << "Must spill " << rg.var->name() << std::endl;
        } else {
          VirtualRegister vreg(*newReg);
          rg.assigned.emplace(vreg);
          usedRegisters.ref(*newReg);
          m_sched.registers().assign(rg.start, rg.var, vreg);
        }
      } else {
        for ( const auto &vreg: rg.assigned ) {
          if ( !vreg.isSpill() )
            usedRegisters.ref(vreg.reg());
        }
      }

      if ( rg.tied && tied != activeRanges.end() ) {
        // Make sure that the variable we're tied to has the registers we have
        std::cerr << "Tie received registers ";
        std::copy(rg.assigned.begin(), rg.assigned.end(),
                  std::ostream_iterator<VirtualRegister>(std::cerr, " "));
        std::cerr << std::endl;
        for ( const auto &reg: rg.assigned ) {
          auto it(tied->second->assigned.find(reg));
          if ( it == tied->second->assigned.end() ) {
            m_sched.registers().assign(rg.start, tied->second->var, reg);
            if ( reg.isRegister() )
              usedRegisters.ref(reg.reg());
          }
        }
      }
    }

    std::cerr << "For block: " << block().name().index() << std::endl;
    for ( const auto &rg: m_ranges ) {
      std::cerr << "  " << rg.var->name() << ": " << rg.start << " - " << rg.end;
      if ( !rg.assigned.empty() ) {
        std::cerr << " = ";
        for ( const auto &vreg: rg.assigned )
          std::cerr << vreg << " ";
        std::cerr << std::endl;
      } else {
        std::cerr << "[";
        for ( const auto &reg: rg.regs )
          std::cerr << reg << " ";
        std::cerr << "]" << std::endl;
      }
    }

  }
}
