#include "compile/x86_64/manager.hpp"
#include "compile/x86_64/encoding.hpp"
#include "arch/x86_64/schedule.hpp"
#include "arch/x86_64/registers.hpp"
#include "compile/regalloc.hpp"
#include "compile/linear_alloc.hpp"
#include "compile/insnsched.hpp"
#include "compile/dump.hpp"
#include "compile/unused.hpp"
#include "compile/link.hpp"

#include <sstream>
#include <fstream>
#include <vector>

namespace nnc::arch::x86_64::compile {
  using namespace ::nnc::compile;

  class PotentialBuildOutput : public RtlOpSelector {
  public:
    PotentialBuildOutput(RtlFunction &fn, RtlBasicBlockDumper &dump)
      : m_ix(0), m_function(fn), m_dump(dump) {
    }

    virtual ~PotentialBuildOutput() {
    }

    virtual RtlFunction &function() const override {
      return m_function;
    }

    virtual void arbitrary(const RtlVariablePtr &var, const RegClass &rc) override {
      m_dump.out() << "  make arbitrary " << var->name() << std::endl;
    }

    virtual void alias(const RtlVariablePtr &left, const RtlVariablePtr &right) override {
      m_dump.out() << "  alias " << left->name() << " -> " << right->name() << std::endl;
    }

    virtual void op(std::unique_ptr<RtlOp> &&op) override {
      m_dump.dumpOp(m_function, nullptr, *op, m_ix);
      m_dump.out() << std::endl;
      ++m_ix;
    }

    virtual void clear() override { }

  private:
    int m_ix;
    RtlFunction &m_function;
    RtlBasicBlockDumper &m_dump;
  };

  class OpPriorities {
  private:
    class PrioUpdater : public RtlOperandVisitor {
    public:
      PrioUpdater(RtlFunction &fn, std::map<RtlVariablePtr, int> &varPrios)
        : RtlOperandVisitor(fn), m_varPrios(varPrios), m_prio(1) {
      }

      virtual ~PrioUpdater() {
        for ( const auto &v: m_inputs ) {
          auto it(m_varPrios.find(v));
          if ( it == m_varPrios.end() )
            m_varPrios.emplace(v, priority());
          else {
            it->second = std::max(it->second, priority());
          }
        }
      }

      virtual void operand(const std::string &nm, RtlVariablePtr var, bool input = true, bool output = false) override {
        if ( input ) {
          m_inputs.push_back(var);
        }

        if ( output ) {
          auto it(m_varPrios.find(var));
          if ( it != m_varPrios.end() ) {
            m_prio = std::max(m_prio, it->second + 1);
          }
        }
      }

      virtual void operand(const std::string &nm, std::shared_ptr<RtlType> ty, const void *l, std::size_t sz) override {
      }

      virtual void operand(const std::string &name, const RtlBlockName &dest) override {
      }

      inline int priority() const { return m_prio; }

    private:
      std::deque<RtlVariablePtr> m_inputs;
      std::map<RtlVariablePtr, int> &m_varPrios;
      int m_prio;
    };
  public:
    OpPriorities() {
    }

    OpPriorities(const RtlBasicBlock &bl) {
      add(bl);
    }

    void add(const RtlBasicBlock &bl) {
      std::deque<RtlOp*> ops;
      for ( const auto &op: bl )
        ops.push_front(op.get());

      for ( const auto &op: ops ) {
        PrioUpdater p(bl.function(), m_varPrios);
        op->operands(p);
        m_opPrios.emplace(op, p.priority());
      }
    }

    int operator[] (const RtlVariablePtr &p) {
      auto it(m_varPrios.find(p));
      if ( it == m_varPrios.end() ) return -1;
      else return it->second;
    }

    int operator[] (const RtlOp &o) {
      auto it(m_opPrios.find(&o));
      if ( it == m_opPrios.end() ) return -1;
      else return it->second;
    }

  private:
    std::map<RtlVariablePtr, int> m_varPrios;
    std::map<const RtlOp *, int> m_opPrios;
  };

  class InsnSchedDumper : public RtlBasicBlockDumper {
  public:
    InsnSchedDumper(std::ostream &out, OpPriorities &p)
      : RtlBasicBlockDumper(out),
        m_priorities(p) {
    }

    virtual ~InsnSchedDumper() {
    }

    virtual void dumpOp(RtlFunction &fn, const RtlBasicBlock *block, RtlOp &op, int index) override {
      RtlBasicBlockDumper::dumpOp(fn, nullptr, op, index);
      out() << "[prio=" << m_priorities[op] << "]";
    }

  private:
    OpPriorities &m_priorities;
  };

  class OpCoster : public RtlOpSelector {
  public:
    OpCoster(RtlFunction &fn, double defaultCost = 1000.0)
      : m_function(fn), m_cost(0), m_defaultCost(defaultCost) { }
    virtual ~OpCoster() { };

    virtual void alias(const RtlVariablePtr &left, const RtlVariablePtr &right) { }
    virtual void arbitrary(const RtlVariablePtr &v, const RegClass &rc) { }

    virtual void clear() { m_cost = 0; }
    virtual void op(std::unique_ptr<RtlOp> &&op) {
      auto *costed(dynamic_cast<RtlCostedOp *>(op.get()));
      if ( costed )
        m_cost += costed->cost();
      else
        m_cost += m_defaultCost;
    }

    virtual RtlFunction &function() const { return m_function; }

    inline double cost() const { return m_cost; }

  private:
    RtlFunction &m_function;
    double m_cost, m_defaultCost;
  };

  class PrioritySummer : public RtlOpMarker {
  public:
    PrioritySummer(OpPriorities &prio)
      : m_prios(prio), m_total(0) {
    }

    virtual ~PrioritySummer() {
    }

    virtual void matchOp(RtlOp *op, bool consume = true) override {
      if ( consume ) {
        auto n(m_prios[*op]);
        if ( n > 0 )
          m_total += n;
      }
    }

    inline int total() const { return m_total; }

  private:
    OpPriorities &m_prios;
    int m_total;
  };

  DefaultManager::DefaultManager(const std::string &nm)
    : ::nnc::compile::DefaultManager(nm) {
  }

  DefaultManager::~DefaultManager() {
  }

  InsnScheduler::BlockScheduler::MatchPtr explore(InsnScheduler::BlockScheduler &s, OpPriorities &priorities, bool &automatic) {
    std::vector<std::pair<double, InsnScheduler::BlockScheduler::MatchPtr>> ready;
    std::transform(std::begin(s.ready()), std::end(s.ready()), std::back_inserter(ready),
                   [&priorities, &s](const auto &a) {
                     PrioritySummer prio(priorities);
                     a->consumedOperations(prio);

                     std::vector<InsnScheduler::BlockScheduler::MatchPtr> interference(s.interferes(a));
                     // Now sum all priorities
                     double maxInterferedPrio(0);
                     for ( const auto &i: interference ) {
                       if ( i->ready() ) continue;

                       PrioritySummer iprio(priorities);
                       i->consumedOperations(iprio);
                       if ( iprio.total() > maxInterferedPrio )
                         maxInterferedPrio = iprio.total();
                     }

                     OpCoster coster(s.destination().function());
                     a->build(coster);

                     return std::make_pair((prio.total() - maxInterferedPrio)/coster.cost(), a);
                   });

    std::sort(std::begin(ready), std::end(ready),
              [](const auto &a, const auto &b) { return a.first > b.first; });

    auto i(ready.begin());
    int ix(0);

    std::cerr << "There are " << s.possibilityCount() << " rule(s) that matched" << std::endl;

    if ( automatic ) return i->second;

    std::stringstream nm;
    nm << "block" << s.source().name().index() << ".dot";
    std::fstream f(nm.str().c_str(), std::fstream::out);
    s.produceDot(f);

    InsnSchedDumper srcDump(std::cerr, priorities);
    RtlBasicBlockDumper dstDump(std::cerr);
    for ( ;; ) {
      std::cerr << "Option " << ix << "(" << &i->second->rule() << ", prio=" << i->first << "):" << std::endl;

      int opix(0);
      for ( const auto &op: i->second->coverage() ) {
        srcDump.dumpOp(s.source().function(), &s.source(), *op, opix);
        srcDump.out() << std::endl;
        opix++;
      }

      auto next(i);
      ++next;

      bool hasNext(next != ready.end()), hasPrev(i != ready.begin());
      std::cerr << "Press ENTER to select";
      if ( hasNext )
        std::cerr << ", n to go forward";
      if ( hasPrev )
        std::cerr << ", p to go back";
      std::cerr << ", or ? to see what would be produced: ";
      std::cerr.flush();

      std::string resp;
      std::getline(std::cin, resp);

      if ( resp.empty() ) {
        return i->second;
      } else if ( resp == "n" && hasNext ) {
        ++i;
        ++ix;
        continue;
      } else if ( resp == "p" && hasPrev ) {
        --i;
        --ix;
        continue;
      } else if ( resp == "v" ) {
        std::stringstream cmd;
        cmd << "dot -Tpdf -o block" << s.source().name().index() << ".pdf block" << s.source().name().index() << ".dot";
        system(cmd.str().c_str());

        cmd.str("");
        cmd << "okular block" << s.source().name().index() << ".pdf";
        system(cmd.str().c_str());
      } else if ( resp == "?" ) {
        PotentialBuildOutput o(s.source().function(), dstDump);
        i->second->build(o);
        continue;
      } else if ( resp == "!" ) {
        automatic = true;
        return i->second;
      }
    }
  }

  void DefaultManager::schedule(RtlFunction &fn,
                                ::nnc::invoke::FunctionLibrary &lib) {
    RtlFunction compiled(fn.functionName());
    InsnScheduler s(fn, compiled);
    bool automatic(true); // Note: enable if you want it to be a manual thing

    OpPriorities prio;
    for ( const auto &block: fn.blocks() )
      prio.add(block);

    InsnSchedDumper srcDump(std::cerr, prio);
    for ( auto &block: fn.blocks() )
      srcDump.dump(block);

    for ( const auto &block: fn.bfs() ) {
      InsnScheduler::BlockScheduler &blockScheduler(s.block(block.name()));
      schedule::buildInsnRules(blockScheduler);

      while ( blockScheduler.readyCount() > 0 ) {
        std::cerr << "For block " << block.name().index() << ", " << blockScheduler.readyCount() << " could be scheduled" << std::endl;

        auto selected(explore(blockScheduler, prio, automatic));

        std::cerr << "Selecting operations: " << std::endl;

        blockScheduler.select(selected);
      }

      // Check that every op is covered
      if ( blockScheduler.isComplete() ) {
        blockScheduler.finish();
      } else {
        std::cerr << "Could not complete block" << std::endl;
        InsnSchedDumper srcDump(std::cerr, prio);
        for (const auto &op : blockScheduler.source()) {
          if (!blockScheduler.isCovered(*op))
            srcDump.dumpOp(blockScheduler.function(), &blockScheduler.source(),
                           *op, 0);
        }
        throw std::runtime_error("Could not complete block");
      }
//
//      // Score each operation by what it is
//      OpPriorities scorer(blockScheduler.source());
//
//      std::cerr <<
//
//      blockScheduler.prioritySchedule(scorer);
    }

    // TODO get rid of this
    UnusedVarCuller cull(s.destination());
    cull();

    RtlBasicBlockDumper dump(std::cerr);
    dump.dump(s.destination());

    // Allocate registers
    RegisterAllocator<LinearAllocator> regalloc(s.destination(), registers);
    cconv::ccall cconv;
    cconv(s.destination().entry());
    regalloc.registersFor(s.destination().entry().name()).assign(-1, cconv);
    regalloc();

    // Now dump again
    RtlRegisterBlockDumper regdump(regalloc, std::cerr);
    regdump.dump(s.destination());

    jumps j;
    Linker linker(j);
    for ( const auto &block: s.destination().dfs() ) {
      linker.placeBlock(block, regalloc);
    }
    linker.finish();

    std::cerr << "Dumping compiled.out" << std::endl;
    std::fstream f("compiled.out", std::fstream::out | std::fstream::binary);
    linker.dump(f);
    f.flush();
    f.close();
    std::cerr << "Finish dumping " << std::endl;

    lib.link(linker, s.destination().entry(), m_prefix);

//
//    schedule::scheduler s(fn);
//    s.scheduleEntryPoint(fn.block(entry), fn.functionName());
//
//    RegisterAllocator<LinearAllocator> regalloc(s);
//    regalloc();
//
//    lib.link(s, m_prefix);
  }
}
