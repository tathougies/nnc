#include "compile/genericschedule.hpp"
#include "compile/linear_alloc.hpp"
#include "compile/regalloc.hpp"
#include "compile/registers.hpp"

#include <cmath>
#include <set>
#include <cassert>
#include <functional>
#include <iostream>
#include <ranges>

namespace nnc::compile {
bool live_range_order(const LinearAllocator::LiveRange &a,
                      const LinearAllocator::LiveRange &b) {
  if ( a.start == b.start ) {
    if ( a.tied && b.tied ||
         (!a.tied && !b.tied) ) {
      if ( a.assigned.empty() && b.assigned.empty() ) {
        if ( a.end == b.end ) {
          return a.var < b.var;
        } else return a.end < b.end;
      } else if ( a.assigned.empty() ) {
        return false;
      } else if (b.assigned.empty()) {
        return true;
      } else
        return a.var < b.var;
    } else if ( a.tied ) return false;
    else return true;
  } else
    return a.start < b.start;
}

void delete_active_range(LinearAllocator::ActiveRanges &ranges, LinearAllocator::LiveRange *rg) {
  auto [begin, end] = ranges.equal_range(rg->end); // ranges is ordered by end time
  auto it(std::find_if(begin, end, [rg](const auto &x) {
    return rg == x.second;
  }));
  assert(it != end);
  ranges.erase(it);
}

class OpFinder : public RtlOperandVisitor {
public:
  OpFinder(const RtlBasicBlock &blk, const RtlVariablePtr &var)
    : RtlOperandVisitor(blk.function()), m_found(false), m_var(var) {}
  virtual ~OpFinder() {}

  inline bool found() const { return m_found; }

  virtual void operand(const std::string &name, RtlVariablePtr var, bool input,
                       bool output) override {
    if ( input && var->isSame(m_var) ) m_found = true;
  }

  virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty,
                       const void *l, std::size_t lsz) override {}

  virtual void operand(const std::string &name,
                       const RtlBlockName &dest) override {}

private:
  bool m_found;
  RtlVariablePtr m_var;
};

class UseCollector : public RtlOperandVisitor {
public:
  UseCollector(RtlFunction &fn) : RtlOperandVisitor(fn) {}
  virtual ~UseCollector() {}

  bool isUsed(const RtlVariablePtr &var) const {
    return m_used.find(var->repr()) != m_used.end();
  }

  virtual void operand(const std::string &name, RtlVariablePtr var, bool input,
                       bool output) override {
    m_used.insert(var->repr());
  }

  virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty,
                       const void *l, std::size_t lsz) override {}

  virtual void operand(const std::string &name,
                       const RtlBlockName &dest) override {}

private:
  std::set<RtlVariablePtr> m_used;
  };

class ActiveRegisterSet {
public:
  ActiveRegisterSet(RegisterFile &f) : m_file(f) {}

  void ref(const Register &r) {
    auto it(m_data.find(r));
    if (it == m_data.end())
      m_data.emplace(r, 1);
    else
      it->second++;
  }

  void deref(const Register &r) {
    auto it(m_data.find(r));
    if (it != m_data.end()) {
      it->second--;
      if (it->second == 0)
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
    for (const auto &it : m_data) {
      if (it.second > 0)
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
      std::cerr << "Operand " << var->name() << " @ " << m_curTime << ": ";
      auto it(m_lifetimes.find(var->repr()));
      if ( it == m_lifetimes.end() ) {
        std::cerr << "Start new lifetime " << var->name() << std::endl;
        startLifetime(var);
      } else {
        std::cerr << "Using existing lifetime (" << it->second.end << " -> " << m_curTime << ")" << std::endl;
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
      auto it(m_lifetimes.find(var->repr()));

      std::cerr << "Handle reg class for " << var->name() << " @ " << m_curTime << std::endl;
      if ( it != m_lifetimes.end() ) {
        RegisterCollector regs(m_regFile);
        regs(cls);
        regset newset(it->second.regs & regs.set());
        it->second.constrained = true;
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

      auto canon(m_tracker.canonicalize(var));
      if ( canon != var ) regclass(name, canon, cls);
    }

    virtual void intersect(const RtlVariablePtr &var1, const RtlVariablePtr &var2, const RegClass &cls) {
      auto lifetime1(m_lifetimes.find(var1->repr()));
      auto lifetime2(m_lifetimes.find(var2->repr()));
      if ( lifetime1 == m_lifetimes.end() ) {
        startLifetime(var1);
        lifetime1 = m_lifetimes.find(var1->repr());
        lifetime1->second.start = m_curTime - 1;
      }

      if ( lifetime2 == m_lifetimes.end() ) {
        startLifetime(var2);
        lifetime2 = m_lifetimes.find(var2->repr());
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
      LinearAllocator::LiveRange rg(m_regFile, m_curTime, var);

      auto existing(m_tracker.mapper(m_curTime).lookupVar(var));
      // These are hard requirements
      std::for_each(std::begin(existing), std::end(existing),
                    std::bind(&LinearAllocator::LiveRange::assignRegister, &rg, _1));

      std::cerr << "At start of lifetime for " << var->name() << " @ " << rg.start << ": ";
      std::copy(std::begin(existing), std::end(existing), std::ostream_iterator<VirtualRegister>(std::cerr, " "));
      m_lifetimes.emplace(var->repr(), std::move(rg));
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

  LinearAllocator::LinearAllocator(const RtlBasicBlock &block, RegisterFile &regs)
    : m_block(block), m_registers(regs), m_nextSpillSlot(0) {
  }

  void LinearAllocator::buildRanges() {
    LifetimeTracker lifetimes(block().function(), tracker(), m_registers, m_ranges);

    for ( const auto &arg: block().inputs() ) {
      lifetimes.operand(arg->name(), arg, true, false);
    }

    int i(0);
    for ( const auto &op: block() ) {
      lifetimes.time(i);
      op->operands(lifetimes);
      try {
        const RtlRegisterOp &regOp(dynamic_cast<const RtlRegisterOp &>(*op));
        regOp.regclasses(lifetimes);
      } catch ( std::bad_cast &e ) {
        try {
          const RtlAliasOp &aliasOp(dynamic_cast<const RtlAliasOp &>(*op));
          tracker().alias(aliasOp.src(), aliasOp.dest());
        } catch ( std::bad_cast &e ) {
        }
      }
      i++;
    }

    lifetimes.time(i);
    for ( const auto &jump: block().jumps() ) {
      if ( jump.first ) {
        lifetimes.operand(jump.first->name(), jump.first, true, false);
      }
      for ( const auto &arg: jump.second.arguments() ) {
        lifetimes.operand(arg->name(), arg, true, false);
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

  void LinearAllocator::sortRanges(std::list<LiveRange *> &ranges) {
    std::vector<LiveRange *> sorted;
    sorted.reserve(m_ranges.size());
    std::transform(m_ranges.begin(), m_ranges.end(), std::back_inserter(sorted),
                   [](LiveRange &r) { return &r; });

    std::sort(sorted.begin(), sorted.end(), [](LiveRange *a, LiveRange *b) {
      return live_range_order(*a, *b);
    });

    std::copy(sorted.begin(), sorted.end(), std::back_inserter(ranges));
  }

  void LinearAllocator::operator() () {
    m_ranges.clear();
    buildRanges();

    std::list<LiveRange *> ranges;
    sortRanges(ranges);

    // Now assign all ranges
    //
    // Go over all ranges, push onto end time map
    int nextCycle(0);
    ActiveRanges activeRanges;
    ActiveRegisterSet usedRegisters(m_registers);

    std::cerr << "Begin register allocation" << std::endl;
    while ( !ranges.empty() ) {
      LiveRange &rg(*ranges.front());
      ranges.pop_front();

      std::cerr << "Allocating " << rg.var->name() << "(" << rg.start << "-" << rg.end << ")" << std::endl;
      // End any ranges that have ended by the time we start
      auto irrelevant(activeRanges.upper_bound(rg.start - 1));
      for ( auto it(activeRanges.begin());
            it != irrelevant;
            ++it ) {
        for ( const auto &vreg: it->second->assigned ) {
          if ( !vreg.isSpill() &&
               tracker().canonicalize(it->second->var) == it->second->var ) {
            usedRegisters.deref(vreg.reg());
          }
          tracker().unassign(it->second->end + 1, it->second->var, vreg);
        }
      }
      nextCycle = rg.start + 1;

      for ( const auto [_, rg]: std::ranges::subrange(activeRanges.begin(), irrelevant) ) {
        std::cerr << "Delete range " << rg->var->name() << "(" << rg->start << "-" << rg->end << ")" << std::endl;
      }
      activeRanges.erase(activeRanges.begin(), irrelevant);

      // Add ourselves to the range list
      activeRanges.emplace(rg.end, &rg);

      // If this is an alias, assign all registers used by that
      auto canonical(tracker().canonicalize(rg.var));
      std::cerr << "Canonicalized " << rg.var->name() << " to " << canonical->name() << std::endl;
      if ( canonical != rg.var ) {
        std::cerr << "Looking for existing lifetime" << std::endl;
        // Copy all used registers, based on current lifetime
        auto aliased_lifetime(std::find_if(std::begin(activeRanges), std::end(activeRanges),
                                           [canonical](const auto &rg) {
                                             std::cerr << "Match lifetime " << rg.second->var->name() << "(" << rg.second->start << "-" << rg.second->end << "): " << canonical->name() << std::endl;
                                             return rg.second->var->isSame(canonical);
                                           }));
        if ( aliased_lifetime != std::end(activeRanges) ) {
          std::cerr << "Found existing lifetime for " << canonical->name() << std::endl;
          std::copy(std::begin(aliased_lifetime->second->assigned),
                    std::end(aliased_lifetime->second->assigned),
                    std::inserter(rg.assigned, rg.assigned.begin()));
        } else
          std::cerr << "No lifetimes found " << activeRanges.size() << std::endl;
      }

      regset curUsedRegisters(usedRegisters.set());
      std::cerr << "On lifetime " << rg.start << "-" << rg.end << " (" << rg.var->name() << "): ";
      std::copy(curUsedRegisters.begin(), curUsedRegisters.end(), std::ostream_iterator<Register>(std::cerr, " "));
      std::cerr << " (" << curUsedRegisters.size() << " registers)" << std::endl;

      // Evaluate any ties
      decltype(activeRanges)::iterator tied(activeRanges.end());
      if ( rg.tied ) {
        auto tiedreg(tracker().canonicalize(rg.tied));
        std::cerr << "Evaluate tied variable " << rg.var->name() << " (canonical " << tiedreg->name() << ")" << std::endl;
        // Look for the range in the active or deactivating Ranges
        auto pred([&rg, this, tiedreg](decltype(activeRanges)::iterator::value_type &i) {
          std::cerr << "Check " << i.second->var->name() << " @ " << i.second->start << " to match " << tiedreg->name() << "@" << rg.tied_to << std::endl;
          return i.second->start <= rg.tied_to && i.second->var == tiedreg;
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
          tracker().assign(rg.start, rg.var, tied->second->assigned.begin(), tied->second->assigned.end());
        }
      }

    assignNew:
      if ( rg.assigned.empty() ) {
        regset possible(rg.regs & ~curUsedRegisters);
        std::cerr << rg.start << ": possible registers for " << rg.var->name() << " (constrained=" << rg.constrained << "): ";
        std::copy(rg.regs.begin(), rg.regs.end(), std::ostream_iterator<Register>(std::cerr, " "));
        std::cerr << std::endl;
        std::cerr << rg.start << ": used registers are ";
        std::copy(curUsedRegisters.begin(), curUsedRegisters.end(), std::ostream_iterator<Register>(std::cerr, " "));
        std::cerr << std::endl;

        auto newReg(possible.begin());
        if ( newReg == possible.end() ) {
          spill(rg, activeRanges, tracker(), ranges, usedRegisters);
        } else {
          VirtualRegister vreg(*newReg);
          if (curUsedRegisters.find(*newReg) != curUsedRegisters.end()) {
            auto comp(~curUsedRegisters);
            std::cerr << "COmplement of used registers is ";
            std::copy(comp.begin(), comp.end(), std::ostream_iterator<Register>(std::cerr, " "));
            std::cerr << std::endl;
            assert(curUsedRegisters.find(*newReg) == curUsedRegisters.end());
          }
          std::cerr << rg.start << ": Assigning " << vreg << " to " << rg.var->name() << std::endl;
          rg.assigned.emplace(vreg);
          usedRegisters.ref(*newReg);
          tracker().assign(rg.start, rg.var, vreg);
        }
      } else {
        std::cerr << "Reuising existing registers: ";
        for ( const auto &vreg: rg.assigned ) {
          std::cerr << vreg << " ";
          if ( !vreg.isSpill() )
            usedRegisters.ref(vreg.reg());
        }
        std::cerr << std::endl;
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
            tracker().assign(rg.start, tied->second->var, reg);
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

  void LinearAllocator::spill(LiveRange &rg, ActiveRanges &activeRanges, RegisterTracker &tracker,
                              std::list<LiveRange *> &liveRanges, ActiveRegisterSet &usedRegisters) {
    auto op_ptr(block().begin());
    std::advance(op_ptr, rg.start);

    UseCollector uses(block().function());
    (*op_ptr)->operands(uses);

    std::map<Register, double> eviction_scores;

    // TODO examine how best to do this
    std::cerr << "Must spill " << rg.var->name() << std::endl << "  Active ranges:" << std::endl;
    for (const auto & [_, activeRange] : activeRanges) {
      if ( rg.var->isSame(activeRange->var) ) continue;
      if ( uses.isUsed(activeRange->var) ) continue; // Used in this instruction
      if (std::any_of(activeRange->assigned.begin(),
                      activeRange->assigned.end(),
                      [&rg](const VirtualRegister &reg) {
                        return reg.isRegister() &&
                               rg.regs.find(reg.reg()) != rg.regs.end();
                      })) {
        std::cerr << "    - " << activeRange->var->name() << " ("
                  << activeRange->start << " - " << activeRange->end << "): ";
        std::copy(activeRange->assigned.begin(), activeRange->assigned.end(),
                  std::ostream_iterator<VirtualRegister>(std::cerr, " "));
        std::cerr << std::endl;

        for (const auto &assigned : activeRange->assigned) {
          if (assigned.isSpill())
            continue;
          auto existing(eviction_scores.find(assigned.reg()));
          if (existing == eviction_scores.end() ||
              existing->second > activeRange->end) {
            double score(-activeRange->end);
            if (activeRange->start < 0)
              score = INFINITY;
            eviction_scores[assigned.reg()] = score;
          }
        }
      }
    }

    auto to_evict(std::min_element(eviction_scores.begin(), eviction_scores.end(),
                                   [](const auto &a, const auto &b) {
                                     return a.second < b.second;
                                   }));
    assert(to_evict != eviction_scores.end());

    std::cerr << "Will evict for allocating " << to_evict->first << " at " << rg.start << std::endl;
    RtlTypeInfo tyInfo(*rg.var->type());
    VirtualRegister spill_slot(m_nextSpillSlot, rg.var->type()->size());
    m_nextSpillSlot += spill_slot.size();

    // All variables assigned to this register will now be moved into the same spill slot.
    std::list<std::pair<LinearAllocator::LiveRange*, int>> to_retime;
    for (auto &[_, activeRange] : activeRanges) {
      auto evicted(activeRange->assigned.find(to_evict->first));
      if ( evicted == activeRange->assigned.end() ) continue;

      std::cerr << "Register " << to_evict->first << " unassigned from " << activeRange->var->name() << std::endl;
      if (activeRange->tied) {
        std::cerr << "  this register is tied to " << activeRange->tied->name()
                  << " at " << activeRange->tied_to << std::endl;
      }

      activeRange->assigned.erase(evicted);
      tracker.unassign(rg.start, activeRange->var, to_evict->first);

      if ( activeRange->end > rg.start )
        tracker.assign(rg.start, activeRange->var, spill_slot);

      usedRegisters.deref(to_evict->first);

      // Find the next use of this register
      if (activeRange->assigned.empty()) {
        int nextUse(reinsertActiveRange(liveRanges, *activeRange, rg.start));
        to_retime.push_back(std::make_pair(activeRange, nextUse));
      }
    }

    for ( auto [activeRange, nextUsedCycle]: to_retime) {
      bool needsPreservation(activeRange->end > rg.start);

      delete_active_range(activeRanges, activeRange);

      activeRange->end = nextUsedCycle + 1;
      activeRange->start = rg.start;
      if ( needsPreservation ) {
        auto [begin, end] = activeRanges.equal_range(activeRange->end);
        auto loc(std::find_if(begin, end, [activeRange](const auto &existingRange) {
          return !live_range_order(*activeRange, *existingRange.second);
        }));
        activeRanges.insert(loc, std::make_pair(activeRange->end, activeRange));
      }
    }

    usedRegisters.ref(to_evict->first);
    rg.assigned.emplace(to_evict->first);
    tracker.assign(rg.start, rg.var, to_evict->first);
    std::cerr << "Spill assign " << to_evict->first << " to " << rg.var->name() << std::endl;
  }

  int LinearAllocator::reinsertActiveRange(std::list<LinearAllocator::LiveRange *> &ranges,
                                           LinearAllocator::LiveRange &rg, int after) {
    auto op(block().begin());
    int cycle(after + 1);
    std::advance(op, cycle);

    std::cerr << "Cut lifetime " << rg.var->name() << "(" << rg.start << " - " << rg.end << ") short at " << after << std::endl;

    OpFinder finder(block(), rg.var);
    for (; op != block().end(); ++op, ++cycle) {
      (*op)->operands(finder);
      if ( finder.found() )
        break;
    }

    std::cerr << "Will now start at " << cycle << std::endl;
    if ( cycle <= rg.end ) {
      std::cerr << "Creating new range" << std::endl;
      LinearAllocator::LiveRange newRange(rg);
      newRange.assigned.clear();
      newRange.tied = nullptr;
      newRange.tied_to = 0;
      newRange.start = cycle - 1;

      std::cerr << "Inserting at " << newRange.start << std::endl;

      auto location(std::find_if(ranges.begin(), ranges.end(), [&newRange](const auto &existingRange) {
        return !live_range_order(*existingRange, newRange);
      }));
      std::cerr << "INserting before " << std::distance(ranges.begin(), location) << std::endl;
      m_ranges.emplace_back(std::move(newRange));
      //      if ( location == ranges.end() ) std::cerr << "insert at end " << std::endl;
      //      else std::cerr << " Insert before " << (*location)->var->name() << std::endl;
      ranges.emplace(location, &m_ranges.back());
    }

    return cycle;
  }
}
