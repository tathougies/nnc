#ifndef __nnc_compile_controlflow_HPP__
#define __nnc_compile_controlflow_HPP__

#include "compile/rtl.hpp"

namespace nnc {
  namespace compile {
    class RtlBlockEdge {
    public:
      RtlBlockEdge(std::shared_ptr<RtlVariable> condition);
      RtlBlockEdge();

      inline std::shared_ptr<RtlVariable> condition() const { return m_cond; }

    private:
      std::shared_ptr<RtlVariable> m_cond;
    };

    class ControlFlowAnalysis {
    public:
      ControlFlowAnalysis(RtlFunction &fn);

      void reanalyze();

      inline RtlFunction &function() const { return m_function; }

      void findPredecessors(const RtlBlockName &nm,
                            std::vector<RtlBlockName> &pres);

    private:
      RtlFunction &m_function;

      std::multimap<RtlBlockName, RtlBlockName> m_jumpSources, m_jumpTargets;
      std::map< std::pair<RtlBlockName, RtlBlockName>, RtlBlockEdge > m_jumps;
    };
  }
}

#endif
