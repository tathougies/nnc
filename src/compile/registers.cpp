#include "compile/registers.hpp"

#include <iostream>
namespace nnc::exception {
  RegisterDoesNotExist::RegisterDoesNotExist(const ::nnc::compile::RegisterFile *file, const std::string &name)
    : m_regName(name) {
  }

  RegisterDoesNotExist::~RegisterDoesNotExist() {
  }

  const char *RegisterDoesNotExist::what() const noexcept {
    return m_regName.c_str();
  }

  RegisterIndexOutOfRange::RegisterIndexOutOfRange(int ix, int max)
    : m_ix(ix), m_max(max) {
  }

  RegisterIndexOutOfRange::~RegisterIndexOutOfRange() {
  }

  const char *RegisterIndexOutOfRange::what() const noexcept {
    return "reg index out of range";
  }
}

namespace nnc::compile {
  RegisterFile::~RegisterFile() {
  }

  Register::Register(const RegisterFile *file, int regno)
    : m_regs(file), m_regno(regno) {
  }

  bool Register::belongsToClass(int cls) const {
    return m_regs->registerInClass(m_regno, cls);
  }

  std::set<int> Register::classes() const {
    std::set<int> ret;

    for ( int i(0); i < m_regs->regclassCount(); ++i )
      if ( belongsToClass(i) ) ret.insert(i);

    return ret;
  }

  const char *Register::name() const {
    return m_regs->registerName(number());
  }

  VirtualRegister::VirtualRegister(const Register &reg)
    : m_size(-1), m_reg(reg) {
  }

  VirtualRegister::VirtualRegister(int spillSlot, int size)
    : m_size(size), m_spillSlot(spillSlot) {
  }

  VirtualRegister::VirtualRegister(const VirtualRegister &r)
    : m_size(r.m_size) {
    if ( m_size > 0 )
      m_spillSlot = r.m_spillSlot;
    else
      m_reg = r.m_reg;
  }

  std::ostream &operator<<(std::ostream &out, const ::nnc::compile::Register &v) {
    out << v.registers().registerName(v.number());
    return out;
  }

  std::ostream &operator<<(std::ostream &out, const ::nnc::compile::VirtualRegister &v) {
    if ( v.isSpill() ) {
      out << "spill[" << v.spillSlot() << "," << v.size() << "]";
    } else {
      out << v.reg();
    }
    return out;
  }

  Register chooseRegister(const RtlRegisterMapper &regs,
                          const RegClass &cls,
                          const RtlVariablePtr &b) {
    std::vector<VirtualRegister> vregs(regs.lookupVar(b));
    auto it(std::find_if(vregs.begin(), vregs.end(),
                         [&cls](const VirtualRegister &v) {
                           return v.isRegister() && v.reg().belongsToClass(cls);
                         }));

    if ( it == vregs.end() ) {
      throw exception::VariableNotInClass(b, cls);
    }

    return it->reg();
  }
}
