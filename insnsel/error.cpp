#include "error.hpp"
#include <list>
#include <sstream>

static std::list<NncErrorLocation *> globalContext;

NncErrorLocation::NncErrorLocation(const nnc::location &loc, const std::string &what,
                                   bool showListing)
  : NncErrorContext(loc, what, showListing),
    m_added(true) {
  globalContext.push_front(this);
}

NncErrorLocation::NncErrorLocation(const std::string &what, bool showListing)
  : NncErrorContext(nnc::location(), what, showListing),
    m_added(false) {

  if ( !globalContext.empty() ) {
    m_loc = globalContext.front()->location();
    m_added = true;
    globalContext.push_front(this);
  }
}

NncErrorLocation::~NncErrorLocation() {
  if (m_added) {
    globalContext.pop_front();
  }
}

NncErrorContext::NncErrorContext(const nnc::location &loc, const std::string &what,
                                 bool showListing)
  : m_loc(loc), m_what(what), m_showListing(showListing) {
}

NncErrorContext::NncErrorContext(const NncErrorContext &ctxt)
  : m_loc(ctxt.m_loc),
    m_what(ctxt.m_what),
    m_showListing(ctxt.m_showListing) {
}

NncCompileError::NncCompileError(const NncErrorContextStack &e)
  : m_context(e) {
}

NncCompileError::NncCompileError()
  : m_context(NncErrorContextStack::current()) {
}

NncCompileError::~NncCompileError() {
}

void NncCompileError::printError(NncErrorReporter &reporter) {
  printOrigError(reporter);
  printErrorStack(m_context, reporter);
}

void NncCompileError::printErrorStack(const NncErrorContextStack &stk, NncErrorReporter &reporter) const {
  for ( const NncErrorContext &c : stk.m_context ) {
    reporter.note(c.location(), c.what(), c.showListing());
  }
}

NncParseError::NncParseError(const nnc::location &l,
                             const std::string &what)
  : m_loc(l), m_what(what) {
}

NncParseError::NncParseError(const std::string &what)
  : m_what(what) {
  m_loc = m_context.location();
}

NncParseError::~NncParseError() {
}

const char *NncParseError::what() const noexcept {
  return "nnc parse error";
}

void NncParseError::printOrigError(NncErrorReporter &reporter) {
  reporter.note(m_loc, m_what, true);
}

NncSimpleErrorReporter::~NncSimpleErrorReporter() {
}

void NncSimpleErrorReporter::note(const nnc::location &where,
                                  const std::string &what,
                                  bool showListing) {
  std::cerr << " * " << where << ": " << what << std::endl;
}

NncErrorContextStack NncErrorContextStack::current() {
  return NncErrorContextStack();
}

NncErrorContextStack NncErrorContextStack::empty() {
  return NncErrorContextStack(false);
}

NncErrorContextStack NncErrorContextStack::rest() const {
  if ( m_context.empty() ) return NncErrorContextStack::empty();
  return NncErrorContextStack(++m_context.begin(), m_context.end());
}

NncErrorContextStack::NncErrorContextStack(bool dontFill) {
}

NncErrorContextStack::NncErrorContextStack() {
  std::transform(globalContext.begin(), globalContext.end(),
                 std::back_inserter(m_context),
                 [](NncErrorContext *e) {
                   return *e;
                 });
}

nnc::location NncErrorContextStack::location() const {
  if ( m_context.empty() ) return nnc::location();
  else return m_context.front().location();
}

NncGenerationError::NncGenerationError(const std::string &what)
  : NncCompileError(NncErrorContextStack::empty()), m_what(what) {
}

NncGenerationError::NncGenerationError(const NncErrorContextStack &stk,
                                       const std::string &what)
  : NncCompileError(stk),
    m_what(what) {
}

NncGenerationError::NncGenerationError(const NncErrorContextStack &stk,
                                       const std::stringstream &what)
  : NncCompileError(stk),
    m_what(what.str()) {
}

NncGenerationError::~NncGenerationError() {
}

const char *NncGenerationError::what() const noexcept {
  return "generation error";
}

void NncGenerationError::printOrigError(NncErrorReporter &reporter) {
  nnc::location where = m_context.location();
  reporter.note(where, m_what);
}