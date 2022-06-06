#ifndef __nnc_compile_rtl_types_HPP__
#define __nnc_compile_rtl_types_HPP__

#include <memory>
#include <span>
#include <map>

namespace nnc {
  namespace compile {
    enum RtlIntSignedness {
      RtlUnsigned,
      RtlTwosComplement,
      RtlOnesComplement
    };

    static constexpr RtlIntSignedness RtlSigned = RtlTwosComplement;

    typedef std::uint16_t RtlIntWidth;
    typedef std::uint16_t RtlFloatWidth;

    class RtlType;

    class RtlTypeFactory {
    public:
      virtual std::shared_ptr<RtlType> intType(RtlIntSignedness s, RtlIntWidth w) =0;
      virtual std::shared_ptr<RtlType> floatType(RtlFloatWidth f) =0;

      virtual std::shared_ptr<RtlType> intptr() =0;
    };

    std::unique_ptr<RtlTypeFactory> typesForCurrentArch();
    std::uint64_t interpretAsInt(const std::span<const std::uint8_t> &v);

    class RtlTypeVisitor {
    public:
      virtual ~RtlTypeVisitor();

      virtual void intType(RtlIntSignedness s, RtlIntWidth w) =0;
      virtual void pointerType(std::shared_ptr<RtlType> underlying) =0;
      virtual void floatType(RtlFloatWidth w) =0;
    };

    class RtlType {
    public:
      virtual ~RtlType();

      virtual void visit(RtlTypeVisitor &v) const =0;
      virtual std::size_t size() const =0;

      std::shared_ptr<RtlType> pointer();

      virtual void output(std::ostream &out) const =0;

    protected:
      virtual std::shared_ptr<RtlType> makePointer() =0;

    private:
      std::weak_ptr<RtlType> m_pointer;
    };

    class RtlTypeInfo : public RtlTypeVisitor {
    public:
      RtlTypeInfo(const RtlType &t);
      virtual ~RtlTypeInfo();

      virtual void intType(RtlIntSignedness s, RtlIntWidth w);
      virtual void pointerType(std::shared_ptr<RtlType> underlying);
      virtual void floatType(RtlFloatWidth w);

      enum TypeClass {
        UnknownClass, FloatClass, IntClass, PointerClass
      };
      inline TypeClass typeClass() const { return m_cls; }

      inline bool isIntType() const { return typeClass() == IntClass; }
      inline bool isFloatType() const { return typeClass() == FloatClass; }
      inline bool isPointerType() const { return typeClass() == PointerClass; }
      inline std::shared_ptr<RtlType> underlying() const { return m_underlying; }

      inline std::size_t width() const { return m_width; }

      inline RtlIntSignedness intFormat() const { return m_signedness; }

    private:

      TypeClass m_cls;
      RtlIntSignedness m_signedness;
      std::size_t m_width;
      std::shared_ptr<RtlType> m_underlying;
    };

    bool isIntType(const RtlType &ty);

    class RtlIntType : public RtlType {
    public:
      RtlIntType(RtlIntSignedness s, RtlIntWidth w);
      virtual ~RtlIntType();

      inline RtlIntSignedness signedness() const { return m_signedness; }
      inline RtlIntWidth width() const { return m_width; }

      virtual std::size_t size() const;
      virtual void output(std::ostream &out) const;

      virtual void visit(RtlTypeVisitor &v) const;

    private:
      RtlIntSignedness m_signedness;
      RtlIntWidth m_width;
    };

    class RtlFloatType : public RtlType {
    public:
      RtlFloatType(RtlFloatWidth w);
      virtual ~RtlFloatType();

      inline RtlFloatWidth width() const { return m_width; }
      virtual std::size_t size() const;

      virtual void visit(RtlTypeVisitor &v) const;

      virtual void output(std::ostream &out) const;

    private:
      RtlFloatWidth m_width;
    };

    template<typename T>
    class RtlTypeTraits {
    public:
      static bool isCompatible(const RtlTypeInfo &info) { return false; }
    };

    template<>
    class RtlTypeTraits<float> {
    public:
      static bool isCompatible(const RtlTypeInfo &info) {
        return info.isFloatType() && info.width() == 32;
      }
    };

    template<>
    class RtlTypeTraits<double> {
    public:
      static bool isCompatible(const RtlTypeInfo &info) { return info.isFloatType() && info.width() == 64; }
    };

#define INT_TYPE_TRAITS(Type, Format, Width)                            \
    template<>                                                          \
    class RtlTypeTraits<Type> {                                         \
    public:                                                             \
      static bool isCompatible(const RtlTypeInfo &info) {               \
        return info.isIntType() && info.intFormat() == Format && info.width() == Width; \
      }                                                                 \
    }

    INT_TYPE_TRAITS(std::int8_t, RtlTwosComplement, 8);
    INT_TYPE_TRAITS(std::int16_t, RtlTwosComplement, 16);
    INT_TYPE_TRAITS(std::int32_t, RtlTwosComplement, 32);
    INT_TYPE_TRAITS(std::int64_t, RtlTwosComplement, 64);
    INT_TYPE_TRAITS(std::uint8_t, RtlUnsigned, 8);
    INT_TYPE_TRAITS(std::uint16_t, RtlUnsigned, 16);
    INT_TYPE_TRAITS(std::uint32_t, RtlUnsigned, 32);
    INT_TYPE_TRAITS(std::uint64_t, RtlUnsigned, 64);

    template<typename T>
    class RtlTypeTraits<T *> {
    public:
      static bool isCompatible(const RtlTypeInfo &info) {
        return info.isPointerType() && RtlTypeTraits<T>::isCompatible(*info.underlying());
      }
    };

  }
}

#endif
