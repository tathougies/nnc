#include "insn.hpp"
#include "archdesc.hpp"

#include <iostream>

bool hasPrefix(const std::string &name, const std::string &prefix) {
  if ( name.size() < prefix.size() ) return false;

  return std::mismatch(prefix.begin(), prefix.end(), name.begin()).first ==
    prefix.end();
}

std::string stripPrefix(const std::string &name, const std::string &prefix) {
  return std::string(name.begin() + prefix.size(), name.end());
}

class RequiredAfterOptional : public NncCompileError, public std::exception {
public:
  RequiredAfterOptional(const InsnArgDecl &required, const InsnArgDecl &optional)
    : NncCompileError(optional.errorContext()),
      m_requiredLocation(required.errorContext().location())
    , m_requiredName(required.name()) {
  }
  virtual ~RequiredAfterOptional() {
  }

  virtual const char *what() const noexcept { return "required after optional"; }

protected:
  void printOrigError(NncErrorReporter &reporter) {
    std::stringstream s;
    s << "The argument " << m_requiredName << " is required, but it occurs after the optional argument";
    reporter.note(m_requiredLocation, s.str(), true);

    s.str("");
    s << "This is the location of the first optional argument";
    reporter.note(m_context.location(), s.str(), true);
  }

private:
  nnc::location m_requiredLocation;
  std::string m_requiredName;
};

class ConstructorDeclArityMismatch : public NncCompileError, public std::exception {
public:
  ConstructorDeclArityMismatch(const ConstructorDecl &con,
                               std::size_t reqArity, std::size_t maxArity)
    : m_conLocation(con.errorContext().location()),
      m_conArity(con.arity()), m_reqArity(reqArity), m_maxArity(maxArity)
  {
  }
  virtual ~ConstructorDeclArityMismatch() { }

  virtual const char *what() const noexcept { return "constructor decl arity mismatch"; }

protected:
  void printOrigError(NncErrorReporter &reporter) {
    reporter.note(m_conLocation, "The constructor declared here", true);
    std::stringstream e;
    e << "Has " << m_conArity << " arguments, but must have between " << m_reqArity << " and " << m_maxArity << " arguments";
    reporter.note(m_conLocation, e.str());
  }

private:
  nnc::location m_conLocation;
  std::size_t m_conArity, m_reqArity, m_maxArity;
};

InsnModifier::~InsnModifier() {
}

InsnArgDecl::InsnArgDecl(const std::string &nm)
  : m_context(NncErrorContextStack::current()),
    m_name(nm), m_optional(false) {
  m_customType = std::unique_ptr<InsnVarType>(new InsnRegisterVarType());
}

InsnBase::InsnBase()
  : m_context(NncErrorContextStack::current()),
    m_cost(1000) {
}

InsnArgDecl &InsnBase::arg(const std::string &nm) {
  decltype(m_args)::iterator i(std::find_if(m_args.begin(), m_args.end(), [&](auto &x) { return x.name() == nm; }));
  if ( i == m_args.end() )
    throw NncGenerationError(m_context, "Argument " + nm + "not found");

  return *i;
}

std::ostream &InsnBase::declareConstructor(const std::string &nm, std::ostream &out, bool header) const {
  out << nm << "(::nnc::compile::RtlFunction &_fn";

  for ( const InsnArgDecl &decl : m_args ) {
    out << ", const ";
    decl.type().outputCType(out) << " &" << cName(decl.name());

    if ( decl.optional() && header ) {
      if ( decl.type().isDefaultable() ) {
        out << " = ";
        decl.type().outputArgDefault(out);
      } else {
        throw NncGenerationError(decl.type().errorContext(), "Variable " + decl.name() + " marked optional, but there is no default type declared");
      }
    }
  }

  out << ")";
  return out;
}

Insn::Insn(const std::string &nm, InsnBase &&o)
  : InsnBase(std::move(o)), m_name(nm) {
}

InsnFactory::InsnFactory() {
}

void InsnFactory::addConstructor(ConstructorDecl &&decl) {
  m_constructors.emplace_front(std::move(decl));
}

void InsnFactory::addIntersection(IntersectDecl &&decl) {
  m_intersects.emplace_front(std::move(decl));
}

InsnArgDecl &InsnFactory::addArgument(const std::string &nm) {
  return m_args.emplace_back(nm);
}

InsnArgDecl &InsnFactory::addArgument(InsnArgDecl &&d) {
  return  m_args.emplace_back(std::move(d));
}

bool InsnFactory::hasArgument(const std::string &nm) const {
   auto found(std::find_if(m_args.begin(), m_args.end(), [&nm](const InsnArgDecl &d) { return d.name() == nm; }));
   return found != m_args.end();
}

InsnArgDecl &InsnFactory::argument(const std::string &nm) {
  auto found(std::find_if(m_args.begin(), m_args.end(), [&nm](const InsnArgDecl &d) { return d.name() == nm; }));
  if ( found == m_args.end() ) {
    std::stringstream e;
    e << "Variable " << nm << " is not part of the instruction declaration";
    throw NncParseError(e.str());
  } else {
    return *found;
  }
}

void InsnFactory::param(const std::string &s, const Literal &l) {
  if ( s == "emit" ) {
    template_string t;
    SetTemplateStringLiteral set(t, WarnMissing("emit", "string"));
    l.visit(set);
    m_emit = std::move(t);
  } else if ( s == "cost" ) {
    SetNumberLiteral set(m_cost, WarnMissing("cost", "number"));
    l.visit(set);
  } else
    std::cerr << "Parameter " << s << " not valid for insn" << std::endl;
}

Insn *InsnFactory::build(const std::string &name) {
  // Check that the optional arguments come after the mandatory ones
  auto firstOptional(std::find_if(m_args.begin(), m_args.end(),
                                  [](const auto &argDecl) { return argDecl.optional(); }));

  // Check that all options after the first optional is optional
  auto requiredAfterOptional(std::find_if(firstOptional, m_args.end(),
                                          [](const auto &argDecl) { return !argDecl.optional(); }));
  if ( requiredAfterOptional != m_args.end() ) {
    throw RequiredAfterOptional(*requiredAfterOptional, *firstOptional);
  }

  std::size_t reqArity(std::distance(m_args.begin(), firstOptional));
  std::size_t maxArity(m_args.size());

  // Check that all constructors have at least as many required arguments, but no more than the total number
  auto it(std::find_if(m_constructors.begin(), m_constructors.end(),
                       [reqArity, maxArity](const auto &con) {
                         return ( con.arity() > maxArity || con.arity() < reqArity );
                       }));

  if ( it != m_constructors.end() )
    throw ConstructorDeclArityMismatch(*it, reqArity, maxArity);

  return new Insn(name, std::move(*this));
}

InsnArgsModifier::InsnArgsModifier() {
}

InsnArgsModifier::~InsnArgsModifier() {
}

InsnArgDecl &InsnArgsModifier::addArgument(const std::string &nm) {
  return m_args.emplace_front(nm);
}

void InsnArgsModifier::modify(InsnFactory &f) {
  args_type old_args;
  args_type &args(f.arguments());
  std::swap(old_args, args);

  // For every argument we have, we look it up in old_args, to re-use as a template.
  // Otherwise, if it's not there, we issue an error

  for (InsnArgDecl &d: m_args) {
    auto it(std::find_if(old_args.begin(), old_args.end(), [&d](const auto &arg) { return arg.name() == d.name(); }));
    if ( it == old_args.end() ) {

      // This declaration can just be moved
      f.addArgument(std::move(d));
    } else {
      auto &newDecl(f.addArgument(std::move(*it)));

      // We may have it marked optional here
      if ( d.optional() )
        newDecl.makeOptional();

      newDecl.direction(d.direction());
    }
  }
}

InsnRtlType::InsnRtlType()
  : m_ptrs(0), m_type(Unknown), m_width(-1) {
}

InsnRtlType::InsnRtlType(const std::string &s) {
  std::stringstream width;
  width << s.substr(1);
  width >> m_width;
  if ( width.bad() )
    throw NncParseError("Invalid width: " + width.str());

  switch ( s[0] ) {
  case 'i':
  case 's':
    m_type = SignedInteger;
    break;

  case 'f':
    m_type = FloatingPoint;
    break;

  case 'u':
    m_type = UnsignedInteger;
    break;

  default: {
    std::stringstream err;
    err << "Type code " << s[0] << " is not valid";
    throw NncParseError(err.str());
  }
  }
}

void InsnRtlType::outputTypeBuilder(std::ostream &out) const {
  switch ( m_type ) {
  case UnsignedInteger:
    out << ".intType(::nnc::compile::RtlUnsigned, " << m_width << ")";
    break;

  case SignedInteger:
    out << ".intType(::nnc::compile::RtlTwosComplement, " << m_width << ")";
    break;

  case FloatingPoint:
    out << ".floatType(" << m_width << ")";
    break;
  }

  std::fill_n(std::ostream_iterator<const char*>(out), m_ptrs,
              "->pointer()");
}

InsnRtlType InsnRtlType::ptr() const {
  InsnRtlType ret(*this);
  ret.m_ptrs++;
  return ret;
}

InsnVarType::InsnVarType()
  : m_context(NncErrorContextStack::current()) {
}

InsnVarType::~InsnVarType() {
}

bool InsnVarType::hasRegClass() const {
  return false;
}

void InsnVarType::setters(SettersVisitor &v) const {
}

void InsnVarType::outputIntersect(const std::string &declNm,
                                  const std::string &aNm,
                                  const std::string &bNm,
                                  const std::string &regclassNm,
                                  std::ostream &out) const {
  out << declNm << ".intersect(" << aNm << ", " << bNm << ", regclass::" << regclassNm << ");" << std::endl;
}

bool InsnVarType::canBeImpure() const { return false; }
void InsnVarType::outputIsPure(const std::string &memName, std::ostream &out) const {
  out << "true";
}

std::ostream &InsnVarType::outputRegClasses(const std::string &visitorNm,
                                            const std::string &argNm,
                                            const std::string &memName,
                                            std::ostream &out) const {
  return out;
}

std::ostream &InsnVarType::outputVisitOperand(const std::string &visitorNm,
                                              const std::string &argName,
                                              const std::string &memName,
                                              InsnArgDecl::Direction dir,
                                              std::ostream &out) const {
  out << visitorNm << ".operand(\"" << argName << "\", " << memName;
  switch ( dir ) {
  case InsnArgDecl::Output:
    out << ", false, true";
        break;

  case InsnArgDecl::InputOutput:
    out << ", true, true";
    break;

  case InsnArgDecl::Input:
  default:
    break;
  }
  out << ");" << std::endl;
  return out;
}

InsnRegisterVarType::InsnRegisterVarType() {
}

InsnRegisterVarType::~InsnRegisterVarType() {
}


std::ostream &InsnRegisterVarType::outputRegClasses(const std::string &visitorNm,
                                                    const std::string &argNm,
                                                    const std::string &memName,
                                                    std::ostream &out) const {
  if ( hasRegClass() ) {
    out << visitorNm << ".regclass(\"" << argNm << "\", " << memName << ", regclass::" <<
      cName(m_regClassName) << ");" << std::endl;
  }
  return out;
}

bool InsnRegisterVarType::hasRegClass() const {
  return !m_regClassName.empty();
}

bool InsnRegisterVarType::isDefaultable() const {
  return m_rtlType.has_value();
}

std::ostream &InsnRegisterVarType::outputCType(std::ostream &out) const {
  out << "std::shared_ptr<::nnc::compile::RtlVariable>";
  return out;
}

std::ostream &InsnRegisterVarType::outputArgDefault(std::ostream &out) const {
  out << "nullptr";
  return out;
}

void InsnRegisterVarType::outputArgMakeDefault(const std::string &fnName,
                                               const std::string &memName,
                                               const std::string &varName,
                                               IndentedHeaderBase &out) const {
  if ( m_rtlType ) {
    out.indent() << "if ( !" << varName << ") {" << std::endl;
    out.indent(1) << memName << " = "
                  << fnName << ".variable(\"" << varName << "\", " << fnName << ".types()";
    m_rtlType->outputTypeBuilder(out.noindent());
    out.noindent() << ");" << std::endl;
    out.indent() << "}" << std::endl;
  }
}

InsnConstantVarType::InsnConstantVarType() {
}

InsnConstantVarType::~InsnConstantVarType() {
}

bool InsnConstantVarType::isDefaultable() const {
  return m_rtlType.has_value();
}

std::ostream &InsnConstantVarType::outputCType(std::ostream &out) const {
  out << "std::span<std::uint8_t>";
  return out;
}

std::ostream &InsnConstantVarType::outputArgDefault(std::ostream &out) const {
  out << "std::span<std::uint8_t>()";
  return out;
}

void InsnConstantVarType::outputArgMakeDefault(const std::string &fnName, const std::string &memName,
                                               const std::string &varName,  IndentedHeaderBase &out) const {
  if ( m_rtlType ) {
    out.indent() << "if ( " << varName << ".empty() ) {" << std::endl;
    out.indent(1) << memName << " = "
        << fnName << ".variable(\"" << varName << "\", " << fnName << ".types()";
    m_rtlType->outputTypeBuilder(out.noindent());
    out.noindent() << ");";
    out.indent() << "}";
  }
}

InsnVarDeclBase::InsnVarDeclBase()
  : m_context(NncErrorContextStack::current()) {
}

InsnVarDeclBase::InsnVarDeclBase(InsnVarDeclBase &&o)
  : m_context(NncErrorContextStack::current()) {
}

InsnVarDeclFactory::InsnVarDeclFactory() {
}

void InsnVarDeclFactory::param(const std::string &name, const Literal &l) {
  std::cerr << "Parameter " << name << " invalid for insn var decl" << std::endl;
}

InsnVarDecl *InsnVarDeclFactory::build(const std::string &varNm, InsnVarType *ty) {
  return new InsnVarDecl(varNm, ty, std::move(*this));
}

InsnVarDecl::InsnVarDecl(const std::string &nm,
                         InsnVarType *ty,
                         InsnVarDeclBase &&o)
  : InsnVarDeclBase(std::move(o)),
    m_name(nm), m_type(ty) {
}

void InsnVarDecl::modify(InsnFactory &f) {
  if ( !f.hasArgument(m_name) ) {
    f.addArgument(m_name);
  }

  f.argument(m_name).setCustomType(std::move(m_type));
}

InsnVarDeclModifier::~InsnVarDeclModifier() {
}

CTypeModifier::~CTypeModifier() {
}

CType::CType()
  : m_makeDefault("if ( !$var ) $var = nullptr;")
  , m_visitOperand("$visitor.operand(\"$name\", $member, $isInput, $isOutput);")
  , m_visitRegclasses("$member.regclasses(\"$name\", $visitor, $default);")
  , m_intersect("$a.intersect($visitor, $b, $regclass);")
  , m_acceptsRtlType(false) {
}

CType::~CType() {
}

bool CType::hasRegClass() const {
  return true;
}

void CType::setRtlType(const InsnRtlType &ty) {
  m_rtlType = ty;
};

void CType::param(const std::string &name, const Literal &l) {
  if ( name == "name" ) {
    SetTemplateStringLiteral set(m_name, WarnMissing("name", "string"));
    l.visit(set);
  } else if ( name == "makeDefault" ) {
    SetTemplateStringLiteral set(m_makeDefault, WarnMissing("makeDefault", "string"));
    l.visit(set);
  } else if ( name == "default" ) {
    template_string d;
    SetTemplateStringLiteral set(d, WarnMissing("default", "string"));
    l.visit(set);
    m_default = d;
  } else if ( name == "visitOperands" ) {
    SetTemplateStringLiteral set(m_visitOperand, WarnMissing("visitOperands", "string"));
    l.visit(set);
  } else if ( name == "visitRegclasses" ) {
    SetTemplateStringLiteral set(m_visitRegclasses, WarnMissing("visitRegclasses", "string"));
    l.visit(set);
  } else if ( name == "intersection" ) {
    SetTemplateStringLiteral set(m_intersect, WarnMissing("intersection", "string"));
    l.visit(set);
  } else if ( name == "checkPure" ) {
    template_string d;
    SetTemplateStringLiteral set(d, WarnMissing("checkPure", "string"));
    l.visit(set);
    m_canBeImpure = d;
  } else if ( name == "acceptsRtlType" ) {
    std::uint32_t v(~0);
    SetNumberLiteral set(v, WarnMissing("acceptsRtlType", "number"));
    l.visit(set);
    if ( v != ~0 ) {
      m_acceptsRtlType = !!v;
    }
  } else if ( hasPrefix(name, "setter.") ) {
    auto setterFor(stripPrefix(name, "setter."));
    template_string d;
    SetTemplateStringLiteral set(d, WarnMissing(name, "string"));
    l.visit(set);
    m_setters[setterFor] = d;
  } else
    std::cerr << "Parameter " << name << " for Ctype is invalid" << std::endl;
}

bool CType::isDefaultable() const {
  return m_default.has_value();
}

void CType::setters(SettersVisitor &v) const {
  for ( const auto &t: m_setters )
    v.setter(t.first, t.second);
}

bool CType::canBeImpure() const {
  return m_canBeImpure.has_value();
}

void CType::outputIsPure(const std::string &memName, std::ostream &out) const {
  std::map<std::string, std::string> vars { { "member", memName } };
  out << m_canBeImpure->render(vars);
}

void CType::outputIntersect(const std::string &declNm,
                            const std::string &aNm,
                            const std::string &bNm,
                            const std::string &rcNm,
                            std::ostream &out) const {
  std::map<std::string, std::string> vars {
    { "visitor", declNm },
    { "a", aNm },
    { "b", bNm },
    { "regclass",  "regclass::" + rcNm }
  };
  out << m_intersect.render(vars) << std::endl;
}

std::ostream &CType::outputRegClasses(const std::string &visitorNm,
                                      const std::string &argNm,
                                      const std::string &memName,
                                      std::ostream &out) const {
  std::map<std::string, std::string> vars {
    { "visitor", visitorNm },
    { "name", argNm },
    { "member", memName },
    { "default", "regclass::" + cName(m_regClassName) }
  };
  out << m_visitRegclasses.render(vars) << std::endl;
  return out;
}

std::ostream &CType::outputCType(std::ostream &out) const {
  std::map<std::string, std::string> vars;
  out << m_name.render(vars);
  return out;
}

std::ostream &CType::outputArgDefault(std::ostream &out) const {
  if ( m_default.has_value() ) {
    std::map<std::string, std::string> vars;
    out << m_default->render(vars);
  }
  return out;
}


std::ostream &CType::outputVisitOperand(const std::string &visitorNm,
                                        const std::string &argName,
                                        const std::string &memName,
                                        InsnArgDecl::Direction dir,
                                        std::ostream &out) const {
  std::map<std::string, std::string> vars {
     { "visitor", visitorNm },
     { "name", argName },
     { "member", memName },
     { "isInput", "false" },
     { "isOutput", "false" }
  };
  if ( dir == InsnArgDecl::Input ||
       dir == InsnArgDecl::InputOutput )
    vars["isInput"] = "true";

  if ( dir == InsnArgDecl::Output ||
       dir == InsnArgDecl::InputOutput )
    vars["isOutput"] = "true";

  out << m_visitOperand.render(vars) << std::endl;
  return out;
}

void CType::outputArgMakeDefault(const std::string &fnName, const std::string &memName,
                                 const std::string &varName, IndentedHeaderBase &out) const {
  std::map<std::string, std::string> vars {
    { "function", fnName },
    { "var", varName },
    { "member", memName },
    { "name", "\"" + varName + "\"" },
  };

  if ( m_rtlType ) {
    std::stringstream s;
    s << fnName << ".types()";
    m_rtlType->outputTypeBuilder(s);
    vars.insert(std::make_pair("rtlType", s.str()));
  }

  m_makeDefault.render(vars).emit_by_lines(out);
}

ConstructorDecl::ConstructorDecl()
  : m_context(NncErrorContextStack::current()) {
}

void ConstructorDecl::addType(InsnVarType *ty) {
  m_args.emplace_front(ty);
}

void ConstructorDecl::modify(InsnFactory &f) {
  f.addConstructor(std::move(*this));
}

IntersectDecl::IntersectDecl(const std::string &a, const std::string &b,
                             const std::string &regclass)
  : m_a(a), m_b(b), m_regclass(regclass) {
}

void IntersectDecl::modify(InsnFactory &f) {
  f.addIntersection(std::move(*this));
}
