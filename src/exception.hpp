#ifndef __nnc_exception_HPP__
#define __nnc_exception_HPP__

#include <string>
#include <exception>

namespace nnc {
  namespace exception {
    class InvalidDimension : public std::exception {
    public:
      InvalidDimension(int requested_dim, int max_dim)
        : m_requested_dim(requested_dim), m_max_dim(max_dim)
      {
      }
      virtual ~InvalidDimension();

      inline int requested_dim() const { return m_requested_dim; }
      inline int max_dim() const { return m_max_dim; }

      virtual const char *what() const noexcept;

    private:
      int m_requested_dim, m_max_dim;
    };

    class NotImplemented : public std::exception {
    public:
      NotImplemented(const std::string &what);
      virtual ~NotImplemented();

      virtual const char *what() const noexcept;

    private:
      std::string m_what;
    };
  }
}

#endif
