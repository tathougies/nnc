#ifndef __insnsel_error_HPP__
#define __insnsel_error_HPP__

#include "location.hh"

#include <list>

class NncErrorContextStack;

class NncErrorReporter {
public:
  virtual void note(const nnc::location &where,
                    const std::string &what,
                    bool showListing = false) =0;
};

class NncErrorContext {
public:
  NncErrorContext(const nnc::location &loc, const std::string &what,
                  bool showListing = false);
  NncErrorContext(const NncErrorContext &ctxt);

  inline const nnc::location &location() const { return m_loc; }
  inline const std::string &what() const { return m_what; }
  inline bool showListing() const { return m_showListing; }

protected:
  nnc::location m_loc;
  std::string m_what;
  bool m_showListing;
};

class NncErrorLocation : public NncErrorContext {
public:
  NncErrorLocation(const std::string &what, bool showListing = false);
  NncErrorLocation(const nnc::location &loc, const std::string &what,
                   bool showListing = false);
  ~NncErrorLocation();

private:
  bool m_added;
};

class NncErrorContextStack {
public:
  NncErrorContextStack(NncErrorContextStack &&e) =default;
  NncErrorContextStack(const NncErrorContextStack &e) =default;

  static NncErrorContextStack current();
  static NncErrorContextStack empty();
  NncErrorContextStack rest() const;

  NncErrorContextStack &operator=(const NncErrorContextStack &e) =default;

  nnc::location location() const;

private:
  NncErrorContextStack();
  NncErrorContextStack(bool dontFill);

  template<typename It>
  NncErrorContextStack(const It &begin, const It &end) {
    std::copy(begin, end, std::back_inserter(m_context));
  }

  std::list<NncErrorContext> m_context;
  friend class NncGenerationError;
  friend class NncCompileError;
};

class NncCompileError {
public:
  NncCompileError();
  virtual ~NncCompileError();

  void printError(NncErrorReporter &reporter);

protected:
  NncCompileError(const NncErrorContextStack &m_context);
  virtual void printOrigError(NncErrorReporter &reporter) =0;

  void printErrorStack(const NncErrorContextStack &stk, NncErrorReporter &reporter) const;

  NncErrorContextStack m_context;
};

class NncGenerationError : public std::exception, public NncCompileError {
public:
  NncGenerationError(const std::string &what);
  NncGenerationError(const NncErrorContextStack &stk,
                     const std::string &what);
  NncGenerationError(const NncErrorContextStack &stk,
                     const std::stringstream &what);
  virtual ~NncGenerationError();

  virtual const char *what() const noexcept;

protected:
  virtual void printOrigError(NncErrorReporter &reporter);

private:
  std::string m_what;
};

class NncParseError : public std::exception, public NncCompileError {
public:
  NncParseError(const nnc::location &l,
                const std::string &what);
  NncParseError(const std::string &what);
  virtual ~NncParseError();

  virtual const char *what() const noexcept;

  inline const nnc::location &location() const { return m_loc; }
  inline const std::string &parseError() const { return m_what; }

protected:
  virtual void printOrigError(NncErrorReporter &reporter);

private:
  nnc::location m_loc;
  std::string m_what;
};

class NncSimpleErrorReporter : public NncErrorReporter {
public:
  virtual ~NncSimpleErrorReporter();

  virtual void note(const nnc::location &where,
                    const std::string &what,
                    bool showListing = false);
};

#endif
