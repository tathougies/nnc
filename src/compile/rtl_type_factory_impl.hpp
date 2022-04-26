#ifndef __nnc_compile_rtl_type_factory_impl_HPP__
#define __nnc_compile_rtl_type_factory_impl_HPP__

#include <memory>

namespace nnc {
  namespace compile {
    class RtlPointerType : public RtlType, public std::enable_shared_from_this<RtlPointerType> {
    public:
      RtlPointerType(std::shared_ptr<RtlType> tgt);
      virtual ~RtlPointerType();

      virtual void visit(RtlTypeVisitor &v) const;
      virtual void output(std::ostream &out) const;

      inline std::shared_ptr<RtlType> underlying() const { return m_underlying; }

    private:
      std::shared_ptr<RtlType> m_underlying;
    };

    template<typename IntType, typename FloatType>
    class RtlTypeFactoryImpl : public virtual RtlTypeFactory {
    public:
      virtual ~RtlTypeFactoryImpl() {}

      virtual std::shared_ptr<RtlType> intType(RtlIntSignedness s, RtlIntWidth w) {
        auto key(std::make_pair(s, w));
        auto found(m_intTypes.find(key));

        if ( found == m_intTypes.end() ) {
          auto next(std::make_shared<IntType>(s, w));
          m_intTypes.insert(std::make_pair(key, next));
          return next;
        } else
          return found->second;
      }


      virtual std::shared_ptr<RtlType> floatType(RtlFloatWidth w) {
        auto found(m_floatTypes.find(w));

        if ( found == m_floatTypes.end() ) {
          auto next(std::make_shared<FloatType>(w));
          m_floatTypes.insert(std::make_pair(w, next));
          return next;
        } else
          return found->second;
      }

    private:
      std::map< std::pair<RtlIntSignedness, RtlIntWidth>, std::shared_ptr<RtlType> > m_intTypes;
      std::map< RtlFloatWidth, std::shared_ptr<RtlType> > m_floatTypes;
    };
  }
}

#endif
