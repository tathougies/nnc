#include "errors.hpp"

#include <iterator>

namespace nnc {
  namespace error {
    DataTypeMismatch::DataTypeMismatch(const std::string &inputName1, const DataType &inputType1,
                                       const std::string &inputName2, const DataType &inputType2)
      : m_input1(inputName1), m_input2(inputName2),
        m_inputType1(inputType1.name()), m_inputType2(inputType2.name()) {
    }

    DataTypeMismatch::~DataTypeMismatch() {
    }

    std::ostream &DataTypeMismatch::report(std::ostream &o) const {
      o << "Types of inputs did not match:" << std::endl;
      o << "  Input '" << m_input1 << "' had type " << m_inputType1 << std::endl;
      o << "  but   '" << m_input2 << "' had type " << m_inputType2;
      return o;
    }

    ShapeMismatch::ShapeMismatch(const std::string &inputName1, const TensorShape &shape1,
                                 const std::string &inputName2, const TensorShape &shape2)
      : m_input1(inputName1), m_input2(inputName2),
        m_inputShape1(shape1.asVector()), m_inputShape2(shape2.asVector()) {
    }

    ShapeMismatch::~ShapeMismatch() {
    }

    std::ostream &ShapeMismatch::report(std::ostream &o) const {
      o << "The shapes of inputs did not match:" << std::endl;
      o << "  Input '" << m_input1 << "' had shape {";
      std::copy(m_inputShape1.begin(), m_inputShape1.end(), std::ostream_iterator<int>(o, ","));
      o << "}" << std::endl;
      o << "    but '" << m_input2 << "' had shape {";
      std::copy(m_inputShape2.begin(), m_inputShape2.end(), std::ostream_iterator<int>(o, ","));
      o << "}" << std::endl;
      return o;
    }

    ScalarRequired::ScalarRequired(const std::string &name)
      : m_name(name) {
    }

    ScalarRequired::~ScalarRequired() {
    }

    std::ostream &ScalarRequired::report(std::ostream &o) const {
      o << "A scalar is required for input '" << m_name << "'";
      return o;
    }

    DimensionMismatch::DimensionMismatch(const std::string &opName,
                                         const std::string &nm1, int dims1,
                                         const std::string &nm2, int dims2)
      : m_opName(opName),
        m_left(nm1), m_right(nm2),
        m_leftDims(dims1), m_rightDims(dims2) {
    }

    DimensionMismatch::~DimensionMismatch() {
    }

    std::ostream &DimensionMismatch::report(std::ostream &o) const {
      o << "Operation '" << m_opName << "' requires operands to have identical dimensions" << std::endl;
      o << "  Argument '" << m_left << "' has " << m_leftDims << "dimension(s)" << std::endl;
      o << "  Argument '" << m_right << "' has " << m_rightDims;

      return o;
    }

    SizeMismatch::SizeMismatch(const std::string &nm1, const TensorShape &shape1,
                               const std::string &nm2, const TensorShape &shape2,
                               int dim)
      : m_nm1(nm1), m_nm2(nm2),
        m_d1(shape1.sizeInDimension(dim)),
        m_d2(shape2.sizeInDimension(dim)),
        m_dim(dim) {
    }

    SizeMismatch::~SizeMismatch() {
    }

    std::ostream &SizeMismatch::report(std::ostream &o) const {
      o << "Arguments have a size mismatch in dimension " << m_dim << std::endl;
      o << "  Argument '" << m_nm1 << "' has size " << m_d1 << std::endl;
      o << "  Argument '" << m_nm2 << "' has size " << m_d2;

      return o;
    }
  }
}
