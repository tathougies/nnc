#include "compile/controlflow.hpp"
#include "compile/rtl_ops.hpp"

namespace nnc {
  namespace compile {
    RtlBlockEdge::RtlBlockEdge()
      : m_cond(nullptr) {
    }

    RtlBlockEdge::RtlBlockEdge(std::shared_ptr<RtlVariable> condition)
      : m_cond(condition) {
    }

    ControlFlowAnalysis::ControlFlowAnalysis(RtlFunction &fn)
      : m_function(fn) {
      reanalyze();
    }

    void ControlFlowAnalysis::findPredecessors(const RtlBlockName &nm,
                                               std::vector<RtlBlockName> &pres) {
      auto first(m_jumpTargets.lower_bound(nm));
      auto last(m_jumpTargets.upper_bound(nm));
      std::transform(first, last, std::back_inserter(pres),
                     [] ( const std::pair<RtlBlockName, RtlBlockName> &edge ) { return edge.second; });
    }

    void ControlFlowAnalysis::reanalyze() {
      m_jumpSources.clear();
      m_jumpTargets.clear();
      m_jumps.clear();

      for ( const auto &block: m_function.blocks() ) {
        // Go throw all branch instructions
        for ( const auto &jump: block.second->jumps() ) {
          auto source(std::make_pair(block.first, jump.second.to()));

          m_jumpSources.insert(source);
          m_jumpTargets.insert(std::make_pair(jump.second.to(), block.first));
          m_jumps.insert(std::make_pair(source, RtlBlockEdge(jump.first)));
        }
      }
    }
  }
}
