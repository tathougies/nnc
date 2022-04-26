#include <iostream>
#include <deque>
#include <set>
#include <iterator>
#include <cassert>

#include "compile/ssa.hpp"
#include "compile/rtl_ops.hpp"

namespace nnc {
  namespace exception {
    RtlVariableNotInitialized::RtlVariableNotInitialized(std::shared_ptr<compile::RtlVariable> var)
      : m_uninitialized(var) {
    }

    RtlVariableNotInitialized::~RtlVariableNotInitialized() {
    }

    const char *RtlVariableNotInitialized::what() const noexcept {
      return "variable not initialized";
    }
  }

  namespace compile {
    class SsaThreader {
    public:
      SsaThreader(SsaRewriter &rewriter, const RtlBlockName &origName,
                  std::shared_ptr<RtlVariable> var)
        : m_rewriter(rewriter),
          m_origBlock(origName), m_function(rewriter.block()->function()),
          m_var(var) {
        addVarName(origName, var);
      }

      void operator() (std::shared_ptr<RtlBasicBlock> finalBlock) {
        varNameInBlock(finalBlock);

        while ( !done() ) {
          auto nextCall(m_upcoming.front());
          m_upcoming.pop_front();

          std::shared_ptr<RtlBasicBlock> nextBlock(nextCall.first);
          RtlBlockName nextJump(nextCall.second);

          // For every call into the target block (nextBlock.second),
          // find the variable name in the block and add it as an
          // argument.
          auto var(varNameInBlock(nextBlock));


          for ( auto &jump: nextBlock->jumps() ) {
            if ( jump.second.to() == nextJump )
              jump.second.push_argument(var);
          }
        }
      };

      void addVarName(const RtlBlockName &block, std::shared_ptr<RtlVariable> nm) {
        m_rewriter.aliases().addBlockAlias(block, m_var, nm);
      }

      RtlFunction &function() const { return m_function; }
      ControlFlowAnalysis &controlFlow() const { return m_rewriter.controlFlow(); }

      void addDestination(RtlBasicBlock &blk) {
        std::vector< RtlBlockName > predecessors;
        controlFlow().findPredecessors(blk.name(), predecessors);

        for ( const RtlBlockName &pre : predecessors ) {
          enqueueCall(pre, blk.name());
        }
      }

    private:
      bool done() const {
        return m_upcoming.empty();
      }

      bool visited(std::shared_ptr<RtlBasicBlock> block, const RtlBlockName &dest) const {
        return m_visited.find(std::make_pair(block->name(), dest)) != m_visited.end();
      }

      std::shared_ptr<RtlVariable> varNameInBlock(std::shared_ptr<RtlBasicBlock> blk) {
        std::shared_ptr<RtlVariable> v(m_rewriter.aliases().varNameInBlock(blk->name(), m_var));

        if ( v ) return v;

        auto blkVar(blk->addInput(m_var->namePrefix(), m_var->type()));
        addVarName(blk->name(), blkVar);
        m_rewriter.dataFlow().addOriginatingBlock(blkVar, blk->name());

        addDestination(*blk);

        return blkVar;
      }

      void enqueueCall(const RtlBlockName &pre, const RtlBlockName &tgt) {
        auto preBlock(function().block(pre));
        if ( visited(preBlock, tgt) ) return;

        m_upcoming.push_back(std::make_pair(preBlock, tgt));
      }

      SsaRewriter &m_rewriter;
      RtlBlockName m_origBlock;
      RtlFunction &m_function;
      std::shared_ptr<RtlVariable> m_var;

      std::set< std::pair<RtlBlockName, RtlBlockName> > m_visited;

      std::deque< std::pair< std::shared_ptr<RtlBasicBlock>, RtlBlockName > > m_upcoming;
    };

    class SsaOpRewriter : public RtlOperandVisitor {
    public:
      SsaOpRewriter(RtlFunction &fn, SsaRewriter &rewrite, RtlOp &op)
        : RtlOperandVisitor(fn), m_rewriter(rewrite), m_op(op) {
      }

      virtual ~SsaOpRewriter() {
        for ( const auto &arg : m_newArgs ) {
          m_op.operand(arg.first, arg.second);
        }
      }

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
        auto origBlock(m_rewriter.m_dataFlow.findOriginatingBlock(var));
        if ( ! origBlock )
          throw exception::RtlVariableNotInitialized(var);

        if ( *origBlock == m_rewriter.block()->name() ) return;

        // Check if it's already rewritten
        auto newVar(m_rewriter.aliases().varNameInBlock(m_rewriter.block()->name(), var));
        if ( !newVar ) {
          // Now we have to thread the variable through all edges that reach this block
          SsaThreader threader(m_rewriter, *origBlock, var);
          threader(m_rewriter.block());

          newVar = m_rewriter.aliases().varNameInBlock(m_rewriter.block()->name(), var);
        }
        m_newArgs.insert(std::make_pair(name, newVar));
      }

      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
      }

      virtual void operand(const std::string &name, const RtlBlockName &dest) {
      }
    private:
      SsaRewriter &m_rewriter;
      RtlOp &m_op;
      std::map< std::string, std::shared_ptr<RtlVariable> > m_newArgs;
    };

    SsaRewriter::SsaRewriter(std::shared_ptr<RtlBasicBlock> block, SsaMapper &mapper,
                             ControlFlowAnalysis &controlFlow,
                             DataflowAnalysis &dataFlow)
      : m_block(block), m_controlFlow(controlFlow), m_dataFlow(dataFlow), m_aliases(mapper) {
    }

    SsaRewriter::~SsaRewriter() {
    }

    void SsaRewriter::operator() () {
      for ( const auto &op : *m_block ) {
        SsaOpRewriter rewriter(m_block->function(), *this, *op);
        op->operands(rewriter);
      }
    }

    SsaMapper::SsaMapper() {
    }

    std::shared_ptr<RtlVariable> SsaMapper::varNameInBlock(const RtlBlockName &blk,
                                                           std::shared_ptr<RtlVariable> var) {
      auto it(m_aliases.find(std::make_pair(blk, var->repr())));
      if ( it == m_aliases.end() ) return nullptr;

      return it->second;
    }

    void SsaMapper::addBlockAlias(const RtlBlockName &blk, std::shared_ptr<RtlVariable> subject,
                                  std::shared_ptr<RtlVariable> aka) {
      auto key(std::make_pair(blk, subject->repr()));
      if ( m_aliases.find(key) != m_aliases.end() ) {
        throw std::runtime_error("bad block alias");
      }

      m_aliases.insert(std::make_pair(key, aka));
    }
  }
}
