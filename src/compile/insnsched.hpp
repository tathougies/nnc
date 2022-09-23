#ifndef __nnc_compile_insnsched_HPP__
#define __nnc_compile_insnsched_HPP__

#include "compile/insnsel.hpp"
#include "compile/registers.hpp"

namespace nnc::compile {
  class RuleApplier;
  class RuleReadinessCheck;
  class SuspensionRemover;

  class InsnScheduler {
  public:
    InsnScheduler(RtlFunction &src, RtlFunction &dest);
    virtual ~InsnScheduler();

    inline RtlFunction &destination() const { return m_destination; }
    inline RtlFunction &source() const { return m_source; }

    class BlockScheduler;

    class SchedulableInsn {
    public:
      SchedulableInsn(BlockScheduler &s, std::unique_ptr<SelectedInsn> &&rule);

      template<typename Deps>
      void wait(const Deps &deps) { std::copy(std::begin(deps), std::end(deps), std::inserter(m_unresolvedDeps, m_unresolvedDeps.begin())); }
      void resolveOneDep(const RtlVariablePtr &p);
      inline bool ready() const { return m_unresolvedDeps.empty(); }

      inline SelectedInsn &rule() const { return *m_rule; }
      inline const std::set<RtlOp *> &coverage() const { return m_coverage; }

      void eliminateRedundant() const;
      bool redundant() const;

      inline void build(RtlOpSelector &s) const {
        m_rule->build(s);
      }

      inline void consumedOperations(RtlOpMarker &m) const {
        m_rule->consumedOperations(m);
      }

    private:
      BlockScheduler &m_sched;

      std::unique_ptr<SelectedInsn> m_rule;
      std::set<RtlVariablePtr> m_unresolvedDeps;

      std::set<RtlVariablePtr> m_productions;
      std::set<RtlOp *> m_coverage;
    };

    class BlockScheduler : public InsnSelector {
    private:
      std::list<SchedulableInsn> m_selected;

      RegisterTracker m_registers;

    public:
      typedef decltype(m_selected)::iterator MatchPtr;

      BlockScheduler(InsnScheduler &s, RtlBasicBlock &src, RtlBasicBlock &dest);
      virtual ~BlockScheduler();

      virtual RtlFunction &function() const;
      inline InsnScheduler &functionScheduler() const { return m_function; }
      virtual void addRule(std::unique_ptr<InsnSelRule> &&rule);
      virtual void markInsnSchedulable(std::unique_ptr<SelectedInsn> &&rule);

      inline RtlBasicBlock &source() const { return m_src; }
      inline RtlBasicBlock &destination() const { return m_dest; }

      inline std::size_t readyCount() const { return m_ready.size(); }
      inline const std::deque<MatchPtr> &ready() const { return m_ready; }

      inline std::size_t possibilityCount() const { return m_selected.size(); }

      inline RegisterTracker &registers() { return m_registers; }
      inline const RegisterTracker &registers() const { return m_registers; }

      void alias(const RtlVariablePtr &var, const std::shared_ptr<RtlType> &ty,
               const RtlVariablePtr &res);
      RtlVariablePtr getAlias(const RtlVariablePtr &var, const std::shared_ptr<RtlType> &ty) const;

      void ignore(MatchPtr i);
      void select(MatchPtr i);
      void finish();

      /**
       * Collect the set of rules that interfere with us
       */
      std::vector<MatchPtr> interferes(const MatchPtr &p);

      bool isCovered(RtlOp &op) const;
      bool isProduced(const RtlVariablePtr &v) const;
      bool isNeeded(const RtlVariablePtr &v) const;
      bool isComplete() const;
      bool isOpNecessary(const RtlOp &v) const;

      void produceDot(std::ostream &o);

      inline const std::map<RtlVariablePtr, RtlVariablePtr> &varMappings() const { return m_produced; }

    private:
      friend class RuleApplier;
      friend class RuleReadinessCheck;
      friend class SchedulableInsn;
      friend class SuspensionRemover;

      void removeCoverage(MatchPtr i);
      void removeSuspended(MatchPtr i);
      void removeReady(MatchPtr i);
      void removeOp(RtlOp &op);
      void addOp(RtlOp &op);

      RtlVariablePtr mapVar(const RtlVariablePtr &v) const;

      std::deque<MatchPtr> m_ready;
      std::multimap<RtlVariablePtr, MatchPtr> m_suspended;
      // How many unconsumed operations demand this variable. An operation is unconsumed if it
      // appears in m_ops_by_mnemonic.
      std::map<RtlVariablePtr, int> m_demanded;
      std::map<RtlVariablePtr, RtlVariablePtr> m_produced;

      std::set<std::pair<std::string, RtlOp *>> m_ops_by_mnemonic;
      std::multimap<RtlOp *, MatchPtr> m_coverage;

      std::map<std::pair<RtlVariablePtr, std::shared_ptr<RtlType>>, RtlVariablePtr> m_aliases;

      InsnScheduler &m_function;
      RtlBasicBlock &m_src, &m_dest;
    };

    BlockScheduler &block(const RtlBlockName &srcBlock);

  private:
    RtlFunction &m_source, &m_destination;
    std::map<RtlVariablePtr, RtlVariablePtr> m_varMap;
    std::map<RtlBlockName, BlockScheduler> m_blockMapping;
  };
}

#endif
