#include "compile/x86_64/encoding.hpp"
#include "arch/x86_64/registers.hpp"

#include <cassert>
#include <iomanip>
#include <limits>

namespace nnc::arch::x86_64 {
  using namespace nnc::compile;
  using namespace nnc::exception;

  class ArchTypeInfo : public RtlTypeInfo {
  public:
    ArchTypeInfo(const RtlType &t, bool unwrapPointer = true)
      : RtlTypeInfo(t) {
      if ( unwrapPointer && isPointerType() ) {
        m_cls = IntClass;
        m_width = 64;
        m_underlying.reset();
        m_signedness = RtlUnsigned;
      }
    }
  };

  class opcode_overflow : public std::exception {
  public:
    virtual ~opcode_overflow() { }

    virtual const char *what() const noexcept { return "opcode overflow"; }
  };

  class Constant32Fixup : public BytecodeFixup {
  public:
    Constant32Fixup(std::uintptr_t const_offset) : m_offset(const_offset) {}
    virtual ~Constant32Fixup() {}

    virtual std::size_t size() const override { return 4; }

    virtual void fixup(BytecodeInfo &info, std::uint8_t *buf, std::size_t sz) override {
      std::uintptr_t base(info.section_start(BytecodeInfo::DataSection));
      union {
        std::uint32_t imm;
        std::uint8_t d[4];
      };
      imm = base + m_offset;
      std::copy(std::begin(d), std::end(d), buf);
    }

  private:
    std::uintptr_t m_offset;
  };

    opcode::opcode() { clear(); }

    opcode::opcode(std::uint8_t u) {
      clear();
      code(u);
  }

  opcode::opcode(std::uint8_t u, std::uint8_t v) {
    clear();
    code(u, v);
  }

  opcode::opcode(std::uint8_t u, std::uint8_t v, std::uint8_t w) {
    clear();
    code(u, v, w);
  }

  void opcode::code(std::uint8_t u) {
    m_opcodesz = 1;
    m_opcode[0] = u;
  }

  void opcode::code(std::uint8_t u, std::uint8_t v) {
    m_opcodesz = 2;
    m_opcode[0] = u;
    m_opcode[1] = v;
  }

  void opcode::code(std::uint8_t u, std::uint8_t v, std::uint8_t w) {
    m_opcodesz = 3;
    m_opcode[0] = u;
    m_opcode[1] = v;
    m_opcode[2] = w;
  }

  opcode &opcode::immediate(const types::imm &i, std::size_t sz) {
    std::size_t bytesz((sz + 7) / 8);

    union {
      std::uint64_t v;
      std::uint8_t d[8];
    };

    v = i.value();
    m_immsz = bytesz;

    std::copy(d, d + bytesz, m_imm);

    return *this;
  }

  opcode &opcode::rexvexr(bool v) {
    if ( has_vex() )
      return vexr(v);
    else
      return rexr(v);
  }

  opcode &opcode::rexvexx(bool v) {
    if ( has_vex() )
      return vexx(v);
    else
      return rexx(v);
  }

  opcode &opcode::rexvexb(bool v) {
    if ( has_vex() )
      return vexb(v);
    else
      return rexb(v);
  }

  opcode &opcode::rexvexw(bool v) {
    if ( has_vex() )
      return vexw(v);
    else
      return rexw(v);
  }

  opcode &opcode::rexvexbit(modrm_extension bit, bool v) {
    switch (bit) {
    case modrm_rbit: return rexvexr(v);
    case modrm_xbit: return rexvexx(v);
    case modrm_wbit: return rexvexw(v);
    case modrm_bbit: return rexvexb(v);
    }
    return *this;
  }


  opcode &opcode::rexw(bool set) {
    rex();
    if ( set )
      m_rex |= 0x08;
    else
      m_rex &= ~0x08;
    return *this;
  }

  opcode &opcode::rexr(bool set) {
    rex();
    if ( set )
      m_rex |= 0x04;
    else
      m_rex &= ~0x04;
    return *this;
  }

  opcode &opcode::rexx(bool set) {
    rex();
    if ( set )
      m_rex |= 0x02;
    else
      m_rex &= ~0x02;
    return *this;
  }

  opcode &opcode::rexb(bool set) {
    rex();
    if ( set )
      m_rex |= 0x01;
    else
      m_rex &= ~0x01;
    return *this;
  }

  opcode &opcode::rexbit(modrm_extension bit, bool v) {
    switch (bit) {
    case modrm_rbit: return rexr(v);
    case modrm_xbit: return rexx(v);
    case modrm_wbit: return rexw(v);
    case modrm_bbit: return rexb(v);
    }
    return *this;
  }

  bool opcode::get_rexb() const {
    return has_rex() && (m_rex & 0x01);
  }

  bool opcode::get_rexx() const {
    return has_rex() && (m_rex & 0x02);
  }

  bool opcode::get_rexr() const {
    return has_rex() && (m_rex & 0x04);
  }

  bool opcode::get_rexw() const {
    return has_rex() && (m_rex & 0x08);
  }

  opcode &opcode::opreg(const compile::RtlRegisterMapper &regs,
                        const compile::RegClass &cls,
                        const compile::RtlVariablePtr &b) {
    compile::Register r(chooseRegister(regs, cls, b));
    bool bbit;
    Reg g(translate_reg(r, bbit));
    m_opcode[m_opcodesz - 1] &= 0xF8;
    m_opcode[m_opcodesz - 1] |= (std::uint8_t) g;

    if ( bbit ) rexvexb();
    return *this;
  }

  opcode &opcode::modrm(const compile::RtlRegisterMapper &regs,
                        const compile::RegClass &cls,
                        const types::modrm &a) {
    if ( a.isMemoryRef() ) {
      std::uint32_t disp(a.displacement());
      Mod m;
      if ( disp == 0 )
        m = Mod00;
      else if ( disp <= 0xFF )
        m = Mod01;
      else
        m = Mod10;

      if ( m == Mod01 || m == Mod10 )
        m_disp = a.displacement();

      bool bbit(false), xbit(false);
      compile::Register baseReg(chooseRegister(regs, regclass::baseregs, a.base()));
      Reg g(translate_reg(baseReg, bbit));

      if (g == Reg101 && m == Mod00) {
        // This is x86-64 RIP-relative addressing. We must force a displacement
        m = Mod01; // Adds an 8-bit displacement
        m_disp = 0;
      }

      if ( a.indexReg() ) {
        compile::Register indexReg(chooseRegister(regs, regclass::indexregs, a.indexReg()));

        std::uint8_t sib_ss(a.scalePow2() & 0x3);
        Reg sib_index(translate_reg(indexReg, xbit));

        if ( (sib_index == Reg101 || g == Reg101) && m == Mod00 ) {
          if ( m == Mod00 )
          m = Mod01;
          m_disp = 0;
        }

        m_sib = (sib_ss << 6) |
          ((((std::uint8_t) sib_index) & 0x7) << 3) |
          (((std::uint8_t) g) & 0x7);

        g = Reg100; // SIB Byte
      }

      if ( bbit ) rexvexb();
      if ( xbit ) rexvexx();

      return modrm(m, g);
    } else {
      return modrm(regs, cls, a.base());
    }
  }

  opcode &opcode::modrm(const compile::RtlRegisterMapper &regs,
                        const compile::RegClass &cls,
                        const compile::RtlVariablePtr &a) {
    bool bbit(false);
    compile::Register r(chooseRegister(regs, cls, a));
    Reg g(translate_reg(r, bbit));

    std::cerr << "MODRM SAYS BBIT " << bbit << std::endl;
    if ( bbit ) rexvexb();

    return modrm(Mod11, (Rm) g);
  }

  opcode &opcode::modrm(std::uint8_t mod, std::uint8_t rm) {
    m_flags |= k_has_modrm;
    m_modrm &= 0x38;
    m_modrm |= (mod & 0x3) << 6;
    m_modrm |= rm & 0x7;
    return *this;
  }

  opcode &opcode::sib(std::uint8_t scale, std::uint8_t index,
                      std::uint8_t base) {
    m_sib = ((scale & 0x3) << 6) |
      (index & 0x7) << 3 |
      (base & 0x7);
    return *this;
  }

  opcode &opcode::modrm_reg(const compile::RtlRegisterMapper &regs,
                            const compile::RegClass &cls,
                            const compile::RtlVariablePtr &b) {
    compile::Register r(chooseRegister(regs, cls, b));
    return modrm_reg(r);
  }

  opcode &opcode::modrm_reg(const compile::Register &r) {
    Reg g;
    bool rbit(false);
    g = translate_reg(r, rbit);

    if ( rbit )
      rexvexr(true);

    return modrm_reg(g);
  }

  opcode &opcode::modrm_reg(std::uint8_t reg) {
    std::uint8_t regp((reg & 0x7) << 3);
    m_modrm = (m_modrm & 0xC7) | regp;
    m_flags |= k_has_modrm;
    return *this;
  }

  opcode::Reg opcode::get_modrm_reg(bool &rbit) const {
    rbit = false;

    if ( !has_modrm() ) return Reg000;

    if ( has_vex() )
      rbit = get_vexr();
    else
      rbit = get_rexr();

    return (Reg) ((m_modrm >> 3) & 0x7);
  }

  opcode::Reg opcode::get_modrm_rmreg(bool &rbit) const {
    rbit = false;

    if ( !has_modrm() ) return Reg000;

    if ( has_vex() )
      rbit = get_vexb();
    else
      rbit = get_rexb();

    return (Reg) (m_modrm & 0x7);
  }

  opcode &opcode::avx_reg(const compile::Register &r) {
    Reg g;
    bool rbit(false);
    g = translate_reg(r, rbit);

    if ( has_modrm_rmreg() ) {
      bool existing_rbit;
      Reg existing_g;
      existing_g = get_modrm_rmreg(existing_rbit);

      if ( g == existing_g && rbit == existing_rbit ) return *this;
    }

    std::uint8_t fullreg((std::uint8_t) g);
    if ( rbit )
      fullreg |= 0x08;

    return avx_reg(fullreg);
  }

  opcode &opcode::avx_reg(std::uint8_t g) {
    if ( !has_vex() ) vex();

    std::uint8_t bits(((~g) & 0xF) << 3);
    int i(has_vex2() ? 1 : 0);
    m_vex[i] = (m_vex[i] & 0b10000111) | bits;

    std::cerr << "Reg is " << (int)g<< " " << (int) bits << std::endl;

    return *this;
  }

  opcode &opcode::avx_reg(const compile::RtlRegisterMapper &regs,
                          const compile::RegClass &cls,
                          const compile::RtlVariablePtr &b) {
    compile::Register r(chooseRegister(regs, cls, b));
    return avx_reg(r);
  }

  opcode &opcode::rex() {
    m_rex = (m_rex & 0xF) | 0x40;
    if ( m_flags & k_has_vex ) {

      // Translate opcode back
      std::uint8_t oppre[2] = { 0x0F, 00 };
      int oppresz(1);
      if ( has_vex2() ) {
        switch ( m_vex[0] & 0x1F ) {
        case 0x01: break;

        case 0x02:
          oppre[1] = 0x38;
          oppresz = 2;
          break;

        case 0x03:
          oppre[1] = 0x3A;
          oppresz = 2;
          break;

        default:
          throw InvalidOpcode("invalid vex m-mmmm bits");
        }
      }

      if ( (m_opcodesz + oppresz) > 3 )
        throw InvalidOpcode("Implied vex prefix makes opcode too long");

      std::copy(m_opcode, m_opcode + m_opcodesz, m_opcode + oppresz);
      std::copy(oppre, oppre + oppresz, m_opcode);
      m_opcodesz += oppresz;

      m_opcode[2] = m_opcode[0];

      bool w(get_vexw()), r(get_vexr()), x(get_vexx()), b(get_vexb());

      m_flags &= ~(k_has_vex | k_has_vex2);

      // Translate bits
      rexw(w);
      rexr(r);
      rexx(x);
      rexb(x);
    }
    return *this;
  }

  opcode &opcode::vex() {
    std::cerr << "Adding vex " << has_rex() << std::endl;

    m_vex[0] = 0b11111000;

    if ( has_rex() ) {
      bool w(get_rexw()), r(get_rexr()), x(get_rexx()), b(get_rexb());

      m_flags |= k_has_vex;
      m_rex = 0;

      vexw(w);
      vexr(r);
      vexx(x);
      vexb(b);

      std::cerr << "Translate rex to vex " << w << " " << r << " " << x << " " << b << std::endl;
    } else {
      m_flags |= k_has_vex;
    }

    if ( m_opcode[0] != 0x0F )
      throw InvalidOpcode("Invalid SSE prefix");

    // Now do opcode translation
    if ( m_opcodesz == 1 )
      throw InvalidOpcode("Invalid opcode for vex prefix");

    std::copy(m_opcode + 1, m_opcode + m_opcodesz, m_opcode);
    m_opcodesz --;

    if ( m_opcodesz > 1 ) {
      if ( m_opcode[0] == 0x38 || m_opcode[1] == 0x3A ) {
        // Need 2 byte vex
        vex2();

        if ( m_opcode[0] == 0x38 )
          m_vex[0] |= 0x02;
        else
          m_vex[0] |= 0x03;

        std::copy(m_opcode + 1, m_opcode + m_opcodesz, m_opcode);
        m_opcodesz--;
      }
    }

    return *this;
  }

  opcode &opcode::vex2() {
    if ( has_vex2() ) return *this;

    m_flags |= k_has_vex2;
    m_vex[1] = m_vex[0] & 0x7F;
    m_vex[0] = m_vex[0] & 0x80;
    m_vex[0] |= 0x01; // 2 -byte vex is assumed 0x0f;

    return *this;
  }

  opcode &opcode::vexbit(modrm_extension bit, bool v) {
    switch (bit) {
    case modrm_rbit: return vexr(v);
    case modrm_xbit: return vexx(v);
    case modrm_wbit: return vexw(v);
    case modrm_bbit: return vexb(v);
    }
    return *this;
  }

  opcode &opcode::vexw(bool v) {
    if ( !has_vex2() ) {
      if ( !v ) return *this;
      std::cerr << "Force vex2" << std::endl;
      vex2();
    }

    if ( v )
      m_vex[1] |= 0x80; // Set W bit
    else
      m_vex[1] &= 0x7F;

    return *this;
  }

  opcode &opcode::vexr(bool v) {
    if ( !has_vex2() ) {
      if ( !v ) return *this;
      vex2();
    }

    // inverted
    if ( v )
      m_vex[0] &= 0x7F;
    else
      m_vex[0] |= 0x80;

    return *this;
  }

  opcode &opcode::vexx(bool v) {
    if ( (m_flags & k_has_vex2) == 0 ) {
      if ( !v ) return *this;
      vex2();
    }

    // inverted
    if ( v )
      m_vex[0] &= 0xBF;
    else
      m_vex[0] |= 0x40;

    return *this;
  }

  opcode &opcode::vexb(bool v) {
    if ( (m_flags & k_has_vex2) == 0 ) {
      if ( !v ) return *this;
      vex2();
    }

    // inverted
    if ( v )
      m_vex[0] &= 0xDF;
    else
      m_vex[0] |= 0x20;

    return *this;
  }

  bool opcode::get_vexw() const {
    if ( has_vex2() )
      return (m_vex[1] & 0x80);
    else return false;
  }

  bool opcode::get_vexr() const {
    return has_vex() && (m_vex[0] & 0x80) == 0;
  }

  bool opcode::get_vexx() const {
    if ( has_vex2() )
      return (m_vex[0] & 0x40) == 0;
    else return false;
  }

  bool opcode::get_vexb() const {
    if ( has_vex2() )
      return m_vex[0] & 0x20 == 0;
    else return false;
  }

  opcode::Reg opcode::translate_reg(const compile::Register &r, bool &rbit) {
    Reg g;

    rbit = false;

    if ( r == registers.rax() || r == registers.r8() || r == registers.zmm0() || r == registers.zmm8() )
      g = Reg000;
    else if ( r == registers.rcx() || r == registers.r9() || r == registers.zmm1() || r == registers.zmm9() )
      g = Reg001;
    else if ( r == registers.rdx() || r == registers.r10() || r == registers.zmm2() || r == registers.zmm10() )
      g = Reg010;
    else if ( r == registers.rbx() || r == registers.r11() || r == registers.zmm3() || r == registers.zmm11() )
      g = Reg011;
    else if ( r == registers.rsp() || r == registers.r12() || r == registers.zmm4() || r == registers.zmm12() )
      g = Reg100;
    else if ( r == registers.rbp() || r == registers.r13() || r == registers.zmm5() || r == registers.zmm13() )
      g = Reg101;
    else if ( r == registers.rsi() || r == registers.r14() || r == registers.zmm6() || r == registers.zmm14() )
      g = Reg110;
    else if ( r == registers.rdi() || r == registers.r15() || r == registers.zmm7() || r == registers.zmm15() )
      g = Reg111;
    else
      throw InvalidOpcode("Can't fit this register into a Modrm Reg field");

    if ( r == registers.zmm8() ||
         r == registers.zmm9() ||
         r == registers.zmm10() ||
         r == registers.zmm11() ||
         r == registers.zmm12() ||
         r == registers.zmm13() ||
         r == registers.zmm14() ||
         r == registers.zmm15() ||

         r == registers.r8() ||
         r == registers.r9() ||
         r == registers.r10() ||
         r == registers.r11() ||
         r == registers.r12() ||
         r == registers.r13() ||
         r == registers.r14() ||
         r == registers.r15() )
      rbit = true;

    return g;
  }

  void opcode::clear() {
    m_opcodesz = 0;
    m_rex = 0;
    m_flags = 0;
    m_modrm = m_sib = 0;
    m_simd_prefix = simd_packed_single;
    std::fill(std::begin(m_vex), std::end(m_vex), 0);

    m_immsz = 0;
  }

  bool opcode::has_sib() const {
    if ( !has_modrm() ) return false;

    return mod() != Mod11 && rm() == Rm100;
  }

  bool opcode::has_disp() const {
    if ( !has_modrm() ) return false;

    return (mod() == Mod00 && rm() == Rm101) ||
      mod() == Mod01 || mod() == Mod10;
  }

  bool opcode::has_modrm_rmreg() const {
    return mod() == Mod11;
  }

  std::size_t opcode::dispsz() const {
    if ( mod() == Mod00 && rm() == Rm101 ) return 4;
    else if ( mod() == Mod01 ) return 1;
    else if ( mod() == Mod10 ) return 4;
    else return 0;
  }

  bool opcode::has_rex() const {
    return m_rex > 0;
  }

  std::ostream &opcode::dump(std::ostream &o) const {
    if ( has_simd_prefix() && !has_vex() ) {
      std::uint8_t pre(simd_prefix());
      o.write((const char *) &pre, 1);
    }

    if ( has_vex() ) {
      std::uint8_t vex[3] { 0xC5, 0, 0 };
      std::copy(std::begin(m_vex), std::end(m_vex), vex + 1);

      std::uint8_t pp;
      switch ( m_simd_prefix ) {
      default:
      case simd_packed_single:
        pp = 0x00;
        break;

      case simd_packed_double:
        pp = 0x01;
        break;

      case simd_scalar_single:
        pp = 0x02;
        break;

      case simd_scalar_double:
        pp = 0x03;
        break;
      }

      if ( has_vex2() ) {
        vex[0] = 0xC4;
        vex[2] &= 0xFC;
        vex[2] |= pp;
      } else {
        vex[1] &= 0xFC;
        vex[1] |= pp;
      }

      o.write((const char *) vex, has_vex2() ? 3 : 2);
    } else if ( has_rex() )
      o.write((const char *) &m_rex, 1);


    o.write((const char *) m_opcode, opcodesz());
    if ( has_modrm() )
      o.write((const char *) &m_modrm, 1);
    if ( has_sib() ) {
      o.write((const char *) &m_sib, 1);
    }
    if ( has_disp() ) {
      o.write((const char *) &m_disp, dispsz());
    }
    if ( has_immediate() )
      o.write((const char *) m_imm, immsz());

    return o;
  }

  opcode_builder::opcode_builder(const compile::RtlRegisterMapper &inputs,
                                 const compile::RtlRegisterMapper &outputs,
                                 compile::BytecodeEmitter &out)
    : m_inputs(inputs), m_outputs(outputs), m_out(out) {
  }

  void opcode_builder::c_xor(const types::modrm &dest, const types::modrm &a, const types::modrmimm &b) {
    RtlType &ty(*a.type());
    ArchTypeInfo size(ty);

    if ( dest.isAReg(m_outputs) && b.isImmediate() && size.width() <= 32 ) {
      // Check if the dest (or A arg) is 64-bits
      if ( dest.isRegister() ) {
        ArchTypeInfo dstSize(*dest.base()->type());
        if ( dstSize.width() == 64 &&
             size.width() == 32 ) {
          // Sign extend
          throw std::runtime_error("sign extened xor not supported"); // TODO
        }
      }

      // Use EAX, imm opcodes
      if ( size.width() <= 8 ) {
        emit(opcode(0x34).immediate(b.immediate(), 8));
      } else {
        emit(opcode(0x35).immediate(b.immediate(), 32));
      }
    } else if ( b.isImmediate() ) {
      if ( size.width() <= 8 ) {
        emit(opcode(0x80).modrm(m_outputs, regclass::gp, dest).modrm_reg(6).immediate(b.immediate(), 8));
      } else if ( size.width() <= 32 ) {
        emit(opcode(0x81).modrm(m_outputs, regclass::gp, dest).modrm_reg(6).immediate(b.immediate(), 32));
      } else
        throw InvalidOpcode("Xor with memory/register cannot be used with an immediate of size greater than 32");
    } else if ( b.isRegister() ) {
      // MR encoding
      if ( size.width() <= 8 ) {
        emit(opcode(0x30).modrm(m_outputs, regclass::gp, dest).modrm_reg(m_inputs, regclass::gp, b.reg().base()));
      } else if ( size.width() <= 32 ) {
        emit(opcode(0x31).modrm(m_outputs, regclass::gp, dest).modrm_reg(m_inputs, regclass::gp, b.reg().base()));
      } else {
        emit(opcode(0x31).rexw().modrm(m_outputs, regclass::gp, dest).modrm_reg(m_inputs, regclass::gp, b.reg().base()));
      }
    } else if ( b.isMemoryRef() ) {
      // RM encoding
      if ( size.width() <= 8 )
        emit(opcode(0x32).modrm(m_inputs, regclass::gp, b.reg()).modrm_reg(m_outputs, regclass::gp, dest.base()));
      else if ( size.width() <= 32 )
        emit(opcode(0x33).modrm(m_inputs, regclass::gp, b.reg()).modrm_reg(m_outputs, regclass::gp, dest.base()));
      else
        emit(opcode(0x33).rexw().modrm(m_inputs, regclass::gp, b.reg()).modrm_reg(m_outputs, regclass::gp, dest.base()));
    }
  }

  void opcode_builder:: test(const compile::RtlVariablePtr &a, const compile::RtlVariablePtr &b,
                             const compile::RtlVariablePtr &sign, const compile::RtlVariablePtr &zero,
                             const compile::RtlVariablePtr &parity) {
  }

  void opcode_builder:: cmp(const types::modrm &a, const types::modrmimm &b,
                            const compile::RtlVariablePtr &sign, const compile::RtlVariablePtr &carry) {
    RtlType &ty(*a.type());
    ArchTypeInfo size(ty);
    assert(size.isIntType() && size.width() <= 64);

    if ( a.isAReg(m_inputs) && b.isImmediate() && size.width() <= 32 ) {
      if ( size.width() <= 8 )
        emit(opcode(0x3C).immediate(b.immediate(), 8));
      else
        emit(opcode(0x3D).immediate(b.immediate(), 32));
    } else if ( b.isImmediate() ) {
      if ( size.width() <= 8 )
        emit(opcode(0x80).modrm(m_inputs, regclass::gp, a).modrm_reg(7).immediate(b.immediate(), 8));
      else
        emit(opcode(0x81).modrm(m_inputs, regclass::gp, a).modrm_reg(7).immediate(b.immediate(), 32));
    } else if ( b.isRegister() ) {
      if ( size.width() <= 8 )
        emit(opcode(0x38).modrm(m_inputs, regclass::gp, a).modrm_reg(m_inputs, regclass::gp, b.reg().base()));
      else if ( size.width() <= 32 )
        emit(opcode(0x39).modrm(m_inputs, regclass::gp, a).modrm_reg(m_inputs, regclass::gp, b.reg().base()));
      else
        emit(opcode(0x39).rexw().modrm(m_inputs, regclass::gp, a).modrm_reg(m_inputs, regclass::gp, b.reg().base()));
    } else if ( b.isMemoryRef() ) {
      if ( size.width() <= 8 )
        emit(opcode(0x3A).modrm(m_inputs, regclass::gp, b.reg()).modrm_reg(m_inputs, regclass::gp, a.base()));
      else if ( size.width() <= 32 )
        emit(opcode(0x3B).modrm(m_inputs, regclass::gp, b.reg()).modrm_reg(m_inputs, regclass::gp, a.base()));
      else
        emit(opcode(0x3B).modrm(m_inputs, regclass::gp, b.reg()).modrm_reg(m_inputs, regclass::gp, a.base()).rexw());
    }
  }

  void opcode_builder::dec(const types::modrm &dst, const types::modrm &src) {
    ArchTypeInfo size(*dst.type());
    assert(size.isIntType() && size.width() <= 64);

    if ( size.width() <= 32 && dst.isRegister() ) {
      opcode o(0x48);
      o.opreg(m_outputs, regclass::gp, dst.base());
      emit(o);
    } else {
      std::uint8_t opc(0xFF);
      if ( size.width() <= 8 )
        opc = 0xFE;

      opcode o(opc);
      if ( size.width() > 32 )
        o.rexw();

      o.modrm_reg(1).modrm(m_outputs, regclass::gp, dst);
      emit(o);
    }
  }

  void opcode_builder::inc(const types::modrm &dst, const types::modrm &src) {
    ArchTypeInfo size(*dst.type());
    assert(size.isIntType() && size.width() <= 64);

    std::uint8_t opc(0xFF);
    if ( size.width() <= 8 )
      opc = 0xFE;

    opcode o(opc);
    if ( size.width() > 32 )
      o.rexw();

    o.modrm_reg(0).modrm(m_outputs, regclass::gp, dst);
    emit(o);
  }

  void opcode_builder::sub(const types::modrm &dst, const types::modrm &a, const types::modrmimm &b) {
    ArchTypeInfo bty(*b.type()), aty(*a.type());;
    assert(bty.isIntType() && bty.width() <= 64);

    if ( aty.width() != bty.width() ) throw std::runtime_error("signe extended sub not supported");
    if ( a.isAReg(m_outputs) && b.isImmediate() && bty.width() <= 32 ) {
      // EAX, imm opcodes (I)
      if ( bty.width() <= 8 )
        emit(opcode(0x2C).immediate(b.immediate(), 8));
      else
        emit(opcode(0x2D).immediate(b.immediate(), 32));
    } else if ( b.isImmediate() ) {
      if ( bty.width() <= 8 )
        emit(opcode(0x80).modrm_reg(5).modrm(m_inputs, regclass::gp, a).immediate(b.immediate(), 8));
      else if ( bty.width() <= 32 )
        emit(opcode(0x81).modrm_reg(5).modrm(m_inputs, regclass::gp, a).immediate(b.immediate(), 32));
    } else if ( b.isRegister() ) {
      if ( bty.width() <= 8 )
        emit(opcode(0x28).modrm_reg(m_inputs, regclass::gp, b.reg().base()).modrm(m_inputs, regclass::gp, a));
      else {
        opcode opc(0x29);
        opc.modrm_reg(m_inputs, regclass::gp, b.reg().base()).modrm(m_inputs, regclass::gp, a);
        if ( bty.width() > 32 )
          opc.rexw();

        emit(opc);
      }
    } else if ( b.isMemoryRef() ) {
      if ( bty.width() <= 8 )
        emit(opcode(0x2A).modrm_reg(m_inputs, regclass::gp, a.base()).modrm(m_inputs, regclass::gp, b.reg()));
      else {
        opcode opc(0x2B);
        opc.modrm_reg(m_inputs, regclass::gp, a.base()).modrm(m_inputs, regclass::gp, b.reg());
        if ( bty.width() > 32 )
          opc.rexw();

        emit(opc);
     }
    }
  }

  void opcode_builder::imul(const types::modrm &dst, const types::modrm &a, const types::modrmimm &b) {
    ArchTypeInfo bty(*b.type()), aty(*a.type());
    assert(bty.isIntType() && bty.width() <= 64 && bty.intFormat() == RtlTwosComplement);

//    if ( dst.isAReg(m_outputs) && !b.isImmediate() ) {
//      emit(opcode(bty.width() <= 8 ? 0xF6 : 0xF7).modrm_reg(5).modrm(m_inputs, regclass::gp, b.reg()));
//    } else // This is only for 128-bit
    if ( !b.isImmediate() ) {
      if ( bty.width() < 32 )
        throw std::runtime_error("Can't do signed mul less than 32-bits");
      else
        emit(opcode(0x0F, 0xAF).modrm_reg(m_outputs, regclass::gp, dst.base()).modrm(m_inputs, regclass::gp, b.reg()));
    } else {
      if ( bty.width() <= 8 )
        emit(opcode(0x6B).modrm_reg(m_outputs, regclass::gp, dst.base()).modrm(m_inputs, regclass::gp, a).immediate(b.immediate(), 8));
      else if ( bty.width() <= 32 )
        emit(opcode(0x69).modrm_reg(m_outputs, regclass::gp, dst.base()).modrm(m_inputs, regclass::gp, a).immediate(b.immediate(), 32));
      else
        throw std::runtime_error("Immediate sized incorrectly");
    }
  }

  void opcode_builder::add(const types::modrm &dst, const types::modrm &a, const types::modrmimm &b) {
    ArchTypeInfo bty(*b.type()), aty(*a.type());
    assert(bty.isIntType() && bty.width() <= 64);

    if ( aty.width() != bty.width() ) throw std::runtime_error("signe extended sub not supported");
    if ( dst.isAReg(m_outputs) && b.isImmediate() && bty.width() <= 32 ) {
      // EAX, imm opcodes (I)
      if ( bty.width() <= 8 )
        emit(opcode(0x04).immediate(b.immediate(), 8));
      else
        emit(opcode(0x05).immediate(b.immediate(), 32));
    } else if ( b.isImmediate() ) {
      if ( bty.width() <= 8 )
        emit(opcode(0x80).modrm_reg(0).modrm(m_outputs, regclass::gp, dst).immediate(b.immediate(), 8));
      else if ( bty.width() <= 32 )
        emit(opcode(0x81).modrm_reg(0).modrm(m_outputs, regclass::gp, dst).immediate(b.immediate(), 32));
    } else if ( b.isRegister() ) {
      if ( bty.width() <= 8 )
        emit(opcode(0x00).modrm_reg(m_inputs, regclass::gp, b.reg().base()).modrm(m_outputs, regclass::gp, dst));
      else {
        opcode opc(0x01);
        opc.modrm_reg(m_inputs, regclass::gp, b.reg().base()).modrm(m_outputs, regclass::gp, dst);
        if ( bty.width() > 32 )
          opc.rexw();

        emit(opc);
      }
    } else if ( b.isMemoryRef() ) {
      if ( bty.width() <= 8 )
        emit(opcode(0x02).modrm_reg(m_outputs, regclass::gp, dst.base()).modrm(m_inputs, regclass::gp, b.reg()));
      else {
        opcode opc(0x03);
        opc.modrm_reg(m_outputs, regclass::gp, dst.base()).modrm(m_inputs, regclass::gp, b.reg());
        if ( bty.width() > 32 )
          opc.rexw();

        emit(opc);
     }
    }
  }

  void opcode_builder::lea(const compile::RtlVariablePtr &dst, const types::modrm &src) {
    RtlType &ty(*dst->type());
    ArchTypeInfo size(ty);
    assert(size.isIntType() && size.width() <= 64);

    opcode o(0x8D);
    if ( size.width() > 32 )
      o.rexw();
    o.modrm(m_inputs, regclass::gp, src).modrm_reg(m_outputs, regclass::gp, dst);
    emit(o);
  }

  void opcode_builder::mov(const types::modrm &dst, const types::modrmimm &src) {
    RtlType &ty(*dst.type());
    ArchTypeInfo size(ty);
    assert(size.isIntType() && size.width() <= 64);

    std::uint8_t opc;

    if ( src.isImmediate() ) {

      std::uint8_t sz;
      if ( size.width() <= 8 )
        sz = 8;
      else if ( size.width() <= 32 )
        sz = 32;
      else
        sz = 64;

      if ( dst.isRegister() ) {
        if ( size.width() <= 8 )
          opc = 0xB0;
        else
          opc = 0xB8;

        opcode o(opc);
        if ( size.width() > 32 )
          o.rexw();

        o.opreg(m_outputs, regclass::gp, dst.base());

        o.immediate(src.immediate(), sz);
        emit(o);
      } else {
        if ( size.width() <= 8 )
          opc = 0xC6;
        else if ( size.width() <= 32 )
          opc = 0xC7;
        else
          throw InvalidOpcode("Can't move 64-bit to memref");

        opcode o(opc);
        o.modrm_reg(0).modrm(m_outputs, regclass::gp, dst);
        o.immediate(src.immediate(), sz);

        emit(o);
      }
    } else if ( dst.isMemoryRef() ) {
      if ( size.width() <= 8 )
        opc = 0x88;
      else
        opc = 0x89;

      opcode o(opc);
      if ( size.width() > 32 )
        o.rexw();

      o.modrm(m_outputs, regclass::gp, dst).modrm_reg(m_inputs, regclass::baseregs, src.reg().base());
      emit(o);
    } else {
      if ( size.width() <= 8 )
        opc = 0x8A;
      else
        opc = 0x8B;

      opcode o(opc);
      if ( size.width() > 32 )
        o.rexw();
      o.modrm(m_inputs, regclass::gp, src.reg()).modrm_reg(m_outputs, regclass::baseregs, dst.base());

      emit(o);
    }
  }

  void opcode_builder::movzx(const compile::RtlVariablePtr &dst,
                             const types::modrm &src) {
    std::uint8_t szcode(0xB6);
    RtlTypeInfo srcinfo(*src.type()), dstinfo(*dst->type());
    assert(srcinfo.width() <= 16);
    assert(dstinfo.width() <= 64);

    if (srcinfo.width() > 8)
      szcode = 0xB7;

    opcode o(0x0F, szcode);

    if (dstinfo.width() > 32)
      o.rexw();

    o.modrm_reg(m_outputs, regclass::gp, dst).modrm(m_inputs, regclass::gp, src);

    emit(o);
  }

  void opcode_builder::mulss(const compile::RtlVariablePtr &dst,
                             const compile::RtlVariablePtr &a,
                             const types::modrm &b) {
    opcode o(0x0f, 0x59);

    o.simd_prefix(opcode::simd_scalar_single);
    o.modrm_reg(m_outputs, regclass::avx, dst).modrm(m_inputs, regclass::avx, b);
    o.avx_reg(m_inputs, regclass::avx, a);

    emit(o);
  }

  void opcode_builder::addss(const compile::RtlVariablePtr &dst, const compile::RtlVariablePtr &a,
                              const types::modrm &b) {
    opcode o(0x0F, 0x58);

    compile::Register areg(chooseRegister(m_inputs, regclass::avx, a));
    compile::Register breg(chooseRegister(m_inputs, regclass::avx, b.base()));
    compile::Register dstreg(chooseRegister(m_outputs, regclass::avx, dst));

    std::cerr << "ADDSS REGISTERS ARE " << dstreg << " = " << areg << " + " << breg << std::endl;
    o.simd_prefix(opcode::simd_scalar_single);
    o.modrm_reg(m_outputs, regclass::avx, dst).modrm(m_inputs, regclass::avx, b);

    o.avx_reg(m_inputs, regclass::avx, a);

    emit(o);
  }

  void opcode_builder::movss(const types::modrm &dst, const types::modrmimm &src) {
    if ( src.isMemoryRef() ) {
      emit(opcode(0x0F, 0x10).simd_prefix(opcode::simd_scalar_single).modrm_reg(m_outputs, regclass::avx, dst.base()).modrm(m_inputs, regclass::avx, src.reg()));
    } else if ( dst.isMemoryRef() ) {
      emit(opcode(0x0F, 0x11).simd_prefix(opcode::simd_scalar_single).modrm_reg(m_outputs, regclass::avx, src.reg().base()).modrm(m_inputs, regclass::avx, dst));
    } else if (src.isImmediate()) {
      auto offset(allocate_constant(src.immediate()));
      emit(opcode(0x0F, 0x10).simd_prefix(opcode::simd_scalar_single)
           .modrm(opcode::Mod00, opcode::Reg100)
           .sib(0, opcode::IndexNone, opcode::BaseNone)
           .modrm_reg(m_outputs, regclass::avx, dst.base()));

      auto disp(std::make_unique<Constant32Fixup>(offset));
      fixup(std::move(disp));
    } else {
      emit(opcode(0x0F, 0x10).simd_prefix(opcode::simd_scalar_single).modrm_reg(m_outputs, regclass::avx, dst.base()).modrm(m_inputs, regclass::avx, src.reg().base()));
    }
  }

  void opcode_builder::cvtsi2ss(const compile::RtlVariablePtr &dst,
                                const types::modrm &src) {
    opcode o(0x0F, 0x2A);
    RtlTypeInfo srctype(*src.type());

    o.simd_prefix(opcode::simd_scalar_single);

    if ( srctype.width() >= 32 )
      o.rexw(); // Enable 64-bit version

    o.modrm_reg(m_outputs, regclass::avx, dst).modrm(m_inputs, regclass::gp, src);

    emit(o);
  }

  void opcode_builder::fixup(std::unique_ptr<compile::BytecodeFixup> &&f) {
    m_out.write_fixup(compile::BytecodeInfo::CodeSection, std::move(f));
  }

  std::uintptr_t opcode_builder::allocate_constant(const types::imm &i) const {
    union {
      uint64_t v;
      std::uint8_t d[8];
    };
    v = i.value();

    std::span<std::uint8_t> span(d, i.type()->size());
    return m_out.allocate_constant(span);
  }

  void opcode_builder::emit(const opcode &e) {
    std::stringstream s;
    e.dump(s);
    std::string ss(s.str());
    std::transform(ss.begin(), ss.end(), std::ostream_iterator<unsigned int>(std::cerr, " "),
                   [](char c) {
                     std::uint8_t u(c);
                     return u;
                   });
    std::cerr << std::dec << std::endl;

    m_out.write_bytes(compile::BytecodeInfo::CodeSection, (const std::uint8_t *)ss.data(), ss.size());
  }

  class RelJump32 : public JumpInsn {
  public:
    RelJump32(std::int32_t offs, std::optional<compile::Register> v)
      : m_cond(v) {
      relative(offs);
    }
    virtual ~RelJump32() {
    }

    virtual std::size_t size() const {
      if ( !m_cond ) {
        return 5;
      } else {
        return 6;
      }
    }

    virtual void output(std::ostream &o) const {
      emitOpcode(o);
      o.write((const char *) &m_offset, 4);
    }

    virtual std::int64_t max_distance() const {
      return std::numeric_limits<decltype(m_offset)>::max();
    }

    virtual void absolute(std::uint64_t i) {
      throw std::runtime_error("absolute jump not supported");
    }

    virtual void relative(std::int64_t i) {
      if ( i < 0 )
        i -= size();

      m_offset = i;
    }

  private:
    void emitOpcode(std::ostream &o) const {
      if ( !m_cond ) {
        std::uint8_t b(0xE9);
        o.write((const char *)&b, 1);
      } else {
        compile::Register cond(*m_cond);

        std::uint8_t b(0x0F);
        o.write((const char *)&b, 1);

        if ( cond == registers.sf() ) b = 0x88;
        else if ( cond == registers.pf() ) b = 0x8A;
        else if ( cond == registers.cf() ) b = 0x82;
        else if ( cond == registers.zf() ) b = 0x84;
        else
          throw std::runtime_error("conditional jump");

        o.write((const char *)&b, 1);
      }
    }

    std::int32_t m_offset;
    std::optional<compile::Register> m_cond;
  };


  class RelJump8 : public JumpInsn {
  public:
    RelJump8(std::int8_t offs, std::optional<compile::Register> v)
      : m_cond(v) {
      relative(offs);
    }
    virtual ~RelJump8() {
    }

    virtual std::size_t size() const {
      return 2;
    }

    virtual void output(std::ostream &o) const {
      emitOpcode(o);
      o.write((const char *) &m_offset, 1);
    }

    virtual std::int64_t max_distance() const {
      return std::numeric_limits<decltype(m_offset)>::max();
    }

    virtual void absolute(std::uint64_t i) {
      throw std::runtime_error("absolute jump not supported");
    }

    virtual void relative(std::int64_t i) {
      if ( i < 0 ) i -= size();
      m_offset = i;
    }
  private:
    void emitOpcode(std::ostream &o) const {
      if ( !m_cond ) {
        std::uint8_t b(0xEB);
        o.write((const char *)&b, 1);
      } else {
        throw std::runtime_error("conditional jump 8");
      }
    }

    std::int8_t m_offset;
    std::optional<compile::Register> m_cond;
  };

  jumps::~jumps() {
  }

  const compile::RegClass &jumps::indirect_jump_class() const {
    return regclass::gp;
  }

  const compile::RegClass &jumps::jump_cond_class() const {
    return regclass::flags;
  }

  std::unique_ptr<compile::JumpInsn> jumps::jump_relative(std::int64_t offs, std::optional<compile::Register> v) const {
    std::int8_t rel8(offs);
    if ( rel8 == offs ) {
      return std::make_unique<RelJump8>(rel8, v);
    }

    std::int32_t rel32(offs);
    if ( rel32 != offs ) throw std::runtime_error("Relative jump too far");

    return std::make_unique<RelJump32>(offs, v);
  }

  std::unique_ptr<compile::JumpInsn> jumps::jump_absolute(std::int64_t abs, std::optional<compile::Register> v) const {
    return nullptr;
  }

  std::unique_ptr<compile::JumpInsn> jumps::jump_indirect(compile::Register r, std::optional<compile::Register> v) const {
    return nullptr;
  }

  std::unique_ptr<compile::JumpInsn> jumps::generic_jump(std::optional<compile::Register> v) const {
    return std::make_unique<RelJump32>(0, v);
  }

  void jumps::preamble(BytecodeEmitter &out) const {
    std::uint8_t i[] {
      // Int3
      //      0xCC,

      0x55,                  // Push RBP
      0x48, 0x8B, 0b11101101, // Move RBP, RSP
        // push rbx, r12 - r15
      0x53, // Push RBX .. TODO these should only be pushed popped if needed
      0x41, 0x54, // Push R12
      0x41, 0x55, // Push R13
      0x41, 0x56, // Push R14
      0x41, 0x57, // Push R15
    };
    out.write_bytes(compile::BytecodeInfo::CodeSection, i, sizeof i);
  }

  void jumps::postamble(BytecodeEmitter &out) const {
    std::uint8_t i[] {
      0x41, 0x5F, // Pop R15
      0x41, 0x5E, // Pop R14
      0x41, 0x5D, // Pop R13
      0x41, 0x5C, // Pop R12
      0x5B, // Pop RBX
      0x5D, // Pop RBP
    };
    out.write_bytes(compile::BytecodeInfo::CodeSection, i, sizeof i);
  }

  void jumps::funret(BytecodeEmitter &out) const {
    std::uint8_t i[]{0xC3};
    out.write_bytes(compile::BytecodeInfo::CodeSection, i, sizeof i);
  }

  class OutputMapper : public RtlRegisterMapper {
  public:
    OutputMapper(const RtlVariablePtr &var, const VirtualRegister &dest)
      : m_var(var), m_dest(dest) {
    }
    virtual ~OutputMapper() {
    }

    virtual std::vector<VirtualRegister> lookupVar(const RtlVariablePtr &v) const override {
      if ( v->isSame(m_var) ) {
        std::vector<VirtualRegister> r {m_dest};
        return r;
      } else
        return { };
    }

  private:
    RtlVariablePtr m_var;
    VirtualRegister m_dest;
  };

  void jumps::copy(BytecodeEmitter &out, const RtlRegisterMapper &inputs, const RtlVariablePtr &var, const VirtualRegister &dest) const {
    // Get primary register class for dest
    if ( dest.isRegister() ) {
      auto &reg(dest.reg());
      if ( reg.belongsToClass(regclass::gp) ) {
        OutputMapper outputs(var, dest);
        std::cerr << "Copy " << var->name() << " to " << dest << std::endl;
        if (inputs.spilled(var)) {
          std::cerr << "Input is spilled only " << std::endl;
          auto locs(inputs.lookupVar(var));
          OutputMapper inputBase(var, registers.rbp());
          opcode_builder(inputBase, outputs, out).mov(types::modrm::memref(var, var->type(), locs.front().spillSlot()), var);
        } else
          opcode_builder(inputs, outputs, out).mov(var, var);
      } else if (reg.belongsToClass(regclass::avx)) {
        RtlTypeInfo info(*var->type());
        if ( !info.isFloatType() )
          throw std::runtime_error("Can't copy non-float to avx register");

        OutputMapper outputs(var, dest);
        if (info.width() <= 32) {
          opcode_builder(inputs, outputs, out).movss(var, var);
        } else
          throw std::runtime_error("Unsuported width");
      } else {
        std::cerr << "Copy register " << dest << " from " << var->name() << std::endl;
        throw std::runtime_error("Can't copy register");
      }
    } else if ( dest.isSpill() ) {
      auto rbp(std::make_shared<RtlVariable>("rbp", var->type()));
      OutputMapper outputs(rbp, registers.rbp());
      auto locs(inputs.lookupVar(var));

      auto gp(std::find_if(locs.begin(), locs.end(), [](const VirtualRegister &r) {
        return r.isRegister() && r.reg().belongsToClass(regclass::gp);
      }));

      if (gp != locs.end()) {
        opcode_builder(inputs, outputs, out)
          .mov(types::modrm::memref(rbp, var->type(), dest.spillSlot()), var);
      } else {
        throw std::runtime_error("Copying to spill not implemented");
      }
    }
  }
}
