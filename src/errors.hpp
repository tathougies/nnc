#ifndef __nnc_errors_HPP__
#define __nnc_errors_HPP__

#include <ostream>
#include <memory>
#include <string>

#include "tensor.hpp"

namespace nnc {
  namespace error {
    class Error {
    public:
      virtual ~Error();

      virtual std::ostream &report(std::ostream &o) const =0;
    };

    class DataTypeMismatch : public Error {
    public:
      DataTypeMismatch(const std::string &inputName1, const DataType &inputType1,
                       const std::string &inputName2, const DataType &inputType2);
      virtual ~DataTypeMismatch();

      virtual std::ostream &report(std::ostream &o) const;

    private:
      std::string m_input1, m_input2;
      std::string m_inputType1, m_inputType2;
    };

    class ShapeMismatch : public Error {
    public:
      ShapeMismatch(const std::string &inputName1, const TensorShape &inputShape1,
                    const std::string &inputName2, const TensorShape &inputShape2);
      virtual ~ShapeMismatch();

      virtual std::ostream &report(std::ostream &o) const;

    private:
      std::string m_input1, m_input2;
      std::vector<int> m_inputShape1, m_inputShape2;
    };

    class ScalarRequired : public Error {
    public:
      ScalarRequired(const std::string &name);
      virtual ~ScalarRequired();

      virtual std::ostream &report(std::ostream &o) const;

    private:
      std::string m_name;
    };

    class DimensionMismatch : public Error {
    public:
      DimensionMismatch(const std::string &opName,
                        const std::string &nm1, int dims1,
                        const std::string &nm2, int dims2);
      virtual ~DimensionMismatch();

      virtual std::ostream &report(std::ostream &o) const;

    private:
      std::string m_opName;
      std::string m_left, m_right;
      int m_leftDims, m_rightDims;
    };

    class SizeMismatch : public Error {
    public:
      SizeMismatch(const std::string &nm1, const TensorShape &shape1,
                   const std::string &nm2, const TensorShape &shape2,
                   int dim);
      virtual ~SizeMismatch();

      virtual std::ostream &report(std::ostream &o) const;

    private:
      std::string m_nm1, m_nm2;
      int m_d1, m_d2, m_dim;
    };
  }
}

#endif
