#ifndef __nnc_primop_HPP__
#define __nnc_primop_HPP__

#include "tensor.hpp"
#include "op.hpp"
#include "executor/basic.hpp"

namespace nnc {
  namespace op {
    class BinOp : public virtual TensorOp {
    public:
      BinOp(Tensor &a, Tensor &b);
      BinOp();

      virtual ~BinOp();

      virtual bool isReady() const;
      virtual bool hasInput(const std::string &name) const;
      virtual Tensor &input(const std::string &name);
      virtual void inputs(InputVisitor &v) const;

      virtual const char *opName() const =0;

      Tensor &left() const;
      Tensor &right() const;

    private:
      Tensor *m_a, *m_b;
    };

    class ElementWiseBinOp : public BinOp {
    public:
      ElementWiseBinOp(Tensor &a, Tensor &b);
      ElementWiseBinOp();
      virtual ~ElementWiseBinOp();

      virtual void check(error::ErrorReporter &e);
      virtual const DataType &dataType() const;
      virtual const TensorShape &shape() const;

    private:
      const DataType *m_dataType;
      const TensorShape *m_shape;
    };

    class AddOp : public ElementWiseBinOp, public executor::BasicTensorOp {
    public:
      inline AddOp(Tensor &a, Tensor &b) : ElementWiseBinOp(a, b) { }
      virtual ~AddOp();

      virtual const char *opName() const;

      virtual void compile(executor::BasicTensorOps &inputs,
                           std::shared_ptr<executor::BasicTensorInput> dest);
    };
  }
}

#endif
