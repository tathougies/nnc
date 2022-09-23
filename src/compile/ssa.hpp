#ifndef __nnc_compile_ssa_HPP__
#define __nnc_compile_ssa_HPP__

#include "compile/rtl.hpp"
#include "compile/controlflow.hpp"
#include "compile/dataflow.hpp"
#include "compile/rtl_ops_base.hpp"

namespace nnc {
  namespace exception {
    class RtlVariableNotInitialized : public std::exception {
    public:
      RtlVariableNotInitialized(std::shared_ptr<compile::RtlVariable> var);
      virtual ~RtlVariableNotInitialized();

      virtual const char *what() const noexcept;

      inline std::shared_ptr<compile::RtlVariable> variable() const { return m_uninitialized; }

    private:
      std::shared_ptr<compile::RtlVariable> m_uninitialized;
    };
  }

  namespace compile {
    struct SsaAlias {
      SsaAlias(RtlVariablePtr i, RtlVariablePtr e);
      SsaAlias(RtlVariablePtr i);
      SsaAlias();
      SsaAlias(const SsaAlias &a) =default;

      RtlVariablePtr initial, end;
    };

    class SsaMapper {
    public:
      SsaMapper();

      SsaAlias varNameInBlock(const RtlBlockName &blk,
                              std::shared_ptr<RtlVariable> var);
      void addBlockAlias(const RtlBlockName &blk, std::shared_ptr<RtlVariable> subject,
                         std::shared_ptr<RtlVariable> aka);
      void replaceBlockAlias(const RtlBlockName &blk, std::shared_ptr<RtlVariable> subject,
                             std::shared_ptr<RtlVariable> aka);

      RtlVariablePtr nextName(const RtlBlockName &blk,
                              RtlVariablePtr var);
      RtlVariablePtr nameAtBlockEnd(const RtlBlockName &blk, RtlVariablePtr var);

    private:
      std::map< std::pair<RtlBlockName, std::shared_ptr<RtlVariable> >, SsaAlias > m_aliases;
      std::map< std::pair<RtlBlockName, RtlVariablePtr>, RtlVariablePtr > m_transforms;
    };

    class SsaRewriter {
    public:
      SsaRewriter(RtlBasicBlock &block,
                  SsaMapper &mapper,
                  ControlFlowAnalysis &controlFlow,
                  DataflowAnalysis &dataFlow);
      virtual ~SsaRewriter();

      void operator()();

      inline RtlBasicBlock &block() const { return m_block; }
      inline ControlFlowAnalysis &controlFlow() const { return m_controlFlow; }
      inline DataflowAnalysis &dataFlow() const { return m_dataFlow; }

      inline SsaMapper &aliases() const { return m_aliases; }

    private:
      RtlBasicBlock &m_block;
      ControlFlowAnalysis &m_controlFlow;
      DataflowAnalysis &m_dataFlow;
      SsaMapper &m_aliases;

      friend class SsaOpRewriter;
    };
  }
}

#endif
