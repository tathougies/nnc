#include <iostream>
#include <iterator>

#include "primop.hpp"
#include "errors.hpp"

namespace nnc {
  namespace exception {
    OperationNotReady::~OperationNotReady() {
    }

    const char *OperationNotReady::what() const noexcept {
      return "Operation not ready";
    }

    InputDoesNotExist::InputDoesNotExist(const std::string &name)
      : m_name(name) {
    }

    InputDoesNotExist::~InputDoesNotExist() {
    }

    const char *InputDoesNotExist::what() const noexcept {
      return "Input does not exist";
    }

    OperationInputNotReady::OperationInputNotReady(const std::string &name)
      : m_name(name) {
    }

    OperationInputNotReady::~OperationInputNotReady() {
    }

    const char *OperationInputNotReady::what() const noexcept {
      return "Operation input not ready";
    }

    std::ostream &OperationInputNotReady::report(std::ostream &o) const {
      o << "The input '" << inputName() << "' was not provided";
      return o;
    }
  }

  namespace op {
    InputVisitor::~InputVisitor() {
    }

    TensorOp::~TensorOp() {
    }

    BinOp::BinOp(Tensor &a, Tensor &b)
      : m_a(&a), m_b(&b) {
    }

    BinOp::BinOp()
      : m_a(nullptr), m_b(nullptr) {
    }

    BinOp::~BinOp() {
    }

    bool BinOp::isReady() const {
      return m_a && m_b;
    }

    bool BinOp::hasInput(const std::string &name) const {
      if ( name == "left" ) {
        return m_a;
      } else if ( name == "right" ) {
        return m_b;
      } else
        throw exception::InputDoesNotExist(name);
    }

    Tensor &BinOp::input(const std::string &name) {
      if ( name == "left" ) { return left(); }
      else if ( name == "right" ) { return right(); }
      else throw exception::InputDoesNotExist(name);
    }

    void BinOp::inputs(InputVisitor &v) const {
      v.input("left", "The left operand of the operation");
      v.input("right", "The right operand of the operation");
    }

    Tensor &BinOp::left() const {
      if ( !m_a ) throw exception::OperationInputNotReady("left");
      return *m_a;
    }

    Tensor &BinOp::right() const {
      if ( !m_b ) throw exception::OperationInputNotReady("right");
      return *m_b;
    }

    ElementWiseBinOp::ElementWiseBinOp(Tensor &a, Tensor &b)
      : BinOp(a, b),
        m_dataType(nullptr), m_shape(nullptr) {
    }

    ElementWiseBinOp::ElementWiseBinOp()
      : BinOp(),
        m_dataType(nullptr), m_shape(nullptr) {
    }

    ElementWiseBinOp::~ElementWiseBinOp() {
    }

    void ElementWiseBinOp::check(error::ErrorReporter &errors) {
      try {
        auto &a(left());
        auto &b(right());

        if ( a.dataType() != b.dataType() )
          errors.reportError(std::make_shared<error::DataTypeMismatch>("left", a.dataType(),
                                                                       "right", b.dataType()));

        m_dataType = &a.dataType();

        if ( a.shape() != b.shape() )
          errors.reportError(std::make_shared<error::ShapeMismatch>("left", a.shape(),
                                                                    "right", b.shape()));

        m_shape = &a.shape();
      } catch (exception::OperationInputNotReady &e) {
        errors.reportError(std::make_shared<exception::OperationInputNotReady>(e));
      }
    }

    const DataType &ElementWiseBinOp::dataType() const {
      if ( !m_dataType ) throw exception::OperationNotReady();
      return *m_dataType;
    }

    const TensorShape &ElementWiseBinOp::shape() const {
      if ( !m_shape ) throw exception::OperationNotReady();
      return *m_shape;
    }

    AddOp::~AddOp() {
    }

    const char *AddOp::opName() const {
      return "add";
    }

    void AddOp::compile(executor::BasicTensorOps &inputs,
                        std::shared_ptr<executor::BasicTensorInput> dest) {
      auto loop(inputs.elementWise(shape()));

      loop->add(loop->fullyIndexed(inputs.input("left")),
                loop->fullyIndexed(inputs.input("right")),
                loop->fullyIndexed(dest));
    }
  }
}
