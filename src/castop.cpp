#include "castop.hpp"

#include <iostream>

namespace nnc {
  namespace op {
    CastBaseOp::CastBaseOp(Tensor &input)
      : m_input(&input), m_shape(nullptr) {
    }

    CastBaseOp::CastBaseOp()
      : m_input(nullptr), m_shape(nullptr) {
    }

    CastBaseOp::~CastBaseOp() {
    }

    bool CastBaseOp::hasInput(const std::string &name) const {
      if ( name == "input" ) return m_input;
      else return false;
    }

    Tensor &CastBaseOp::input(const std::string &name) {
      if ( name == "input" ) return input();
      else throw exception::InputDoesNotExist(name);
    }

    Tensor &CastBaseOp::input() const {
      if ( !m_input ) throw exception::OperationInputNotReady("input");
      return *m_input;
    }

    void CastBaseOp::inputs(InputVisitor &v) const {
      v.input("input", "The tensor to source data from");
    }

    bool CastBaseOp::isReady() const {
      return hasInput("input");
    }

    const TensorShape &CastBaseOp::shape() const {
      return *m_shape;
    }

    void CastBaseOp::check(error::ErrorReporter &errors) {
      try {
        auto &i(input());
        if ( sourceDataType() != i.dataType() )
          errors.reportError(std::make_shared<error::DataTypeMismatch>("source", i.dataType(),
                                                                       "expected", sourceDataType()));

        m_shape = &i.shape();
      } catch ( exception::OperationInputNotReady &e ) {
        errors.reportError(std::make_shared<exception::OperationInputNotReady>(e));
      }
    }

    void CastBaseOp::compile(executor::BasicTensorOps &inputs,
                             std::shared_ptr<executor::BasicTensorInput> dest) {
      auto loop(inputs.elementWise(shape()));

      loop->cast(sourceDataType(), dataType(),
                 loop->fullyIndexed(inputs.input("input")),
                 loop->fullyIndexed(dest));
    }
  }
}
