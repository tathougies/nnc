#include "reshape.hpp"
#include "errors.hpp"

#include <iostream>
#include <experimental/iterator>

namespace nnc {
  namespace error {
    class InvalidReshape : public Error {
    public:
      InvalidReshape(const TensorShape &from,
                     const TensorShape &to)
        : m_from(from.asVector()),
          m_to(to.asVector()) {
      }
      virtual ~InvalidReshape() {
      }

      std::ostream &report(std::ostream &o) const {
        o << "Cannot reshape vector from {";
        std::copy(m_from.begin(), m_from.end(), std::experimental::make_ostream_joiner(o, ", "));
        o << "} to {";
        std::copy(m_from.begin(), m_from.end(), std::experimental::make_ostream_joiner(o, ", "));
        o << "}";

        return o;
      }

    private:
      std::vector<int> m_from, m_to;
    };
  }

  namespace op {
    Reshape::Reshape(std::initializer_list<int> newShape, Tensor &input)
      : m_shape(newShape),
        m_input(&input) {
      std::reverse(m_shape.begin(), m_shape.end());
    }

    Reshape::Reshape(const std::vector<int> &newShape,
                     Tensor &input)
      : m_shape(newShape),
        m_input(&input) {
      std::reverse(m_shape.begin(), m_shape.end());
    }

    Reshape::Reshape(const std::vector<int> &newShape)
      : m_shape(newShape),
        m_input(nullptr) {
      std::reverse(m_shape.begin(), m_shape.end());
    }

    Reshape::~Reshape() {
    }

    Tensor &Reshape::source() const {
      if ( !m_input ) throw exception::OperationInputNotReady("source");
      return *m_input;
    }

    const DataType &Reshape::dataType() const {
      return source().dataType();
    }

    const TensorShape &Reshape::shape() const {
      return *this;
    }

    int Reshape::dims() const {
      return m_shape.size();
    }

    int Reshape::sizeInDimension(int dim) const {
      if ( dim >= dims() )
        throw exception::InvalidDimension(dim, dims());
      return m_shape[dim];
    }

    bool Reshape::hasInput(const std::string &name) const {
      if ( name == "source" ) return m_input;
      else throw exception::InputDoesNotExist(name);
    }

    Tensor &Reshape::input(const std::string &name) {
      if ( name == "source" ) return source();
      else throw exception::InputDoesNotExist(name);
    }

    void Reshape::inputs(InputVisitor &v) const {
      v.input("source", "The tensor to reshape");
    }

    bool Reshape::isReady() const {
      return hasInput("source");
    }

    void Reshape::check(error::ErrorReporter &e) {
      try {
        auto &src(source());

        if ( src.shape().totalSize() != totalSize() )
          e.reportError(std::make_shared<error::InvalidReshape>(src.shape(), *this));
      } catch ( exception::OperationInputNotReady &x ) {
        e.reportError(std::make_shared<exception::OperationInputNotReady>(x));
      }
    }

    void Reshape::compile(executor::BasicTensorOps &inputs,
                          std::shared_ptr<executor::BasicTensorInput> dest) {
      inputs.reshape(shape(), inputs.input("source"), dest);
    }
  }
}
