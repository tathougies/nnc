#include "compile/dataflow.hpp"

namespace nnc {
  namespace exception {
    RtlVariableWrittenTwice::RtlVariableWrittenTwice(std::shared_ptr<compile::RtlVariable> var,
                                                     compile::RtlBlockName firstWrite,
                                                     compile::RtlBlockName nextWrite)
      : m_var(var), m_firstWrite(firstWrite), m_nextWrite(nextWrite) {
    }

    RtlVariableWrittenTwice::~RtlVariableWrittenTwice() {
    }

    const char *RtlVariableWrittenTwice::what() const noexcept {
      return "rtl variable written twice";
    }
  }

  namespace compile {
    class WriteCollector : public virtual RtlOperandVisitor {
    public:
      WriteCollector(DataflowAnalysis &analysis, RtlBlockName origBlock)
        : RtlOperandVisitor(analysis.function()), m_analysis(analysis), m_block(origBlock){
      }

      virtual ~WriteCollector() {
      }

      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
      }

      virtual void operand(const std::string &name, const RtlBlockName &dest) {
      }

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
        if ( output ) {
          m_analysis.addOriginatingBlock(var->repr(), m_block);
        }
      }

    private:
      DataflowAnalysis &m_analysis;
      RtlBlockName m_block;
    };

    DataflowAnalysis::DataflowAnalysis(RtlFunction &fn)
      : m_function(fn) {
      reanalyze();
    }

    void DataflowAnalysis::reanalyze() {
      m_producers.clear();

      for ( const auto &block : m_function.blocks() ) {
        for ( const auto &op: *block.second ) {
          WriteCollector writes(*this, block.first);
          op->operands(writes);
        }

        for ( const auto &input: block.second->inputs() ) {
          m_producers.insert(std::make_pair(input->repr(), block.first));
        }
      }
    }

    std::optional<RtlBlockName> DataflowAnalysis::findOriginatingBlock(std::shared_ptr<RtlVariable> var) const {
      auto it(m_producers.find(var->repr()));
      if ( it != m_producers.end() ) return std::make_optional(it->second);
      else return std::nullopt;
    }

    void DataflowAnalysis::addOriginatingBlock(std::shared_ptr<RtlVariable> var,
                                               const RtlBlockName &block) {
      auto it(m_producers.find(var));
      if ( it != m_producers.end() ) return;
      //        throw exception::RtlVariableWrittenTwice(var, it->second, block);

      m_producers.insert(std::make_pair(var, block));
    }
  }
}
