#include "categorical.hpp"

namespace nnc {
  namespace op {
    ToCategoricalBase::ToCategoricalBase(Tensor &input, std::uint32_t max_classes)
      : m_maxClasses(max_classes),
        m_input(&input) {
    }

    ToCategoricalBase::ToCategoricalBase()
      : m_maxClasses(~0),
        m_input(nullptr) {
    }

    ToCategoricalBase::~ToCategoricalBase() {
    }

    bool ToCategoricalBase::isReady() const { return m_input; }

    bool ToCategoricalBase::hasInput(const std::string &name) const {
      if ( name == "category" ) return m_input;
      else throw exception::InputDoesNotExist(name);
    }

    Tensor &ToCategoricalBase::input(const std::string &name) {
      if ( name == "category" ) return input();
      else throw exception::InputDoesNotExist(name);
    }

    void ToCategoricalBase::inputs(InputVisitor &v) const {
      v.input("category", "A scalar indicating which category we are in");
    }

    Tensor &ToCategoricalBase::input() const {
      if ( !m_input ) throw exception::OperationInputNotReady("category");
      return *m_input;
    }

    void ToCategoricalBase::check(error::ErrorReporter &errors) {
      DefaultDataType<std::uint32_t> dint;

      try {
        auto &in(input());

        if ( in.dataType() != dint )
          errors.reportError(std::make_shared<error::DataTypeMismatch>("input", in.dataType(),
                                                                       "expected", dint));

        if ( in.shape().dims() != 0 || in.shape().totalSize() != 1 )
          errors.reportError(std::make_shared<error::ScalarRequired>("input"));

      } catch ( exception::OperationInputNotReady &e) {
        errors.reportError(std::make_shared<exception::OperationInputNotReady>(e));
      }
    }

    const TensorShape &ToCategoricalBase::shape() const {
      return *this;
    }

    int ToCategoricalBase::dims() const {
      return 1;
    }

    int ToCategoricalBase::sizeInDimension(int dim) const {
      if ( dim == 0 ) return m_maxClasses;
      else
        throw exception::InvalidDimension(dim, 1);
    }
  }
}
