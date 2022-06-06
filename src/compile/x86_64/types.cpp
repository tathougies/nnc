#include "compile/x86_64/types.hpp"
#include "arch/x86_64/registers.hpp"

namespace nnc::arch::x86_64::types {
  regmem::regmem(std::shared_ptr<compile::RtlVariable> reg,
                 std::shared_ptr<compile::RtlType> ty)
    : m_var(reg), m_type(ty) {
    if ( !ty ) {
      m_type = m_var->type();
    }
  }

  regmem regmem::memref(std::shared_ptr<compile::RtlVariable> reg,
                        std::shared_ptr<compile::RtlType> ty) {
    regmem r(reg);
    r.m_isMemRef = true;
    r.m_type = ty;
    if ( !ty ) {
      compile::RtlTypeInfo info(*reg->type());
      if ( !info.isPointerType() )
        throw std::runtime_error("regmem::memref must be pointer type");

      r.m_type = info.underlying();
    }
    return r;
  }

  regmem regmem::reference(std::shared_ptr<compile::RtlType> ty) const {
    if ( isMemoryRef() ) return regmem(*this);
    else return memref(m_var, ty);
  }

  void regmem::visit(const std::string &baseNm, compile::RtlOperandVisitor &v,
                     bool input, bool output) const {
    if ( isMemoryRef() ) {
      v.operand(baseNm + ".ref", base());
    } else {
      v.operand(baseNm, base(), input, output);
    }
  }

  void regmem::regclasses(const std::string &baseNm, compile::RegClassDeclarer &d, compile::RegClass &bs) const {
    if ( isMemoryRef() ) {
      d.regclass(baseNm + ".ref", base(), regclass::gp);
    } else {
      d.regclass(baseNm, base(), bs);
    }
  }

  modrm::modrm(std::shared_ptr<compile::RtlVariable> reg,
               std::shared_ptr<compile::RtlType> ty)
    : m_baseReg(reg)
    , m_displacement(0)
    , m_scalePow2(-1)
    , m_type(ty) {
    if ( !ty )
      m_type = reg->type();
  }

  modrm modrm::memref(std::shared_ptr<compile::RtlVariable> reg,
                      std::shared_ptr<compile::RtlType> ty,
                      std::uint32_t disp, std::uint8_t scalePow2) {
    modrm ret(reg, ty);
    ret.m_displacement = disp;
    ret.m_scalePow2 = std::clamp<std::uint8_t>(scalePow2, 0, 2);
    return ret;
  }

  modrm modrm::memref(std::shared_ptr<compile::RtlVariable> reg,
                      std::shared_ptr<compile::RtlType> ty,
                      std::shared_ptr<compile::RtlVariable> index,
                      std::uint8_t scalePow2) {
    modrm ret(reg, ty);
    ret.m_indexReg = index;
    ret.m_scalePow2 = std::clamp<std::uint8_t>(scalePow2, 0, 2);
    return ret;
  }

  void modrm::regclasses(const std::string &baseNm, compile::RegClassDeclarer &d, compile::RegClass &bs) const {
    if ( isMemoryRef() ) {
      d.regclass(baseNm + ".ref", base(), ::nnc::arch::x86_64::regclass::gp);
      if ( m_indexReg ) {
        d.regclass(baseNm + ".index", indexReg(), ::nnc::arch::x86_64::regclass::gp);
      }
    } else {
      d.regclass(baseNm, base(), bs);
    }
  }

  void modrm::intersect(compile::RegClassDeclarer &d, const modrm &o, compile::RegClass &rc) const {
    if ( isMemoryRef() && o.isMemoryRef() ) {
      d.intersect(base(), o.base(), ::nnc::arch::x86_64::regclass::gp);
      if ( m_indexReg )
        d.intersect(indexReg(), o.indexReg(), ::nnc::arch::x86_64::regclass::gp);
    } else if ( isRegister() && o.isRegister() ) {
      d.intersect(base(), o.base(), rc);
    } else
      throw std::runtime_error("Cannot alias disjoint types");
  }

  void modrm::visit(const std::string &baseNm, compile::RtlOperandVisitor &v, bool input, bool output) const {
    if ( isMemoryRef() ) {
      v.operand(baseNm + ".ref", base());
      if ( m_indexReg ) {
        v.operand(baseNm + ".index", indexReg());
      }

      if ( m_displacement ) {
        v.operand(baseNm + ".displacement", v.function().types().intType(::nnc::compile::RtlUnsigned, 32),
                  (void *) &m_displacement, 4);
      }

      if ( m_scalePow2 > 0 ) {
        v.operand(baseNm + ".scalePow2", v.function().types().intType(::nnc::compile::RtlUnsigned, 2),
                  (void *) &m_scalePow2, 1);
      }
    } else {
      v.operand(baseNm, base(), input, output);
    }
  }

  bool modrm::operator==(const modrm &o) const {
    if ( o.base() != base() ) return false;

    if ( isMemoryRef() ) {
      if ( !o.isMemoryRef() ) return false;
      return m_indexReg == o.m_indexReg && m_displacement == o.m_displacement && m_scalePow2 == o.m_scalePow2;
    } else {
      if ( o.isMemoryRef() ) return false;
      return true;
    }
  }

  modrm &modrm::operator=(const std::shared_ptr<compile::RtlVariable> &v) {
    modrm n(v);
    std::swap(*this, n);
    return *this;
  }

  modrm modrm::asMemRef() const {
    if ( isMemoryRef() ) return *this;
    else {
      modrm next(*this);
      next.m_scalePow2 = 0;
      return next;
    }
  }

  modrm modrm::indexedBy(std::shared_ptr<compile::RtlVariable> ix) const {
    modrm next(asMemRef());
    next.m_indexReg = ix;
    return next;
  }

  modrm modrm::withBase(std::shared_ptr<compile::RtlVariable> base) const {
    modrm next(asMemRef());
    next.m_baseReg = base;
    return next;
  }

  bool modrm::isAReg(const compile::RtlRegisterMapper &a) const {
    return isReg(a, registers.lookupRegister("rax"));
  }

  bool modrm::isBReg(const compile::RtlRegisterMapper &a) const  {
    return isReg(a, registers.lookupRegister("rbx"));
  }

  bool modrm::isCReg(const compile::RtlRegisterMapper &a) const  {
    return isReg(a, registers.lookupRegister("rcx"));
  }

  bool modrm::isDReg(const compile::RtlRegisterMapper &a) const {
    return isReg(a, registers.lookupRegister("rdx"));
  }

  bool modrm::isReg(const compile::RtlRegisterMapper &a, const compile::Register &r) const {
    if ( !isRegister() ) return false;

    auto locs(a.lookupVar(base()));
    return std::find(locs.begin(), locs.end(), compile::VirtualRegister(r)) != locs.end();
  }

  imm::imm(std::uint64_t i, std::shared_ptr<compile::RtlType> ty)
    : m_value(i), m_type(ty) {
  }

  imm::imm(const std::span<std::uint8_t> &i, std::shared_ptr<compile::RtlType> ty)
    : m_type(ty) {
    union {
      std::uint8_t d[8];
      std::uint64_t val;
    };
    std::remove_reference<decltype(i)>::type::iterator begin(i.begin()), end;
    val = 0;
    if ( i.size() <= 8 ) {
      end = i.end();
    } else {
      end = begin;
      std::advance(end, 8);
    }

    std::copy(begin, end, d);
    m_value = val;
  }

  bool imm::operator== (const imm &o) const {
    return m_value == o.m_value;
  }

  void imm::visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const {
    return v.operand(baseNm, m_type, &m_value, m_type->size());
  }

  modrmimm::modrmimm(const std::span<std::uint8_t> &i, std::shared_ptr<compile::RtlType> ty)
    : m_what(imm(i, ty)) {
  }

  modrmimm::modrmimm(const modrm &m)
    : m_what(modrm(m)) {
  }

  modrmimm::modrmimm(std::shared_ptr<compile::RtlVariable> reg, std::shared_ptr<compile::RtlType> ty)
  : m_what(modrm(reg, ty)) {
  }

  void modrmimm::regclasses(const std::string &clsNm, compile::RegClassDeclarer &d, compile::RegClass &base) const {
    if ( !isImmediate() ) {
      reg().regclasses(clsNm, d, base);
    }
  }

  void modrmimm::visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const {
    if ( isImmediate() ) {
      immediate().visit(baseNm + ".imm", v);
    } else {
      reg().visit(baseNm, v);
    }
  }

  modrmimm &modrmimm::operator=(const std::shared_ptr<compile::RtlVariable> &v) {
    modrm n(v);
    return (*this = n);
  }

  modrmimm &modrmimm::operator=(const modrm &o) {
    m_what = o;
    return *this;
  }

  bool modrmimm::operator==(const modrmimm &o) const {
    return m_what == o.m_what;
  }
}
