#include "compile/unused.hpp"
#include "compile/rtl_ops_base.hpp"

#include <set>

namespace nnc::compile {
  class InputCollector : public RtlOperandVisitor {
  public:
    InputCollector(RtlFunction &fn, std::shared_ptr<RtlBasicBlock> blk)
      : RtlOperandVisitor(fn) {
      for ( const auto &jump : blk->jumps() ) {
        for ( const auto &var: jump.second.arguments() ) {
          m_usedInputs.insert(var);
          if ( jump.first )
            m_usedInputs.insert(jump.first);
        }
      }
    }
    virtual ~InputCollector() {}

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
      if ( input ) m_usedInputs.insert(var);
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &nm) {
    }

    bool isUsedAsInput(const std::shared_ptr<RtlVariable> &v) const {
      return m_usedInputs.find(v) != m_usedInputs.end();
    }

  private:
    std::set<std::shared_ptr<RtlVariable>> m_usedInputs;
  };

  class OpInputUsedChecker : public RtlOperandVisitor {
  public:
    OpInputUsedChecker(InputCollector &c)
      : RtlOperandVisitor(c.function()), m_inputs(c), m_outputsUsed(false) {
    }
    virtual ~OpInputUsedChecker() { }

    inline bool outputsUsed() const { return m_outputsUsed; }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
      if ( m_outputsUsed ) return;

      if ( output && m_inputs.isUsedAsInput(var) )
        m_outputsUsed = true;
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &nm) {
    }
  private:
    InputCollector &m_inputs;
    bool m_outputsUsed;
  };

  UnusedVarCuller::UnusedVarCuller(RtlFunction &fn)
    : m_function(fn) {
  }

  void UnusedVarCuller::cullBlock(std::shared_ptr<RtlBasicBlock> block) {
    int removedOps;

    do {
      removedOps = 0;

      // Find all variables used as inputs
      InputCollector i(function(), block);
      for ( const auto &opPtr: *block) {
        opPtr->operands(i);
      }

      std::list<std::unique_ptr<RtlOp>> remainingOps;
      for ( auto &opPtr: *block) {
        if ( opPtr->isPure() ) {
          // Check if the operation writes to any used variables
          OpInputUsedChecker c(i);
          opPtr->operands(c);

          if ( c.outputsUsed() ) {
            remainingOps.emplace_back(std::move(opPtr));
          } else
            removedOps ++;
        } else
            remainingOps.emplace_back(std::move(opPtr));
      }

      block->replaceOps(remainingOps.begin(), remainingOps.end());
    } while (removedOps);
  }

  void UnusedVarCuller::operator() () {
    for ( const auto &block: function().blocks() ) {
      cullBlock(block.second);
    }
  }
}
