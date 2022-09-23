#ifndef __nnc_compile_dataflow_HPP__
#define __nnc_compile_dataflow_HPP__

#include "compile/rtl.hpp"

#include <optional>

namespace nnc {
  namespace exception {
    class RtlVariableWrittenTwice : public std::exception {
    public:
      RtlVariableWrittenTwice(std::shared_ptr<compile::RtlVariable> var,
                              compile::RtlBlockName firstWrite,
                              compile::RtlBlockName nextWrite);
      virtual ~RtlVariableWrittenTwice();

      virtual const char *what() const noexcept;

      inline std::shared_ptr<compile::RtlVariable> var() const { return m_var; }
      inline compile::RtlBlockName firstWrite() const { return m_firstWrite; }
      inline compile::RtlBlockName nextWrite() const { return m_nextWrite; }

    private:
      std::shared_ptr<compile::RtlVariable> m_var;
      compile::RtlBlockName m_firstWrite, m_nextWrite;
    };
  }

  namespace compile {
    class DataflowAnalysis {
    public:
      DataflowAnalysis(RtlFunction &fn);

      inline RtlFunction &function() const { return m_function; }

      std::optional<RtlBlockName> findOriginatingBlock(std::shared_ptr<RtlVariable> var) const;
      void addOriginatingBlock(std::shared_ptr<RtlVariable> var,
                               const RtlBlockName &block);

      void reanalyze();

    private:
      RtlFunction &m_function;

      std::map< std::shared_ptr<RtlVariable>, RtlBlockName > m_producers;

      friend class WriteCollector;
    };
  }
}

#endif
