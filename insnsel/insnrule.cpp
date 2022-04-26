#include <iostream>

#include "insnrule.hpp"
#include "error.hpp"
#include "archdesc.hpp"

class InsnVarAlreadyDeclared : public NncCompileError, public std::exception {
public:
  InsnVarAlreadyDeclared(const InsnVariable &orig,
                         const InsnVariable &dup)
    : m_orig(orig), m_dup(dup) {
  }
  virtual ~InsnVarAlreadyDeclared() {
  }

  virtual const char *what() const noexcept {
    return "insn var already declared";
  }

protected:
  virtual void printOrigError(NncErrorReporter &reporter) {
    std::stringstream what;
    what << "Variable " << m_dup.name() << " declared multiple times";
    reporter.note(m_dup.where(), what.str(), true);
    reporter.note(m_orig.where(), "Originally declared here", true);
  }

private:
  InsnVariable m_orig, m_dup;
};

class InsnArgAlreadyMatched : public NncCompileError, public std::exception {
public:
  InsnArgAlreadyMatched(const std::string &nm,
                        const InsnArgPattern &orig,
                        const InsnArgPattern &dup)
    : m_name(nm), m_orig(orig.where()), m_dup(dup.where()) {
  }
  virtual ~InsnArgAlreadyMatched() {
  }

  virtual const char *what() const noexcept {
    return "insn arg already matched";
  }

protected:
  virtual void printOrigError(NncErrorReporter &reporter) {
    std::stringstream what;
    what << "Argument " << m_name << " matched multiple times";
    reporter.note(m_dup, what.str(), true);
    reporter.note(m_orig, "Originally declared here", true);
  }

private:
  std::string m_name;
  nnc::location m_orig, m_dup;
};

class InsnPatternVarVisitor : public InsnPatternVisitor {
public:
  InsnPatternVarVisitor(InsnVariableVisitor &v) : m_visitor(v) { }
  virtual ~InsnPatternVarVisitor() { }

  virtual void visit(const InsnPattern &p) {
    p.args().visitVariables(m_visitor);
  }

private:
  InsnVariableVisitor &m_visitor;
};

class PatternCounter : public InsnPatternVisitor {
public:
  PatternCounter() : m_count(0) { }
  virtual ~PatternCounter() { }

  void visit(const InsnPattern &p) { m_count ++; }

  inline int count() const { return m_count; }

private:
  int m_count;
};

InsnRuleModifier::~InsnRuleModifier() {
}

bool InsnRuleModifier::shouldDeleteInsnRuleModifierAfterModify() const { return true; }

InsnRuleBase::InsnRuleBase() {
}

InsnVariable &InsnRuleBase::var(const std::string &nm) {
  auto it(m_variables.find(nm));
  if ( it != m_variables.end() )
    return it->second;
  else {
    std::stringstream err;
    err << "Variable " << nm << " not found";
    throw std::runtime_error(err.str());
  }
}

bool InsnRuleBase::has_var(const std::string &nm) const {
  return m_variables.find(nm) != m_variables.end();
}

InsnRuleFactory::InsnRuleFactory() {
}

void InsnRuleFactory::param(const std::string &name, const Literal &l) {
  if ( name == "extraPrivate" ) {
    std::string extraPrivate;
    SetStringLiteral set(extraPrivate, WarnMissing("extraPrivate", "string"));
    l.visit(set);
    m_extraPrivate += template_string(extraPrivate);
  } else
    std::cerr << "Paramater " << name << " not valid for insnrul" << std::endl;
}

void InsnRuleFactory::declareVariable(const InsnVariable &v) {
  auto existing(m_variables.find(v.name()));
  if ( existing == m_variables.end() ) {
    m_variables.insert(std::make_pair(v.name(), v));
  } else {
    throw InsnVarAlreadyDeclared(v, existing->second);
  }
}

void InsnRuleFactory::setPattern(std::unique_ptr<InsnGen> &&gen) {
  m_pats = std::move(gen);
}

void InsnRuleFactory::addCondition(const template_string &c) {
  m_conds.push_front(c);
}

void InsnRuleFactory::addGeneratedInsn(AsmInsnCall &&c) {
  m_generate.emplace_back(std::move(c));
}

InsnRule *InsnRuleFactory::build(const std::string &name) {
  return new InsnRule(name, std::move(*this));
}

InsnRule::InsnRule(const std::string &name,
                    InsnRuleBase &&tmpl)
  : InsnRuleBase(std::move(tmpl)), m_name(name),
    m_context(NncErrorContextStack::current())
{
}

InsnVariable::InsnVariable(const nnc::location &loc, const std::string &name, InsnVariable::Type ty)
  : m_where(loc), m_name(name), m_type(ty) {
}

InsnRuleVarDecl::InsnRuleVarDecl(const nnc::location &loc, const std::string &name, InsnVariable::Type ty)
  : InsnVariable(loc, name, ty) {
 }

InsnRuleVarDecl::~InsnRuleVarDecl() {
}

void InsnRuleVarDecl::modify(InsnRuleFactory &f) {
  f.declareVariable(*this);
}

InsnCheckDecl::InsnCheckDecl(const nnc::location &l,
                             const std::string &cond)
  : m_loc(l), m_check(cond) {
}

InsnCheckDecl::~InsnCheckDecl() {
}

void InsnCheckDecl::modify(InsnRuleFactory &f) {
  std::stringstream func, fnName;
  fnName << "check_" << cName(*m_loc.begin.filename) << "_" << m_loc.begin.line << "_" << m_loc.begin.column;
  func << "bool " << fnName.str() << "() {" << std::endl;
  func << m_check << std::endl;
  func << "}" << std::endl;

  StringLiteral s(func.str());
  f.param("extraPrivate", s);

  f.addCondition(fnName.str() + "()");
}

InsnCondDecl::InsnCondDecl(const std::string &cond) {
  auto notSpace([](char c) { return !isspace(c); });

  auto firstSpace(std::find_if(cond.begin(), cond.end(), notSpace));
  auto lastSpace(std::find_if(std::make_reverse_iterator(cond.end()),
                              std::make_reverse_iterator(cond.begin()),
                              notSpace));
  std::string trimmed(firstSpace, lastSpace.base());
  template_string newCond(trimmed);

  m_cond = std::move(newCond);
}

InsnCondDecl::~InsnCondDecl() {
}

void InsnCondDecl::modify(InsnRuleFactory &f) {
  f.addCondition(m_cond);
}

int InsnGen::opCount() const {
  PatternCounter p;
  visitPatterns(p);
  return p.count();
}

void InsnGen::visitVars(InsnVariableVisitor &v) const {
  InsnPatternVarVisitor p(v);
  visitPatterns(p);
}

void InsnGen::modify(InsnRuleFactory &f) {
  f.setPattern(std::unique_ptr<InsnGen>(this));
}

bool InsnGen::shouldDeleteInsnRuleModifierAfterModify() const { return false; }

InsnPattern::InsnPattern(const std::string &s, InsnArgPatterns *pats)
  : m_mnemonic(s), m_pats(pats) {
}

InsnPattern::InsnPattern(InsnPattern &&p)
  : m_mnemonic(std::move(p.m_mnemonic)),
    m_pats(std::move(p.m_pats)) {
  p.m_pats = nullptr;
}

InsnPattern::~InsnPattern() {
  delete m_pats;
}

void InsnPattern::visitPatterns(InsnPatternVisitor &d) const {
  d.visit(*this);
}

void InsnPattern::declMnemonicInterest(MnemonicInterestDeclarer &d) const {
  d.declareInterest(mnemonic());
}

InsnRuleModifier *InsnPattern::modifier() {
  return static_cast<InsnRuleModifier *>(this);
}

InsnArgPattern::InsnArgPattern() {
}

InsnArgPattern::InsnArgPattern(const nnc::location &l,
                               Directionality dir,
                               const std::string &tyVar,
                               const std::string &regVar)
  : m_loc(l), m_dir(dir), m_tyVar(tyVar), m_regVar(regVar) {
}

InsnArgPattern::InsnArgPattern(const nnc::location &l,
                               Directionality dir,
                               const std::string &regVar)
  : m_loc(l), m_dir(dir), m_regVar(regVar) {
}

void InsnArgPattern::visitVariables(InsnVariableVisitor &v) const {
  if ( m_tyVar )
    v.visit(*m_tyVar, false);
  v.visit(m_regVar, false);
}

InsnArgPatterns::InsnArgPatterns() {
}

void InsnArgPatterns::addArgument(const std::string &arg,
                                  const InsnArgPattern &p) {
  auto existing(m_args.find(arg));
  if ( existing == m_args.end() ) {
    m_args.insert(std::make_pair(arg, p));
  } else {
    throw InsnArgAlreadyMatched(arg, p, existing->second);
  }
}

void InsnArgPatterns::visitVariables(InsnVariableVisitor &v) const {
  for ( const auto &arg : m_args ) {
    arg.second.visitVariables(v);
  }
}

InsnRuleAsmDecl::InsnRuleAsmDecl() {
}

InsnRuleAsmDecl::~InsnRuleAsmDecl() {
}

void InsnRuleAsmDecl::generateInsn(AsmInsnCall &call) {
  auto &newCall(m_calls.emplace_front());
  newCall.swap(call);
}

void InsnRuleAsmDecl::modify(InsnRuleFactory &f) {
  for ( auto &call : m_calls ) {
    AsmInsnCall c;
    c.swap(call);
    f.addGeneratedInsn(std::move(c));
  }
}

AsmInsnCall::AsmInsnCall()
  : m_context(NncErrorContextStack::current()) {
}

AsmInsnCall::AsmInsnCall(const std::string &mn)
  : m_context(NncErrorContextStack::current())
  , m_mnemonic(mn) {
}

void AsmInsnCall::addArg(const template_string &t) {
  m_args.push_front(t);
}

void AsmInsnCall::addArg(const std::string &nm, const template_string &t) {
  if ( !m_namedArgs.insert(std::make_pair(nm, t)).second ) {
    std::stringstream err;
    err << "Argument " << nm << " supplied more than once";
    throw NncParseError(err.str());
  }
}

void AsmInsnCall::swap(AsmInsnCall &c) {
  std::swap(m_context, c.m_context);
  std::swap(m_mnemonic, c.m_mnemonic);
  std::swap(m_args, c.m_args);
  std::swap(m_namedArgs, c.m_namedArgs);
}

AsmInsnArg::~AsmInsnArg() {
}

AsmInsnPositionalArg::AsmInsnPositionalArg(template_string &&s)
  : m_value(std::move(s)) {
}

AsmInsnPositionalArg::~AsmInsnPositionalArg() {
}

void AsmInsnPositionalArg::addToCall(AsmInsnCall &c) {
  c.addArg(m_value);
}

AsmInsnNamedArg::AsmInsnNamedArg(const std::string &name, template_string &&s)
  : AsmInsnPositionalArg(std::move(s)),
    m_name(name) {
}

AsmInsnNamedArg::~AsmInsnNamedArg() {
}

void AsmInsnNamedArg::addToCall(AsmInsnCall &c) {
  c.addArg(m_name, m_value);
}

InsnRuleCompoundPattern::~InsnRuleCompoundPattern() {
}

void InsnRuleCompoundPattern::visitPatterns(InsnPatternVisitor &d) const {
  for ( const auto &gen: m_gens )
    gen->visitPatterns(d);
}

void InsnRuleCompoundPattern::declMnemonicInterest(MnemonicInterestDeclarer &d) const {
  for ( const auto &gen: m_gens )
    gen->declMnemonicInterest(d);
}

InsnRuleConjunction::InsnRuleConjunction() {
}

InsnRuleConjunction::~InsnRuleConjunction() {
}

template<typename Compound, typename Maker, typename Getter>
InsnGen *makeCompound(InsnGen *a, InsnGen *b, const Maker &make, const Getter &getGens) {
  if ( a && b ) {
    Compound *ac(dynamic_cast<Compound *>(a));
    Compound *bc(dynamic_cast<Compound *>(b));
    if ( ac && bc ) {
      for ( auto &r : getGens(bc) )
        getGens(ac).emplace_back(std::move(r));

      delete bc;

      return ac;
    } else if ( ac ) {
      getGens(ac).push_back(std::unique_ptr<InsnGen>(b));
      return ac;
    } else if ( bc ) {
      getGens(bc).push_front(std::unique_ptr<InsnGen>(a));
      return bc;
    } else {
      Compound *ret(make());
      getGens(ret).push_back(std::unique_ptr<InsnGen>(a));
      getGens(ret).push_back(std::unique_ptr<InsnGen>(b));
      return ret;
    }
  }
  else if ( a ) return a;
  else if ( b ) return b;
  else return nullptr;
}

InsnGen *InsnRuleConjunction::make(InsnGen *a, InsnGen *b) {
  return makeCompound<InsnRuleConjunction>(a, b,
                                           []() -> InsnRuleConjunction *{ return new InsnRuleConjunction(); },
                                           [](InsnRuleConjunction *c) ->decltype(m_gens) & { return c->m_gens; });
}

InsnRuleDisjunction::InsnRuleDisjunction() {
}

InsnRuleDisjunction::~InsnRuleDisjunction() {
}

void InsnRuleDisjunction::visitPatterns(InsnPatternVisitor &d) const {
  InsnPatternBranch b(d);

  for ( const auto &gen: m_gens ) {
    b.newAlternative();
    gen->visitPatterns(d);
  }
}

InsnGen *InsnRuleDisjunction::make(InsnGen *a, InsnGen *b) {
  return makeCompound<InsnRuleDisjunction>(a, b,
                                           []() -> InsnRuleDisjunction *{ return new InsnRuleDisjunction(); },
                                           [](InsnRuleDisjunction *c) ->decltype(m_gens) & { return c->m_gens; });
}

InsnRuleOptionalPattern::InsnRuleOptionalPattern(std::unique_ptr<InsnGen> &&o)
  : m_optional(std::move(o)) {
}

InsnRuleOptionalPattern::~InsnRuleOptionalPattern() {
}

void InsnRuleOptionalPattern::visitPatterns(InsnPatternVisitor &d) const {
  InsnPatternBranch b(d);

  b.newAlternative();
  m_optional->visitPatterns(d);

  b.newAlternative();
}

void InsnRuleOptionalPattern::declMnemonicInterest(MnemonicInterestDeclarer &d) const {
  m_optional->declMnemonicInterest(d);
}

InsnGen *InsnRuleOptionalPattern::make(InsnGen *a) {
  auto existing(dynamic_cast<InsnRuleOptionalPattern *>(a));
  if ( existing ) return a;
  else return new InsnRuleOptionalPattern(std::unique_ptr<InsnGen>(a));
}

void InsnPatternVisitor::newBranch() { }
void InsnPatternVisitor::endBranch() { }
void InsnPatternVisitor::newAlternative() { }
