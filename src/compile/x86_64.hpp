#ifndef __nnc_compile_x86_64_HPP__
#define __nnc_compile_x86_64_HPP__

#include "compile/rtl_types.hpp"
#include "compile/rtl_type_factory_impl.hpp"

#include <memory>

namespace nnc {
  namespace compile {
    namespace x86_64 {
      class IntType : public RtlIntType, public std::enable_shared_from_this<IntType> {
      public:
        IntType(RtlIntSignedness s, RtlIntWidth w);
        virtual ~IntType();

      protected:
        virtual std::shared_ptr<RtlType> makePointer();
      };

      class FloatType : public RtlFloatType, public std::enable_shared_from_this<FloatType> {
      public:
        FloatType(RtlFloatWidth w);
        virtual ~FloatType();

      protected:
        virtual std::shared_ptr<RtlType> makePointer();
      };

      class RtlTypeFactory : public ::nnc::compile::RtlTypeFactoryImpl<IntType, FloatType> {
      public:
        virtual std::shared_ptr<RtlType> intptr();
      };
    };
  }
}

#endif
