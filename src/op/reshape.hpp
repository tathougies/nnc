#ifndef __nnc_op_reshape_HPP__
#define __nnc_op_reshape_HPP__

#include "op.hpp"
#include "error_reporter.hpp"
#include "executor/basic.hpp"

namespace nnc {
  namespace op {
    class Reshape : public virtual TensorOp, public virtual TensorShape, public virtual executor::BasicTensorOp {
    public:
      Reshape(std::initializer_list<int> newShape, Tensor &input);
      Reshape(const std::vector<int> &newShape,
              Tensor &input);
      Reshape(const std::vector<int> &newShape);
      virtual ~Reshape();

      Tensor &source() const;

      virtual const DataType &dataType() const;
      virtual const TensorShape &shape() const;

      virtual int dims() const;
      virtual int sizeInDimension(int dim) const;

      virtual bool hasInput(const std::string &name) const;
      virtual Tensor &input(const std::string &name);
      virtual void inputs(InputVisitor &v) const;
      virtual bool isReady() const;

      virtual void check(error::ErrorReporter &e);

      virtual void compile(executor::BasicTensorOps &inputs,
                           std::shared_ptr<executor::BasicTensorInput> dest);

    private:
      std::vector<int> m_shape;
      Tensor *m_input;
    };
  }
}

#endif
