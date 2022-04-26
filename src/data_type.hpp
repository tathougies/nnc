#ifndef __nnc_data_type_HPP__
#define __nnc_data_type_HPP__

#include <string>
#include "compile/rtl_types.hpp"

namespace nnc {

  template<typename T>
  class TypeTraits {
  };

  template<>
  class TypeTraits<std::uint32_t> {
  public:
    static constexpr const char *name = "uint32";
    static constexpr const char *prettyName = "unsigned 4-byte integer";

    static constexpr std::uint32_t zero_value = 0;
    static constexpr std::uint32_t unit_value = 1;

    static std::shared_ptr<compile::RtlType> rtlType(compile::RtlTypeFactory &t) {
      return t.intType(compile::RtlUnsigned, 32);
    }
  };

  template<>
  class TypeTraits<std::uint8_t> {
  public:
    static constexpr const char *name = "uint8";
    static constexpr const char *prettyName = "unsigned single-byte integer";

    static constexpr std::uint8_t zero_value = 0;
    static constexpr std::uint8_t unit_value = 1;

    static std::shared_ptr<compile::RtlType> rtlType(compile::RtlTypeFactory &t) {
      return t.intType(compile::RtlUnsigned, 8);
    }
  };

  template<>
  class TypeTraits<float> {
  public:
    static constexpr const char *name = "float";
    static constexpr const char *prettyName = "single-precision floating point";

    static constexpr float zero_value = 0;
    static constexpr float unit_value = 1;

    static std::shared_ptr<compile::RtlType> rtlType(compile::RtlTypeFactory &t) {
      return t.floatType(32);
    }
  };

  template<>
  class TypeTraits<double> {
  public:
    static constexpr const char *name = "double";
    static constexpr const char *prettyName = "double-precision floating point";

    static constexpr double zero_value = 0;
    static constexpr double unit_value = 1;

    static std::shared_ptr<compile::RtlType> rtlType(compile::RtlTypeFactory &t) {
      return t.floatType(64);
    }
  };

  template<typename T>
  class DefaultDataType : public DataType {
  public:
    virtual ~DefaultDataType() { };

    virtual const std::string name() const {
      return TypeTraits<T>::name;
    };

    virtual const std::string prettyName() const {
      return TypeTraits<T>::prettyName;
    }

    virtual void zeroValue(std::vector<std::uint8_t> &d) const {
      d.resize(size(), 0);
      encode(zero_value, d.begin());
    }

    virtual std::shared_ptr<compile::RtlType> rtlType(compile::RtlTypeFactory &fn) const {
      return TypeTraits<T>::rtlType(fn);
    }

    template<typename Iterator>
    void encode(const T &v, Iterator it) const {
      union {
        T a;
        std::uint8_t data[sizeof(T)];
      };

      // Copy
      new (&a) T(v);

      std::copy(data, data + size(), it);
    }

    static constexpr T unit_value = TypeTraits<T>::unit_value;
    static constexpr T zero_value = TypeTraits<T>::zero_value;

    virtual size_t size() const {
      return sizeof(T);
    }

    virtual bool operator==(const DataType &dt) const {
      try {
        auto &me(dynamic_cast< const DefaultDataType<T>& >(dt));
        return true;
      } catch ( std::bad_cast &e ) {
        return false;
      }
    }
  };

}

#endif
