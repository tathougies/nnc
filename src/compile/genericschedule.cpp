#include "compile/genericschedule.hpp"

#include <deque>
#include <iostream>

namespace nnc {
  namespace exception {
    class OperationNotFound : public std::exception {
    public:
      OperationNotFound(compile::RtlOp *op) : m_op(op) {
      }
      virtual ~OperationNotFound() {
      }

      virtual const char *what() const noexcept { return "operation not found"; }

      inline compile::RtlOp *op() const { return m_op; }

    private:
      compile::RtlOp *m_op;
    };
  }

  namespace compile {
    GenericScheduler::GenericScheduler(RtlFunction &dest, std::shared_ptr<RtlBasicBlock> block)
      : m_srcBlock(block), m_destBlock(dest.block()) {
      for ( const auto &op : *block )
        m_ops.insert(op.get());
    }

    GenericScheduler::~GenericScheduler() {
    }

    void GenericScheduler::restart() {
      m_selected.clear();
      m_patterns.clear();

      buildPatterns(*this);
    }

    void GenericScheduler::clearSchedulable() {
      m_selected.clear();
    }

    void GenericScheduler::generatePossibilities() {
      std::deque< std::unique_ptr<InsnSelRule> > thesePatterns;
      std::swap(m_patterns, thesePatterns);

//      std::cerr << "Have ops:" << std::endl;
//      std::copy(m_ops.begin(), m_ops.end(), std::ostream_iterator<RtlOp*>(std::cerr, "\n"));
//      std::cerr << "Running rules" << std::endl;

      // Match rules until all rules are exhausted
      while ( !thesePatterns.empty() ) {
        auto rule(std::move(thesePatterns.front()));
        thesePatterns.pop_front();

        for ( RtlOp *op : m_ops ) {
          rule->apply(*this, *op);
        }
      }

      // Now, we should have schedulable instructions
    }

    void GenericScheduler::scheduleInsns() {
      while ( !m_ops.empty() ) {
        restart();

        while ( !m_patterns.empty() )
          generatePossibilities();

        if ( m_selected.empty() ) {
          // TODO exception
          std::cerr << "Could not schedule block" << std::endl;
          return;
        };

      //        std::cerr << "After instruction selection we have " << m_selected.size() << " possibilities" << std::endl;
        auto nextInsn(std::move(m_selected.front()));

        nextInsn->consumedOperations(*this);
        nextInsn->build(*m_destBlock);
      }
    }

    RtlFunction &GenericScheduler::function() const {
      return m_srcBlock->function();
    }

    void GenericScheduler::addRule(std::unique_ptr<InsnSelRule> &&rule) {
      m_patterns.emplace_back(std::move(rule));
    }

    void GenericScheduler::markInsnSchedulable(std::unique_ptr<SelectedInsn> &&insn) {
      m_selected.emplace_back(std::move(insn));
    }

    void GenericScheduler::matchOp(RtlOp *op) {
      //      std::cerr << "Matching op " << op << std::endl;
      auto it(m_ops.find(op));
      if ( it == m_ops.end() ) {
        throw exception::OperationNotFound(op);
      }

      m_ops.erase(it);
    }

    /* Function schedule */
    GenericFunctionScheduler::GenericFunctionScheduler(RtlFunction &src)
      : m_function(src) {
    }

    GenericFunctionScheduler::~GenericFunctionScheduler() {
    }

    void GenericFunctionScheduler::scheduleEntryPoint(std::shared_ptr<RtlBasicBlock> block) {
      auto existing(m_blocks.find(block));
      if ( existing != m_blocks.end() ) return;

      auto it(m_blocks.insert(std::make_pair(block, makeBlockScheduler(block))));
      it.first->second->scheduleInsns();

      for ( const auto &succ : block->jumps() ) {
        scheduleEntryPoint(function().block(succ.second.to()));
      }
    }
  }
}
