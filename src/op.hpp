#ifndef __nnc_op_HPP__
#define __nnc_op_HPP__

#include <string>
#include <exception>
#include <memory>

#include "tensor.hpp"
#include "exception.hpp"
#include "error_reporter.hpp"

namespace nnc {
  namespace exception {
    class OperationNotReady : public std::exception {
    public:
      virtual ~OperationNotReady();

      virtual const char *what() const noexcept;
    };

    class InputDoesNotExist : public std::exception {
    public:
      InputDoesNotExist(const std::string &name);
      virtual ~InputDoesNotExist();

      virtual const char *what() const noexcept;

      inline const std::string &inputName() const { return m_name; }

    private:
      std::string m_name;
    };

    class OperationInputNotReady : public std::exception, public error::Error {
    public:
      OperationInputNotReady(const std::string &name);
      virtual ~OperationInputNotReady();

      virtual const char *what() const noexcept;

      inline const std::string &inputName() const { return m_name; }

      virtual std::ostream &report(std::ostream &o) const;

    private:
      std::string m_name;
    };
  }

  namespace op {
    class InputVisitor {
    public:
      virtual ~InputVisitor();

      virtual void input(const std::string &name, const std::string &description, bool required = true) =0;
    };

    class TensorOp : public virtual Tensor {
    public:
      virtual ~TensorOp();

      virtual bool hasInput(const std::string &name) const =0;
      virtual Tensor &input(const std::string &name) =0;
      virtual void inputs(InputVisitor &v) const =0;

      /* True if all parameters are set */
      virtual bool isReady() const =0;

      /* Check dimensionality of all parameters */
      virtual void check(error::ErrorReporter &e) =0;
    };
  }
}

#endif
