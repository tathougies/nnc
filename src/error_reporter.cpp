#include <iostream>

#include "error_reporter.hpp"

namespace nnc {
  namespace error {
    Error::~Error() {
    }

    ErrorReporter::~ErrorReporter() {
    }

    OstreamErrorReporter::OstreamErrorReporter(std::ostream &o)
      : m_out(o) {
    }

    OstreamErrorReporter::~OstreamErrorReporter() {
    }

    void OstreamErrorReporter::reportError(const std::shared_ptr<Error> &e) {
      e->report(out()) << std::endl;
    }

    NullErrorReporter::~NullErrorReporter() {
    }

    void NullErrorReporter::reportError(const std::shared_ptr<Error> &e) {
    }

    SummaryErrorReporter::SummaryErrorReporter()
      : m_errorCount(0) {
    }

    SummaryErrorReporter::~SummaryErrorReporter() {
    }

    void SummaryErrorReporter::reportError(const std::shared_ptr<Error> &e) {
      m_errorCount++;
    }

    TeeErrorReporter::TeeErrorReporter(ErrorReporter &left,
                                       ErrorReporter &right)
      : m_left(left), m_right(right) {
    }

    TeeErrorReporter::~TeeErrorReporter() {
    }

    void TeeErrorReporter::reportError(const std::shared_ptr<Error> &e) {
      m_left.reportError(e);
      m_right.reportError(e);
    }
  }
}
