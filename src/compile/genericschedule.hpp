#ifndef __nnc_compile_genericschedule_HPP__
#define __nnc_compile_genericschedule_HPP__

#include <set>
#include <deque>
#include <memory>
#include <list>

#include "compile/rtl.hpp"
#include "compile/insnsel.hpp"
#include "compile/registers.hpp"
#include "compile/cconv.hpp"

namespace nnc::compile {
  class CallingConvention;

  class GenericFunctionScheduler;

  std::deque<RtlBlockName> orderBlocksNoLoops(GenericFunctionScheduler &sched);

  class GenericScheduler : public virtual InsnSelector , public virtual RtlOpMarker {
  public:
    typedef RtlBlockName id_type;

    inline RtlBlockName id() const { return destBlock()->name(); }

    GenericScheduler(GenericFunctionScheduler &dest, std::shared_ptr<RtlBasicBlock> block);
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
    inline const RegisterTracker &registers() const { return m_regs; }
    inline RegisterTracker &registers() { return m_regs; }

    inline void setEntryPoint() { m_isEntryPoint = true; }
    inline bool isEntryPoint() const { return m_isEntryPoint; }

    std::shared_ptr<RtlVariable> translateVariable(const std::shared_ptr<RtlVariable> &v);

    void restart();

    virtual RtlFunction &function() const;
    virtual void addRule(std::unique_ptr<InsnSelRule> &&rule);
    virtual void markInsnSchedulable(std::unique_ptr<SelectedInsn> &&rule);
    virtual void matchOp(RtlOp *op);

    virtual void scheduleInsns();

    inline GenericFunctionScheduler &functionScheduler() const { return m_fn; }

  protected:
    virtual void buildPatterns(InsnSelector &sel) =0;

  private:
    GenericFunctionScheduler &m_fn;
    RegisterTracker m_regs;
    bool m_isEntryPoint;

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

    std::map<std::shared_ptr<RtlVariable>, std::shared_ptr<RtlVariable>> m_inputMap;
  };

  class InsnEncoder;

  class GenericFunctionScheduler {
  public:
    typedef GenericScheduler *node_type;
    typedef RtlBlockName id_type;

    GenericFunctionScheduler(RtlFunction &src);
    virtual ~GenericFunctionScheduler();

    void scheduleEntryPoint(std::shared_ptr<RtlBasicBlock> block, const std::string &name = "entry");
    void scheduleEntryPoint(std::shared_ptr<RtlBasicBlock> block, CallingConvention &cconv, const std::string &name = "entry");

    GenericScheduler &blockScheduler(const RtlBlockName &nm) const;

    inline RtlFunction &function() const { return m_function; }

    void dump(std::ostream &out) const;

    virtual RegisterFile &registers() const =0;

    static inline id_type name(GenericScheduler *c) { return c->id(); }

    template<typename Fn>
    void successors(GenericScheduler *c, Fn f) const {
      for ( const auto &jump: c->destBlock()->jumps() ) {
        f(jump.second.to(), &blockScheduler(jump.second.to()));
      }
    }

    template<typename Fn>
    void predecessors(GenericScheduler *c, Fn f) {
      for ( auto &block: m_blocks ) {
        for ( const auto &jump: block.second->destBlock()->jumps() ) {
          if ( jump.second.to() == c->destBlock()->name() ) {
            f(block.second->destBlock()->name(), block.second.get());
          }
        }
      }
    }

    void encode(std::ostream &out);
    void encode(std::ostream &out, std::map<RtlBlockName, std::ptrdiff_t> &entries);

    inline const std::map<std::string, RtlBlockName> &entryPoints() const { return m_entries; }

  protected:
    virtual std::unique_ptr<GenericScheduler> makeBlockScheduler(std::shared_ptr<RtlBasicBlock> block) =0;
    virtual std::unique_ptr<CallingConvention> defaultCallingConvention() const =0;
    virtual const InsnEncoder &defaultInsnEncoder() const =0;

  private:
    GenericScheduler &scheduleBlockAndDeps(std::shared_ptr<RtlBasicBlock> block,
                                           std::list<GenericScheduler *> &newBlocks);
    std::string allocName(const std::string &base) const;

    RtlFunction &m_function;
    std::map< RtlBlockName, std::unique_ptr<GenericScheduler> > m_blocks;
    std::map< RtlBlockName, decltype(m_blocks)::iterator > m_destBlocks;
    std::map<std::string, RtlBlockName> m_entries;

  public:
    class iterator {
    public:
      typedef GenericScheduler value_type;
      typedef int difference_type;

      inline GenericScheduler &operator*() const { return *m_underlying->second; }
      inline GenericScheduler *operator->() const { return m_underlying->second.get(); }
      inline iterator operator++(int i) { iterator r(*this); ++(*this); return r; }
      inline iterator &operator++() { m_underlying++; return *this; }

      inline bool operator!=(const iterator &o) const { return !(*this == o); }
      inline bool operator==(const iterator &o) const { return m_underlying == o.m_underlying; }

      inline iterator operator+(int i) const {
        iterator r(*this);
        std::advance(r, i);
        return r;
      }
    private:
      typedef decltype(m_blocks)::iterator underlying;
      underlying m_underlying;

      inline iterator(const underlying &u) : m_underlying(u) { };

      friend class GenericFunctionScheduler;
    };

    iterator begin();
    iterator end();
  };
}
#endif
