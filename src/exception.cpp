#include "exception.hpp"
#include "mnist.hpp"

namespace nnc {
  namespace exception {
    InvalidDimension::~InvalidDimension() {
    }

    const char *InvalidDimension::what() const noexcept {
      return "Invalid dimension";
    }

    InvalidMnistException::InvalidMnistException(const char *what)
      : m_msg(what) {
    }

    InvalidMnistException::~InvalidMnistException() {
    }

    const char *InvalidMnistException::what() const noexcept {
      return m_msg;
    }

    NotImplemented::NotImplemented(const std::string &what)
      : m_what(what) {
    }

    NotImplemented::~NotImplemented() {
    }

    const char *NotImplemented::what() const noexcept {
      return m_what.c_str();
    }
  }
}
