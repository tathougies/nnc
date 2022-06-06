#include "ast.hpp"
#include "insnrule.hpp"
#include "insn.hpp"

LiteralVisitor::~LiteralVisitor() {
}

void LiteralVisitor::visit(const std::string &s) {
}

void LiteralVisitor::visit(std::uint32_t ui) {
}

NumberLiteral::NumberLiteral(std::uint32_t v)
  : m_value(v) {
}

NumberLiteral::~NumberLiteral() {
}

void NumberLiteral::visit(LiteralVisitor &v) const {
  v.visit(m_value);
}

StringLiteral::StringLiteral(const std::string &s)
  : m_value(s) {
}

StringLiteral::~StringLiteral() {
}

void StringLiteral::visit(LiteralVisitor &v) const {
  v.visit(m_value);
}

ParamDecl::ParamDecl(const std::string &name,
                     Literal *value)
  : m_name(name), m_value(value) {
}

ParamDecl::~ParamDecl() {
  delete m_value;
}

Literal &ParamDecl::value() const { return *m_value; }
const std::string &ParamDecl::name() const { return m_name; }

void ParamDecl::modify(RegClassFactory &f) const {
  f.param(m_name, *m_value);
}

void ParamDecl::modify(RegisterFactory &f) const {
  f.param(m_name, *m_value);
}

void ParamDecl::modify(InsnRuleFactory &f) {
  f.param(m_name, *m_value);
}

void ParamDecl::modify(InsnFactory &f) {
  f.param(m_name, *m_value);
}

void ParamDecl::modify(InsnVarDeclFactory &f) {
  f.param(m_name, *m_value);
}

void ParamDecl::modify(CType &f) {
  f.param(m_name, *m_value);
}

/* Literal visitor */

WarnMissing::WarnMissing(const std::string &name,
                         const std::string &type)
  : m_name(name), m_type(type) {
}

void WarnMissing::operator() () {
  std::cerr << "Warning: param " << m_name << " expected type " << m_type << std::endl;;
}

SetLiteral::SetLiteral(std::function<void()> onMissing)
  : m_created(false), m_onMissing(onMissing) { }

SetLiteral::~SetLiteral() {
  if ( !m_created )
    m_onMissing();
}

void SetLiteral::seen() { m_created = true; }

SetStringLiteral::SetStringLiteral(std::string &s, std::function<void()> onMissing)
  : SetLiteral(onMissing), m_target(s) {
}

SetStringLiteral::~SetStringLiteral() {
}

void SetStringLiteral::visit(const std::string &s) {
  seen();
  m_target = s;
}

SetTemplateStringLiteral::SetTemplateStringLiteral(template_string &s, std::function<void()> onMissing)
  : SetStringLiteral(m_internal, onMissing), m_s(s) {
}

SetTemplateStringLiteral::~SetTemplateStringLiteral() {
}

void SetTemplateStringLiteral::visit(const std::string &a) {
  SetStringLiteral::visit(a);
  m_s = template_string(m_internal);
}

SetNumberLiteral::SetNumberLiteral(std::uint32_t &u, std::function<void()> onMissing)
  : SetLiteral(onMissing), m_target(u) {
}

SetNumberLiteral::~SetNumberLiteral() {
}

void SetNumberLiteral::visit(std::uint32_t x) {
  seen();
  m_target = x;
}

RegMemberDeclarer::~RegMemberDeclarer() {
}

RangedRegMemberDeclarer::RangedRegMemberDeclarer() {
}

RangedRegMemberDeclarer::~RangedRegMemberDeclarer() {
}

void RangedRegMemberDeclarer::setName(const std::string &name) {
  m_name = name;
}

void RangedRegMemberDeclarer::addRange(int start, int end) {
  m_ranges.emplace_back(start, end);
}

void RangedRegMemberDeclarer::modify(RegMemberDecl &d) const {
  for ( const auto &range: m_ranges ) {
    for ( int i(range.first); i <= range.second; ++i ) {
      std::stringstream nm;
      nm << m_name << i;
      d.addRegister(nm.str());
    }
  }
}

SingleRegMemberDeclarer::SingleRegMemberDeclarer(const std::string &nm)
  : m_name(nm) {
}

SingleRegMemberDeclarer::~SingleRegMemberDeclarer() {
}

void SingleRegMemberDeclarer::modify(RegMemberDecl &d) const {
  d.addRegister(m_name);
}
