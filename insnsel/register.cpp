#include "ast.hpp"
#include <iostream>

RegisterBase::RegisterBase() {
}

RegisterBase::RegisterBase(const RegisterBase &b)
  : m_clobbers(b.m_clobbers) {
}

RegisterFactory::RegisterFactory() {
}

void RegisterFactory::param(const std::string &doc, const Literal &l) {
  std::cerr << "Parameter " << doc << " not valid for register" << std::endl;
}

void RegisterFactory::addClobber(const std::string &name) {
  m_clobbers.insert(name);
}

/* Clobbers */

ClobberListDecl::~ClobberListDecl() {
}

void ClobberListDecl::addRegister(const std::string &nm) {
  m_registers.insert(nm);
}

void ClobberListDecl::modify(RegisterFactory &f) const {
  for ( const std::string &nm: m_registers )
    f.addClobber(nm);
}

/* CompositeRegisterModifier */

void CompositeRegisterModifier::modify(RegisterFactory &f) const {
  for ( auto *mod: m_mods )
    mod->modify(f);
}

void CompositeRegisterModifier::addModifier(RegisterModifier *m) {
  m_mods.push_back(m);
}
