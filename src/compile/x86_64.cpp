#include "compile/x86_64.hpp"

namespace nnc {
  namespace compile {
    namespace x86_64 {
//      class X86_64_InsnSelector : public RtlInsnSelector {
//      public:
//        X86_64_InsnSelector() {
//        }
//      };

      class X86_64_PointerType : public RtlPointerType {
      public:
        X86_64_PointerType(std::shared_ptr<RtlType> tgt)
          : RtlPointerType(tgt) {
        }

        ~X86_64_PointerType() {
        }

        virtual std::size_t size() const { return 8; }

      protected:
        std::shared_ptr<RtlType> makePointer() {
          return std::make_shared<X86_64_PointerType>(std::static_pointer_cast<RtlType>(shared_from_this()));
        }
      };

      IntType::IntType(RtlIntSignedness s, RtlIntWidth w)
        : RtlIntType(s, w) {
      }

      IntType::~IntType() {
      }

      std::shared_ptr<RtlType> IntType::makePointer() {
        return std::make_shared<X86_64_PointerType>(std::static_pointer_cast<RtlType>(shared_from_this()));
      }

      std::shared_ptr<RtlType> RtlTypeFactory::intptr() {
        return intType(RtlSigned, 64);
      }

      FloatType::FloatType(RtlFloatWidth w)
        : RtlFloatType(w) {
      }

      FloatType::~FloatType() {
      }

      std::shared_ptr<RtlType> FloatType::makePointer() {
        return std::make_shared<X86_64_PointerType>(std::static_pointer_cast<RtlType>(shared_from_this()));
      }
    }
  }
}
