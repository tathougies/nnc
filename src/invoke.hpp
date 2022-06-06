#ifndef __nnc_invoke_HPP__
#define __nnc_invoke_HPP__

#include <string>
#include <vector>

#include "compile/genericschedule.hpp"

namespace nnc::exception {
  class RuntimeArityMismatch : public std::exception {
  public:
    RuntimeArityMismatch(int givenArity, int expectedArity);
    virtual ~RuntimeArityMismatch();

    virtual const char *what() const noexcept;

  private:
    int m_given, m_expected;
  };

  class RuntimeArgTypeMismatch : public std::exception {
  public:
    RuntimeArgTypeMismatch(int argIx, const nnc::compile::RtlTypeInfo &expected, const std::type_info &ti);
    virtual ~RuntimeArgTypeMismatch();

    virtual const char *what() const noexcept;

  private:
    int m_argIx;
    nnc::compile::RtlTypeInfo m_expected;
    const std::type_info &m_actual;
  };
}

namespace nnc::invoke {
  class FunctionLibrary;

  typedef std::vector<nnc::compile::RtlTypeInfo> arginfotype;

  class Function {
  public:
    template<typename Res, typename... Args>
    Res invoke(Args ... args) const;

    inline std::uintptr_t base() const;

  private:
    Function(FunctionLibrary &lib, std::ptrdiff_t offs,
             const nnc::compile::RtlBasicBlock &blk);

    FunctionLibrary &m_library;
    std::ptrdiff_t m_offset;
    arginfotype m_args;

    friend class FunctionLibrary;
  };

  class FunctionLibrary {
  public:
    FunctionLibrary();
    ~FunctionLibrary();

    void link(nnc::compile::GenericFunctionScheduler &fn, const std::string &namePrefix = "");

    inline bool hasFunction(const std::string &name) const {
      return m_fns.find(name) != m_fns.end();
    }
    const Function &operator[] (const std::string &name) const;

  private:
    inline void *fnPtr(std::ptrdiff_t offs = 0) {
      return (void *) (((std::uintptr_t)m_execRegion) + offs);
    }

    void alloc(std::size_t sz);

    void *m_execRegion; // Pointer to a region of executable memory
    std::size_t m_execRegionSize, m_execRegionCapacity; // Capacity of memory is total mem available, Size is total used

    std::map<std::string, Function> m_fns;

    friend class Function;
  };

  inline std::uintptr_t Function::base() const {
    return (std::uintptr_t) m_library.fnPtr(m_offset);
  }

  template<typename... Args>
  class ArgCheck;

  template<typename Arg1, typename... Args>
  class ArgCheck<Arg1, Args...> {
  public:
    static void check(const arginfotype::const_iterator &begin,
                      const arginfotype::const_iterator &end,
                      int argIx) {
      if ( begin == end ) {
        throw exception::RuntimeArityMismatch(argIx + 1 + sizeof...(Args), argIx);
      } else {
        nnc::compile::RtlTypeInfo expected(*begin);
        if ( !nnc::compile::RtlTypeTraits<Arg1>::isCompatible(expected) ) {
          throw exception::RuntimeArgTypeMismatch(argIx, expected, typeid(Arg1));
        }

        ArgCheck<Args...>::check(begin + 1, end, argIx + 1);
      }
    }
  };

  template<>
  class ArgCheck<> {
  public:
    static void check(const arginfotype::const_iterator &begin,
                      const arginfotype::const_iterator &end,
                      int argIx) {
      if ( begin != end ) {
        int arity(argIx + std::distance(begin, end));
        throw exception::RuntimeArityMismatch(argIx, arity);
      }
    }
  };

  template<typename Res, typename... Args>
  Res Function::invoke(Args ... args) const
  {
    // Runtime type checking
    ArgCheck<Args...>::check(m_args.begin(), m_args.end(), 0);

    // Assume the function uses our calling convention
    Res (*fn)(Args... args);
    fn = (Res (*)(Args... args)) base();

    return fn(args...);
  }
}

#endif
