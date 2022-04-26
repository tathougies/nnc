#ifndef __nnc_compile_genericschedule_HPP__
#define __nnc_compile_genericschedule_HPP__

#include <set>
#include <deque>
#include <memory>
#include <list>

#include "compile/rtl.hpp"
#include "compile/insnsel.hpp"

namespace nnc {
  namespace compile {
class GenericScheduler : public virtual InsnSelector , public virtual RtlOpMarker {
    public:
      GenericScheduler(RtlFunction &dest, std::shared_ptr<RtlBasicBlock> block);
      virtual ~GenericScheduler();

      /* Start a round of scheduling, and generate all potential instructions */
      void generatePossibilities();

      inline bool morePossibilities() const { return !m_patterns.empty(); }

      /* Choose a particular instruction to schedule. This also reduces the
       * amount of operations left in the block */
      void scheduleInsn(SelectedInsn &insn);

      /* Determine whether we have schedulable instructions */
      inline bool hasSchedulable() const { return !m_selected.empty(); }

      void clearSchedulable();

      inline std::shared_ptr<RtlBasicBlock> sourceBlock() const { return m_srcBlock; }
      inline std::shared_ptr<RtlBasicBlock> destBlock() const { return m_destBlock; }

      void restart();

      virtual RtlFunction &function() const;
      virtual void addRule(std::unique_ptr<InsnSelRule> &&rule);
      virtual void markInsnSchedulable(std::unique_ptr<SelectedInsn> &&rule);
      virtual void matchOp(RtlOp *op);

      virtual void scheduleInsns();

    protected:
      virtual void buildPatterns(InsnSelector &sel) =0;

    private:
      /* Instructions that can be scheduled right now.
       *
       * We sort through these instructions in order to be able to choose the next instruction to schedule.
       */
      std::list< std::unique_ptr<SelectedInsn> > m_selected;

      /* Patterns that are still being matched */
      std::deque< std::unique_ptr<InsnSelRule> > m_patterns;

      /* All non-jump operations in this block */
      std::set< RtlOp *> m_ops;

      std::shared_ptr<RtlBasicBlock> m_srcBlock, m_destBlock;
    };

    class GenericFunctionScheduler {
    public:
      GenericFunctionScheduler(RtlFunction &src);
      virtual ~GenericFunctionScheduler();

      void scheduleEntryPoint(std::shared_ptr<RtlBasicBlock> block);

      inline RtlFunction &function() const { return m_function; }

    protected:
      virtual std::unique_ptr<GenericScheduler> makeBlockScheduler(std::shared_ptr<RtlBasicBlock> block) =0;

    private:
      RtlFunction &m_function;
      std::map< std::shared_ptr<RtlBasicBlock>, std::unique_ptr<GenericScheduler> > m_blocks;
    };
  }
}
#endif
