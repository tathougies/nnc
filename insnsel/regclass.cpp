#include <iostream>
#include <sstream>

#include "ast.hpp"
#include "archdesc.hpp"
#include "error.hpp"

RegClassBase::RegClassBase()
  : m_count(1) {
}

RegClassBase::RegClassBase(const RegClassBase &base)
  : m_doc(base.m_doc),
    m_count(base.m_count),
    m_regNames(base.m_regNames) {
}

RegClass::RegClass(const std::string &clsNm, const RegClassBase &base)
  : RegClassBase(base),
    m_name(clsNm) {
}

void RegClass::ensureRegisters(ArchDescBuilder &b) const {
  for ( const auto &nm: m_regNames )
    b.getRegister(nm);
}

RegClassFactory::RegClassFactory() {
}

void RegClassFactory::param(const std::string &nm, const Literal &l) {
  if ( nm == "doc" ) {
    SetStringLiteral set(m_doc, WarnMissing("doc", "string"));
    l.visit(set);
  } else if ( nm == "count" ) {
    SetNumberLiteral set(m_count, WarnMissing("count", "number"));
    l.visit(set);
  } else
    std::cerr << "Warning: regclass parameter " << nm << " does not exist" << std::endl;
}

void RegClassFactory::addRegister(const std::string &nm) {
  m_regNames.insert(nm);
}

void RegClassFactory::addType(const RegType &ty) {
  m_regTypes.insert(ty);
}

RegClass *RegClassFactory::build(const std::string &clsNm) const {
  return new RegClass(clsNm, *this);
}

RegMemberDecl::RegMemberDecl() {
}

RegMemberDecl::~RegMemberDecl() {
}

void RegMemberDecl::modify(RegClassFactory &f) const {
  for ( const auto &name: m_names )
    f.addRegister(name);
}

void RegMemberDecl::addRegister(const std::string &nm) {
  m_names.push_back(nm);
}

RegClassTypeDecl::RegClassTypeDecl() {
}

RegClassTypeDecl::~RegClassTypeDecl() {
}

void RegClassTypeDecl::modify(RegClassFactory &f) const {
  for ( const auto & type: m_types )
    f.addType(type);
}

void RegClassTypeDecl::addType(const RegType &ty) {
  m_types.push_back(ty);
}

RegType::RegType()
  : m_format(RegFormatUnknown),
    m_width(0) {
}

RegType::RegType(RegFormat fmt, int width)
  : m_format(fmt), m_width(width) {
}

RegType RegType::fromString(const std::string &s) {
  std::stringstream ss(s);

  char fmtCode(ss.get());
  RegFormat fmt;

  switch ( fmtCode ) {
  case 'i':
    fmt = RegFormatInt;
    break;

  case 'f':
    fmt = RegFormatIEEE754;
    break;

  default:
    ss.str("");
    ss << "Char code '" << fmtCode << "' is not a valid type format code";
    throw NncParseError(ss.str());
  }

  int width;
  ss >> width;
  if ( ss.fail() )
    throw NncParseError(ss.str());

  return RegType(fmt, width);
}

bool RegType::operator==(const RegType &t) const {
  return m_format == t.m_format && m_width == t.m_width;
}

bool RegType::operator<(const RegType &t) const {
  if ( m_format == t.m_format ) {
    return m_width < t.m_width;
  } else return m_format < t.m_format;
}

RegClassModifier::~RegClassModifier() {
}
