#ifndef __nnc_op_matmul_HPP__
#define __nnc_op_matmul_HPP__

#include "op.hpp"
#include "tensor.hpp"
#include "executor/basic.hpp"

namespace nnc {
  namespace op {
    class MatMul : public virtual TensorOp, public virtual TensorShape,
                   public virtual executor::BasicTensorOp {
    public:
      MatMul(Tensor &a, Tensor &b);
      MatMul();
      virtual ~MatMul();

      Tensor &left() const;
      Tensor &right() const;

      virtual bool hasInput(const std::string &name) const;
      virtual Tensor &input(const std::string &name);
      virtual void inputs(InputVisitor &v) const;

      virtual bool isReady() const;

      virtual void check(error::ErrorReporter &e);

      virtual const DataType& dataType() const;
      virtual const TensorShape &shape() const;

      virtual int dims() const;
      virtual int sizeInDimension(int dim) const;

      virtual void compile(executor::BasicTensorOps &inputs,
                           std::shared_ptr<executor::BasicTensorInput> dest);

    private:
      Tensor *m_left, *m_right;
      const DataType *m_dataType;
      std::vector<int> m_nonMatrixDims;
      int m_rows, m_cols;
    };
  }
}

#endif
