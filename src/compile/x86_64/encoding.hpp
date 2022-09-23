#ifndef __nnc_compile_x86_64_encoding_HPP__
#define __nnc_compile_x86_64_encoding_HPP__

#include <array>
#include <optional>
#include <iostream>

#include "compile/x86_64/types.hpp"
#include "compile/registers.hpp"
#include "compile/encoding.hpp"
#include "compile/rtl.hpp"

namespace nnc::arch::x86_64 {
  class opcode {
  public:
    opcode();
    opcode(std::uint8_t op1);
    opcode(std::uint8_t op1, std::uint8_t op2);
    opcode(std::uint8_t op1, std::uint8_t op2, std::uint8_t op3);
    opcode(const opcode &o) =default;

    void code(std::uint8_t op1);
    void code(std::uint8_t op1, std::uint8_t op2);
    void code(std::uint8_t op1, std::uint8_t op2, std::uint8_t op3);

    std::ostream &dump(std::ostream &o) const;

    void clear();

    inline std::size_t opcodesz() const { return m_opcodesz; }

    inline bool has_modrm() const { return m_flags & k_has_modrm; }
    inline bool has_vex() const { return m_flags & k_has_vex; }
    inline bool has_vex2() const { return m_flags & k_has_vex2; }

    bool has_rex() const;
    bool has_sib() const;
    bool has_disp() const;
    bool has_modrm_rmreg() const;
    std::size_t dispsz() const;

    enum Mod {
      Mod00 = 0x00,
      Mod01 = 0x01,
      Mod10 = 0x02,
      Mod11 = 0x03
    };

    enum Rm {
      Rm000 = 0x00,
      Rm001 = 0x01,
      Rm010 = 0x02,
      Rm011 = 0x03,
      Rm100 = 0x04,
      Rm101 = 0x05,
      Rm110 = 0x06,
      Rm111 = 0x07
    };

    enum Reg {
      Reg000 = 0x00,
      Reg001 = 0x01,
      Reg010 = 0x02,
      Reg011 = 0x03,
      Reg100 = 0x04,
      Reg101 = 0x05,
      Reg110 = 0x06,
      Reg111 = 0x07
    };

    enum Scale : std::uint8_t { Scale0 = 0x0, Scale2 = 0x1, Scale4 = 0x2, Scale8 = 0x3 };
    enum Index : std::uint8_t {
      IndexRax = 0x0,
      IndexRcx = 0x1,
      IndexRdx = 0x2,
      IndexRbx = 0x3,
      IndexNone = 0x4,
      IndexRbp = 0x5,
      IndexRsi = 0x6,
      IndexRdi = 0x7
    };
    enum Base : std::uint8_t {
      BaseRax = 0x0,
      BaseRcx = 0x1,
      BaseRdx = 0x2,
      BaseRbx = 0x3,
      BaseRsp = 0x4,
      BaseNone = 0x5,
      BaseRsi = 0x6,
      BaseRdi = 0x7
    };

    enum SimdPrefix : std::uint8_t {
      simd_packed_single = 0,
      simd_packed_double = 0x66,
      simd_scalar_single = 0xF3,
      simd_scalar_double = 0xF2
    };

    enum modrm_extension {
      modrm_wbit,
      modrm_rbit,
      modrm_xbit,
      modrm_bbit
    };

    inline opcode &simd_prefix(SimdPrefix p) { m_simd_prefix = p; return *this; }
    inline SimdPrefix simd_prefix() const { return m_simd_prefix; }
    inline bool has_simd_prefix() const { return m_simd_prefix != simd_packed_single; }

    opcode &modrm(const compile::RtlRegisterMapper &regs,
                  const compile::RegClass &cls,
                  const types::modrm &a);
    opcode &modrm(const compile::RtlRegisterMapper &regs,
                  const compile::RegClass &cls,
                  const compile::RtlVariablePtr &b);
    opcode &modrm(std::uint8_t mod, std::uint8_t rm);

    opcode &sib(std::uint8_t ss, std::uint8_t index, std::uint8_t base);

    opcode &modrm_reg(const compile::RtlRegisterMapper &regs,
                      const compile::RegClass &cls,
                      const compile::RtlVariablePtr &b);
    opcode &modrm_reg(std::uint8_t reg);
    opcode &modrm_reg(const compile::Register &r);
    inline opcode &modrm_reg(Reg r) { return modrm_reg((std::uint8_t) r); }
    Reg get_modrm_reg(bool &rbit) const;
    Reg get_modrm_rmreg(bool &rbit) const;

    inline opcode &avx_reg(Reg r) { return avx_reg((std::uint8_t) r); }
    opcode &avx_reg(std::uint8_t reg);
    opcode &avx_reg(const compile::Register &r);
    opcode &avx_reg(const compile::RtlRegisterMapper &regs,
                      const compile::RegClass &cls,
                      const compile::RtlVariablePtr &b);

    opcode &opreg(const compile::RtlRegisterMapper &regs,
                  const compile::RegClass &cls,
                  const compile::RtlVariablePtr &b);

    // Set the W bit of the rex prefix
    opcode &rexw(bool v = true);
    opcode &rexr(bool v = true);
    opcode &rexx(bool v = true);
    opcode &rexb(bool v = true);
    opcode &rexbit(modrm_extension bit, bool v = true);
    bool get_rexw() const;
    bool get_rexr() const;
    bool get_rexx() const;
    bool get_rexb() const;
    opcode &rex();

    opcode &rexvexr(bool v = true);
    opcode &rexvexx(bool v = true);
    opcode &rexvexb(bool v = true);
    opcode &rexvexw(bool v = true);
    opcode &rexvexbit(modrm_extension bit, bool v = true);

    opcode &vexr(bool v = true);
    opcode &vexx(bool v = true);
    opcode &vexb(bool v = true);
    opcode &vexw(bool v = true);
    opcode &vexbit(modrm_extension bit, bool v = true);
    bool get_vexw() const;
    bool get_vexr() const;
    bool get_vexx() const;
    bool get_vexb() const;
    opcode &vex();
    opcode &vex2();

    opcode &immediate(const types::imm &i, std::size_t sz = 64);
    inline std::size_t immsz() const { return m_immsz; }
    inline bool has_immediate() const { return immsz() > 0; }

    inline Mod mod() const {
      return (Mod) (((m_modrm & 0xC0) >> 6) & 0x3);
    }

    inline Rm rm() const {
      return (Rm) (m_modrm & 0x7);
    }

  private:
    Reg translate_reg(const compile::Register &r, bool &rbit);

    std::uint8_t m_opcode[3];
    std::size_t m_opcodesz;

    std::uint8_t m_rex;
    std::uint8_t m_vex[2];

    std::uint8_t m_modrm;
    std::uint8_t m_sib;
    std::uint32_t m_disp;

    std::uint8_t m_imm[8];
    std::size_t m_immsz;

    SimdPrefix m_simd_prefix;

    static constexpr std::uint32_t k_has_modrm = 0x00000001;
    static constexpr std::uint32_t k_has_vex = 0x00000002;
    static constexpr std::uint32_t k_has_vex2 = 0x00000004;

    std::uint32_t m_flags;
  };

  class opcode_builder {
  public:
    opcode_builder(const compile::RtlRegisterMapper &inputs,
                   const compile::RtlRegisterMapper &outputs,
                   compile::BytecodeEmitter &out);

    void c_xor(const types::modrm &dest, const types::modrm &a, const types::modrmimm &b);
    void test(const compile::RtlVariablePtr &a, const compile::RtlVariablePtr &b,
              const compile::RtlVariablePtr &sign, const compile::RtlVariablePtr &zero,
              const compile::RtlVariablePtr &parity);
    void cmp(const types::modrm &a, const types::modrmimm &b,
             const compile::RtlVariablePtr &sign, const compile::RtlVariablePtr &carry);
    void mov(const types::modrm &dst, const types::modrmimm &src);
    void movzx(const compile::RtlVariablePtr &dst,
               const types::modrm &src);
    void lea(const compile::RtlVariablePtr &dst, const types::modrm &src);
    void dec(const types::modrm &dst, const types::modrm &src);
    void inc(const types::modrm &dst, const types::modrm &src);
    void sub(const types::modrm &dst, const types::modrm &a, const types::modrmimm &b);
    void add(const types::modrm &dst, const types::modrm &a, const types::modrmimm &b);
    void imul(const types::modrm &dst, const types::modrm &a, const types::modrmimm &b);
    void addss(const compile::RtlVariablePtr &dst, const compile::RtlVariablePtr &a,
               const types::modrm &b);
    void mulss(const compile::RtlVariablePtr &dst, const compile::RtlVariablePtr &a,
               const types::modrm &b);
    void movss(const types::modrm &dst, const types::modrmimm &src);
    void cvtsi2ss(const compile::RtlVariablePtr &dst, const types::modrm &src);

  private:
    void emit(const opcode &e);

    std::uintptr_t allocate_constant(const types::imm &i) const;
    void fixup(std::unique_ptr<compile::BytecodeFixup> &&fixup);

    const compile::RtlRegisterMapper &m_inputs, &m_outputs;
    compile::BytecodeEmitter &m_out;
  };

  class jumps : public compile::InsnEncoder {
  public:
    virtual ~jumps();

    virtual std::unique_ptr<compile::JumpInsn> jump_relative(std::int64_t offs, std::optional<compile::Register> v) const;
    virtual std::unique_ptr<compile::JumpInsn> jump_absolute(std::int64_t abs, std::optional<compile::Register> v) const;

    virtual const compile::RegClass &indirect_jump_class() const;
    virtual std::unique_ptr<compile::JumpInsn> jump_indirect(compile::Register r, std::optional<compile::Register> v) const;

    virtual std::unique_ptr<compile::JumpInsn> generic_jump(std::optional<compile::Register> v) const;

    virtual void funret(compile::BytecodeEmitter &out) const override;
    virtual void preamble(compile::BytecodeEmitter &out) const;
    virtual void postamble(compile::BytecodeEmitter &out) const;

    virtual const compile::RegClass &jump_cond_class() const;

    virtual void copy(compile::BytecodeEmitter &o, const compile::RtlRegisterMapper &inputs, const compile::RtlVariablePtr &var, const compile::VirtualRegister &dest) const override;
  };
};

#endif
