#include "compile/rtl_types.hpp"
#include "compile/rtl.hpp"
#include "compile/x86_64/cconv.hpp"
#include "arch/x86_64/registers.hpp"

using namespace ::nnc::arch::x86_64;
using namespace ::nnc::compile;

namespace nnc::arch::x86_64::cconv {
  ccall::ccall()
    : m_gpreg(0), m_xmmreg(0), m_stackslot(0) {
  }

  ccall::~ccall() {
  }

  /*
   * The size of each argument gets rounded up to eightbytes.
   * The basic types are assigned their natural classes:
   * • Arguments of types (signed and unsigned) _Bool, char, short, int,
   * long, long long, and pointers are in the INTEGER class.
   * • Arguments of types float, double and __m64 are in class SSE.
   * • Arguments of types __float128 and __m128 are split into two halves.
   * The least significant ones belong to class SSE, the most significant one to
   * class SSEUP.
   * • The 64-bit mantissa of arguments of type long double belongs to class
   * X87, the 16-bit exponent plus 6 bytes of padding belongs to class X87UP.
   * • Arguments of type __int128 offer the same operations as INTEGERs,
   * yet they do not fit into one general purpose register but require two registers.
   * For classification purposes __int128 is treated as if it were implemented
   * 9Therefore the stack will always be eightbyte aligned.
   * 15
   * x86-64 ABI Draft 0.21 – September 13, 2002 – 4:57
   * as: t ypedef struct long low, high; __int128; with the exception that
   * arguments of type __int128 that are stored in memory must be aligned
   * on a 16-byte boundary.
   * The classification of aggregate (structures and arrays) and union types works
   * as follows:
   * 1. If the size of an object is larger than two eightbytes, or in C++, is a nonPOD 10 structure or union type, or contains unaligned fields, it has class
   * MEMORY.11
   * 2. Both eightbytes get initialized to class NO_CLASS.
   * 3. Each field of an object is classified recursively so that always two fields are
   * considered. The resulting class is calculated according to the classes of the
   * fields in the eightbyte:
   * (a) If both classes are equal, this is the resulting class.
   * (b) If one of the classes is NO_CLASS, the resulting class is the other
   * class.
   * (c) If one of the classes is MEMORY, the result is the MEMORY class.
   * (d) If one of the classes is INTEGER, the result is the INTEGER.
   * (e) If one of the classes is X87 or X87UP class, MEMORY is used as
   * class.
   * (f) Otherwise class SSE is used.
   * 4. Then a post merger cleanup is done:
   * (a) If one of the classes is MEMORY, the whole argument is passed in
   * memory.
   * (b) If SSEUP is not preceeded by SSE, it is converted to SSE
  */
  ccall::ArgClass ccall::classifyArg(const RtlTypeInfo &ty, std::size_t &sz) {
    sz = ty.width(); // Now in bit size

    // Find byte size by rounding up bit size to eight bits
    sz = (sz + 7) / 8;

    // Round up to size 8
    sz = ((sz + 7) / 8) * 8;

    if ( (ty.isIntType() && ty.width() <= 64) ||
         ty.isPointerType() )
      return INTEGER;
    else if ( ty.isFloatType() && ty.width() <= 64 )
      return SSE;
    else if ( ty.isFloatType() && ty.width() <= 128 )
      return SSEUP; // The standard calls to split the word, but we
                    // can't do that in this return result, but SSEUP
                    // assumes it will be split

    return NO_CLASS;
  }

  ::nnc::compile::VirtualRegister ccall::assignRegToArg(const RtlVariablePtr &v) {
    RtlTypeInfo ty(*v->type());
    std::size_t sz;
    ArgClass cls(classifyArg(ty, sz));

    switch ( cls ) {
    case INTEGER:
      return assignIntReg();

    case SSE:
    case SSEUP:
      return assignSseReg();

    default:
      return assignStackSlot(sz);
    };
  }

  ::nnc::compile::VirtualRegister ccall::assignIntReg() {
    switch ( m_gpreg ) {
    case 0: m_gpreg++; return registers.lookupRegister("rdi");
    case 1: m_gpreg++; return registers.lookupRegister("rsi");
    case 2: m_gpreg++; return registers.lookupRegister("rdx");
    case 3: m_gpreg++; return registers.lookupRegister("rcx");
    case 4: m_gpreg++; return registers.lookupRegister("r8");
    case 5: m_gpreg++; return registers.lookupRegister("r9");
    default: return assignStackSlot(8);
    }
  }

  ::nnc::compile::VirtualRegister ccall::assignSseReg() {
    switch ( m_xmmreg ) {
    case 0: m_xmmreg++; return registers.lookupRegister("xmm0");
    case 1: m_xmmreg++; return registers.lookupRegister("xmm1");
    case 2: m_xmmreg++; return registers.lookupRegister("xmm2");
    case 3: m_xmmreg++; return registers.lookupRegister("xmm3");
    case 4: m_xmmreg++; return registers.lookupRegister("xmm4");
    case 5: m_xmmreg++; return registers.lookupRegister("xmm5");
    case 6: m_xmmreg++; return registers.lookupRegister("xmm6");
    case 7: m_xmmreg++; return registers.lookupRegister("xmm7");
    default: return assignStackSlot(8);
    }
  }

  ::nnc::compile::VirtualRegister ccall::assignStackSlot(int sz) {
    int pos(m_stackslot);
    m_stackslot += sz;
    return ::nnc::compile::VirtualRegister(m_stackslot, sz);
  }
}
