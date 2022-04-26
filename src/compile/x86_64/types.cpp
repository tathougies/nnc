#include "compile/x86_64/types.hpp"

namespace nnc::arch::x86_64::types {
  modrm::modrm(std::shared_ptr<compile::RtlVariable> reg)
    : m_baseReg(reg)
    , m_displacement(0)
    , m_scalePow2(-1) {
  }

  modrm modrm::memref(std::shared_ptr<compile::RtlVariable> reg,
                      std::uint32_t disp, std::uint8_t scalePow2) {
    modrm ret(reg);
    ret.m_displacement = disp;
    ret.m_scalePow2 = std::clamp<std::uint8_t>(scalePow2, 0, 2);
    return ret;
  }

  modrm modrm::memref(std::shared_ptr<compile::RtlVariable> reg,
                      std::shared_ptr<compile::RtlVariable> index,
                      std::uint8_t scalePow2) {
    modrm ret(reg);
    ret.m_indexReg = index;
    ret.m_scalePow2 = std::clamp<std::uint8_t>(scalePow2, 0, 2);
    return ret;
  }

  void modrm::visit(const std::string &baseNm, compile::RtlOperandVisitor &v, bool input, bool output) const {
    if ( isMemoryRef() ) {
      v.operand(baseNm + ".ref", base());
      if ( m_indexReg ) {
        v.operand(baseNm + ".index", indexReg());
      } else if ( m_displacement ) {
        v.operand(baseNm + ".displacement", v.function().types().intType(::nnc::compile::RtlUnsigned, 32),
                  (void *) &m_displacement, 4);
      }

      if ( m_scalePow2 > 0 ) {
        v.operand(baseNm + ".scalePow2", v.function().types().intType(::nnc::compile::RtlUnsigned, 2),
                  (void *) &m_scalePow2, 1);
      }
    } else {
      v.operand(baseNm + ".reg", base(), input, output);
    }
  }

  modrm &modrm::operator=(const std::shared_ptr<compile::RtlVariable> &v) {
    modrm n(v);
    std::swap(*this, n);
    return *this;
  }

  imm::imm(std::uint64_t i)
    : m_value(i) {
  }

  imm::imm(const std::span<std::uint8_t> &i) {
    union {
      std::uint8_t d[8];
      std::uint64_t val;
    };
    std::remove_reference<decltype(i)>::type::iterator begin(i.begin()), end;
    if ( i.size() < 8 )
      end = i.end();
    else {
      end = begin;
      std::advance(end, 8);
    }

    std::copy(begin, end, d);
    m_value = val;
  }

  void imm::visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const {
    return v.operand(baseNm, v.function().types().intType(::nnc::compile::RtlUnsigned, 64), &m_value, 8);
  }

  modrmimm::modrmimm(const std::span<std::uint8_t> &i)
    : m_what(imm(i)) {
  }

  modrmimm::modrmimm(const modrm &m)
    : m_what(modrm(m)) {
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
}
