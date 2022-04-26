#ifndef __nnc_error_reporter_HPP__
#define __nnc_error_reporter_HPP__

#include <ostream>
#include <memory>

#include "errors.hpp"

namespace nnc {
  namespace error {
    class ErrorReporter {
    public:
      virtual ~ErrorReporter();

      virtual void reportError(const std::shared_ptr<Error> &e) =0;
    };

    class OstreamErrorReporter : public ErrorReporter {
    public:
      OstreamErrorReporter(std::ostream &o);
      virtual ~OstreamErrorReporter();

      virtual void reportError(const std::shared_ptr<Error> &e);

      inline std::ostream &out() const { return m_out; }

    private:
      std::ostream &m_out;
    };

    class NullErrorReporter : public ErrorReporter {
    public:
      virtual ~NullErrorReporter();

      virtual void reportError(const std::shared_ptr<Error> &e);
    };

    class SummaryErrorReporter : public ErrorReporter {
    public:
      SummaryErrorReporter();
      virtual ~SummaryErrorReporter();

      virtual void reportError(const std::shared_ptr<Error> &e);

      inline int errors() const { return m_errorCount; }

    private:
      int m_errorCount;
    };

    class TeeErrorReporter : public ErrorReporter {
    public:
      TeeErrorReporter(ErrorReporter &left,
                       ErrorReporter &right);
      virtual ~TeeErrorReporter();

      virtual void reportError(const std::shared_ptr<Error> &e);

    private:
      ErrorReporter &m_left, &m_right;
    };
  }
}

#endif
