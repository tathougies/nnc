#ifndef __nnc_categorical_HPP__
#define __nnc_categorical_HPP__

#include "op.hpp"
#include "data_type.hpp"
#include "tensor.hpp"
#include "executor/basic.hpp"

#include <iostream>

namespace nnc {
  namespace op {
    class ToCategoricalBase : public virtual TensorOp, public virtual TensorShape,
                              public virtual executor::BasicTensorOp {
    public:
      ToCategoricalBase(Tensor &input, std::uint32_t max_classes);
      ToCategoricalBase();
      virtual ~ToCategoricalBase();

      virtual bool isReady() const;

      virtual bool hasInput(const std::string &name) const;
      virtual Tensor &input(const std::string &name);
      virtual void inputs(InputVisitor &v) const;
      Tensor &input() const;

      virtual void check(error::ErrorReporter &e);

      virtual const TensorShape &shape() const;
      virtual int dims() const;
      virtual int sizeInDimension(int dim) const;

    private:
      std::uint32_t m_maxClasses;
      Tensor *m_input;
    };

    template<typename T>
    class ToCategorical : public virtual ToCategoricalBase {
    public:
      ToCategorical(Tensor &input, std::uint32_t max_classes,
                    T on = DefaultDataType<T>::unit_value,
                    T off = DefaultDataType<T>::zero_value)
        : ToCategoricalBase(input, max_classes), m_on(on), m_off(off) {
      }
      ToCategorical(T on = DefaultDataType<T>::unit_value,
                    T off = DefaultDataType<T>::zero_value)
        : m_on(on), m_off(off) {
      }

      virtual ~ToCategorical() { }

      virtual const DataType &dataType() const { return m_dataType; }

      virtual void compile(executor::BasicTensorOps &inputs,
                           std::shared_ptr<executor::BasicTensorInput> dest) {
        char off[m_dataType.size()], on[m_dataType.size()];

        m_dataType.encode(m_off, off);
        m_dataType.encode(m_on, on);

        std::cerr << "Categorical outer loop count " << inputs.loopCount() << std::endl;
        {
          auto loop(inputs.elementWise(shape()));
          std::cerr << "Categorical inner loop count " << loop->loopCount() << std::endl;
          loop->constant(off, m_dataType.size(), loop->fullyIndexed(dest));
        }

        auto cat(inputs.fullyIndexed(inputs.input("category")));
        std::vector< std::shared_ptr<executor::BasicTensorInput> > catIx, destIx;
        inputs.fullIndex(catIx);
        inputs.fullIndex(destIx);

        destIx.push_back(inputs.index(cat, catIx));
        inputs.constant(on, m_dataType.size(), inputs.index(dest, destIx));
      }

    private:
      T m_on, m_off;
      DefaultDataType<T> m_dataType;
    };
  }
}

#endif
