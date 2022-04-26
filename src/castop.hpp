#ifndef __nnc_castop_HPP__
#define __nnc_castop_HPP__

#include "tensor.hpp"
#include "op.hpp"
#include "data_type.hpp"
#include "executor/basic.hpp"

namespace nnc {
  namespace op {
    class CastBaseOp : public virtual TensorOp, public virtual executor::BasicTensorOp {
    public:
      CastBaseOp(Tensor &input);
      CastBaseOp();
      virtual ~CastBaseOp();

      virtual const DataType &sourceDataType() const =0;

      virtual bool hasInput(const std::string &name) const;
      virtual Tensor &input(const std::string &name);
      virtual void inputs(InputVisitor &v) const;
      virtual bool isReady() const;

      Tensor &input() const;

      virtual const TensorShape &shape() const;
      virtual void check(error::ErrorReporter &e);

      virtual void compile(executor::BasicTensorOps &inputs,
                           std::shared_ptr<executor::BasicTensorInput> dest);

    private:
      Tensor *m_input;
      const TensorShape *m_shape;
    };

    template<typename From, typename To>
    class Cast : public virtual CastBaseOp {
    public:
      Cast(Tensor &input) : CastBaseOp(input) { }
      Cast() { }

      virtual ~Cast() { }

      virtual const DataType &dataType() const { return m_dest; }
      virtual const DataType &sourceDataType() const { return m_source; }

    private:
      DefaultDataType<From> m_source;
      DefaultDataType<To> m_dest;
    };
  }
}

#endif
