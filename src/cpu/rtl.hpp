#ifndef __nnc_cpu_rtl_HPP__
#define __nnc_cpu_rtl_HPP__

#include "cpu/cpu_op.hpp"
#include "compile/rtl.hpp"

#include <deque>

namespace nnc {
  namespace executor {
    class CpuMemoryTensor {
    public:
      virtual void
      load(const std::string &basename, compile::RtlFunction &fn,
           CpuRtlInputTranslator &h, std::shared_ptr<compile::RtlVariable> var) = 0;

      virtual void store(compile::RtlFunction &fn, CpuRtlInputTranslator &h,
                         std::shared_ptr<compile::RtlVariable> value) =0;
    };

    class CpuScalarTensor {
    public:
      virtual std::shared_ptr<compile::RtlVariable> load(const std::string &basename, compile:: RtlFunction &fn,
                                                         CpuRtlInputTranslator &h) =0;
    };

    class CpuRtlLoop {
    public:
      CpuRtlLoop(compile::RtlBasicBlock &head,
                 std::shared_ptr<compile::RtlVariable> ix);

      inline compile::RtlBasicBlock &head() const { return m_head; }
      inline std::shared_ptr<compile::RtlVariable> index() const { return m_index; }

    private:
      compile::RtlBasicBlock &m_head;
      std::shared_ptr<compile::RtlVariable> m_index;
    };

    class CpuRtlTranslator : public CpuOpVisitor, public virtual CpuRtlInputTranslator {
    public:
      CpuRtlTranslator(compile::RtlFunction &fn);
      virtual ~CpuRtlTranslator();

      virtual void op(const CpuOp &op);
      virtual void newLoop(int size);
      virtual void endLoop();

      inline compile::RtlFunction &function() const { return m_function; }

      virtual std::shared_ptr<compile::RtlVariable> tensor(const std::string &baseName, CpuInputPtr tensor,
                                                           bool for_reading = true, bool for_writing = false) override;
      virtual void store_tensor(CpuInputPtr tensor, std::shared_ptr<compile::RtlVariable> v) override;

      virtual compile::RtlBasicBlock &block();

      virtual std::shared_ptr<compile::RtlVariable> input(const std::string &baseName, int ix,
                                                          std::shared_ptr<nnc::compile::RtlType> ty);
      virtual std::shared_ptr<compile::RtlVariable> loopIndex(int dim);
      virtual std::shared_ptr<compile::RtlVariable> scalar(const std::string &baseName, int scalarIx,
                                                           std::shared_ptr<nnc::compile::RtlType> ty);

      compile::RtlBasicBlock &entry();

      void finish();

    private:
      compile::RtlBasicBlock &curBlock();

      compile::RtlFunction &m_function;
      compile::RtlBasicBlock *m_curBlock, *m_entry;

      std::deque< CpuRtlLoop > m_loopHeads;

      std::unordered_map<int, std::shared_ptr<compile::RtlVariable> > m_inputs;
      std::unordered_map<int, std::shared_ptr<compile::RtlVariable> > m_scalars;
    };
  }
}

#endif
