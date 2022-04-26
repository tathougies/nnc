#include "compile/rtl_types.hpp"
#include "compile/rtl_type_factory_impl.hpp"
#include "arch.hpp"

#ifdef NNC_ARCH_X86_64
#include "compile/x86_64.hpp"
#endif

#include <ostream>

namespace nnc {
  namespace compile {
    std::unique_ptr<RtlTypeFactory> typesForCurrentArch() {
#if defined(NNC_ARCH_X86_64)
      auto ret(std::make_unique<::nnc::compile::x86_64::RtlTypeFactory>());
      return ret;
#else
#error Unknown architecture
#endif
    }

    std::size_t sizeFromBitWidth(std::size_t bits) {
      return (bits + 7) / 8;
    }

    RtlTypeVisitor::~RtlTypeVisitor() {
    }

    class IsIntTypeVisitor : public RtlTypeVisitor {
    public:
      IsIntTypeVisitor() : m_isInt(false) { }
      virtual ~IsIntTypeVisitor() { };

      virtual void intType(RtlIntSignedness s, RtlIntWidth w) { m_isInt = true; }
      virtual void pointerType(std::shared_ptr<RtlType> u) { };
      virtual void floatType(RtlFloatWidth w) { };

      inline bool isInt() const { return m_isInt; }

    private:
      bool m_isInt;
    };

    bool isIntType(const RtlType &ty) {
      IsIntTypeVisitor v;
      ty.visit(v);
      return v.isInt();
    }

    std::uint64_t interpretAsInt(const std::span<const std::uint8_t> &v) {
      union {
        std::uint64_t ret;
        std::uint8_t d[8];
      };
      auto last(v.end());
      if ( v.size() > 8 )
        last = v.begin() + 8;
      // TODO endianness
      std::copy(v.begin(), last, d);
      return ret;
    }

    RtlType::~RtlType() {
    }

    std::shared_ptr<RtlType> RtlType::pointer() {
      std::shared_ptr<RtlType> ptr(m_pointer.lock());
      if ( ptr ) return ptr;

      ptr = makePointer();
      m_pointer = ptr;

      return ptr;
    }

    RtlIntType::RtlIntType(RtlIntSignedness s, RtlIntWidth w)
      : m_signedness(s), m_width(w) {
    }

    RtlIntType::~RtlIntType() {
    }

    void RtlIntType::output(std::ostream &out) const {
      if ( signedness() == RtlUnsigned )
        out << "u" << width();
      else if ( signedness() == RtlTwosComplement )
        out << "i" << width();
      else
        out << "o" << width();
    }

    std::size_t RtlIntType::size() const {
      return sizeFromBitWidth(m_width);
    }

    void RtlIntType::visit(RtlTypeVisitor &v) const {
      v.intType(signedness(), width());
    }

    RtlPointerType::RtlPointerType(std::shared_ptr<RtlType> tgt)
      : m_underlying(tgt) {
    }

    RtlPointerType::~RtlPointerType() {
    }

    void RtlPointerType::output(std::ostream &out) const {
      out << "ptr ";
      m_underlying->output(out);
    }

    void RtlPointerType::visit(RtlTypeVisitor &v) const {
      v.pointerType(underlying());
    }

    RtlFloatType::RtlFloatType(RtlFloatWidth w)
      : m_width(w) {
    }

    RtlFloatType::~RtlFloatType() {
    }

    void RtlFloatType::visit(RtlTypeVisitor &v) const {
      v.floatType(width());
    }

    void RtlFloatType::output(std::ostream &out) const {
      out << "fp" << width();
    }

    std::size_t RtlFloatType::size() const {
      return sizeFromBitWidth(m_width);
    }

    RtlTypeInfo::RtlTypeInfo(const RtlType &t)
      : m_cls(UnknownClass), m_signedness(RtlUnsigned), m_width(0) {
      t.visit(*this);
    }

    RtlTypeInfo::~RtlTypeInfo() {
    }

    void RtlTypeInfo::intType(RtlIntSignedness s, RtlIntWidth w) {
      m_cls = IntClass;
      m_width = w;
      m_signedness = s;
    }

    void RtlTypeInfo::floatType(RtlFloatWidth w) {
      m_cls = FloatClass;
      m_width = w;
    }

    void RtlTypeInfo::pointerType(std::shared_ptr<RtlType> underlying) {
      m_cls = PointerClass;
      m_underlying = underlying;
    }
  }
}
