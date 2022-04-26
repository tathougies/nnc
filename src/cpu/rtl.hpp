#ifndef __nnc_cpu_rtl_HPP__
#define __nnc_cpu_rtl_HPP__

#include "cpu/cpu_op.hpp"
#include "compile/rtl.hpp"

#include <deque>

namespace nnc {
  namespace executor {
    class CpuRtlTensor {
    public:
      virtual void load(compile::RtlFunction &fn,
                        CpuRtlInputTranslator &h,
                        std::shared_ptr<compile::RtlVariable> v) =0;
      virtual void store(compile::RtlFunction &fn,
                         CpuRtlInputTranslator &h,
                         std::shared_ptr<compile::RtlVariable> v) =0;
    };

    class CpuRtlLoop {
    public:
      CpuRtlLoop(std::shared_ptr<compile::RtlBasicBlock> head,
                 std::shared_ptr<compile::RtlVariable> ix);

      inline std::shared_ptr<compile::RtlBasicBlock> head() const { return m_head; }
      inline std::shared_ptr<compile::RtlVariable> index() const { return m_index; }

    private:
      std::shared_ptr<compile::RtlBasicBlock> m_head;
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

      virtual std::shared_ptr<compile::RtlVariable> tensor(CpuInputPtr tensor);
      virtual void tensor(CpuInputPtr tensor, std::shared_ptr<compile::RtlVariable> v);
      virtual std::shared_ptr<compile::RtlBasicBlock> block();

      virtual std::shared_ptr<compile::RtlVariable> input(int ix, std::shared_ptr<nnc::compile::RtlType> ty);
      virtual std::shared_ptr<compile::RtlVariable> loopIndex(int dim);

      std::shared_ptr<compile::RtlBasicBlock> entry();

      void finish();

    private:
      std::shared_ptr<compile::RtlBasicBlock> curBlock();

      compile::RtlFunction &m_function;
      std::shared_ptr<compile::RtlBasicBlock> m_curBlock, m_entry;

      std::deque< CpuRtlLoop > m_loopHeads;

      std::unordered_map<int, std::shared_ptr<compile::RtlVariable> > m_inputs;
    };
  }
}

#endif
