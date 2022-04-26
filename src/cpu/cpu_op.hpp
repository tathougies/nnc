#ifndef __nnc_cpu_op_HPP__
#define __nnc_cpu_op_HPP__

#include "cpu.hpp"
#include "compile/rtl.hpp"
#include "compile/rtl_ops.hpp"

namespace nnc {
  namespace executor {
    typedef std::shared_ptr<CpuTensorInput> CpuTensorPtr;
    typedef std::shared_ptr<CpuInput> CpuInputPtr;

    class CpuRtlInputTranslator {
    public:
      virtual std::shared_ptr<compile::RtlVariable> tensor(CpuInputPtr tensor) =0;
      virtual void tensor(CpuInputPtr tensor, std::shared_ptr<compile::RtlVariable> v) =0;

      virtual std::shared_ptr<compile::RtlBasicBlock> block() =0;

      virtual std::shared_ptr<compile::RtlVariable> input(int inputIx, std::shared_ptr<compile::RtlType> expType) =0;
      virtual std::shared_ptr<compile::RtlVariable> loopIndex(int dim) =0;
    };

    class CpuRtlOp {
    public:
      virtual void makeRtlOp(compile::RtlFunction &fn,
                              CpuRtlInputTranslator &h) const =0;
    };

    class CpuCastOp : public CpuOp {
    public:
      CpuCastOp(const DataType &from, const DataType &to,
                CpuTensorPtr src,
                CpuTensorPtr dest);
      virtual ~CpuCastOp();

      virtual void print(std::ostream &out) const;

    private:
      const DataType &m_from, &m_to;
      CpuTensorPtr m_source, m_dest;
    };

    class CpuConstantOp : public CpuOp, public CpuRtlOp {
    public:
      CpuConstantOp(const DataType &dt, const void *d, std::size_t sz,
                    std::shared_ptr<CpuTensorInput> dest);
      virtual ~CpuConstantOp();

      virtual void print(std::ostream &out) const;

      virtual void makeRtlOp(compile::RtlFunction &fn,
                             CpuRtlInputTranslator &h) const;

    private:
      const DataType &m_type;
      std::vector<std::uint8_t> m_data;
      CpuTensorPtr m_dest;
    };

    class CpuLoopOp : public CpuOp, public CpuLoop {
    public:
      CpuLoopOp(int size, const CpuOpList &ops);
      virtual ~CpuLoopOp();

      virtual const CpuOpList &ops() const;
      virtual void addOp(const std::shared_ptr<CpuOp> &op);

      virtual void visit(CpuOpVisitor &v) const;
      virtual void print(std::ostream &out) const;

      inline int size() const { return m_size; }

    private:
      int m_size;
      CpuOpList m_ops;
    };

    class CpuBinOp : public CpuOp {
    public:
      CpuBinOp(const CpuTensorPtr &dest,
               const CpuTensorPtr &a,
               const CpuTensorPtr &b);
      virtual ~CpuBinOp();

      virtual void print(std::ostream &out) const;

      virtual const char *opName() const =0;

      virtual CpuTensorPtr a() const;
      virtual CpuTensorPtr b() const;
      virtual CpuTensorPtr dest() const;
      virtual const DataType &resultType() const;

    private:
      CpuTensorPtr m_a, m_b, m_dest;
    };

    class CpuBinRtlOp : public virtual CpuBinOp, public CpuRtlOp {
    public:
      CpuBinRtlOp(compile::RtlArithmetic bop);
      virtual ~CpuBinRtlOp();

      virtual void makeRtlOp(compile::RtlFunction &fn,
                             CpuRtlInputTranslator &h) const;

    private:
      compile::RtlArithmetic m_bop;
    };

    class CpuAddOp : public virtual CpuBinOp, public CpuBinRtlOp {
    public:
      CpuAddOp(const CpuTensorPtr &dest,
               const CpuTensorPtr &a,
               const CpuTensorPtr &b);
      virtual ~CpuAddOp();

      virtual const char *opName() const;
    };

    class CpuMulOp : public virtual CpuBinOp, public CpuBinRtlOp {
    public:
      CpuMulOp(const CpuTensorPtr &dest,
               const CpuTensorPtr &a,
               const CpuTensorPtr &b);
      virtual ~CpuMulOp();

      virtual const char *opName() const;
    };

    class CpuOpPrinter : public CpuOpVisitor {
    public:
      CpuOpPrinter(std::ostream &out);
      virtual ~CpuOpPrinter();

      virtual void op(const CpuOp &op);
      virtual void newLoop(int size);
      virtual void endLoop();

    private:
      void indent();

      int m_indent;
      std::ostream &m_out;
    };
  }
}

#endif
