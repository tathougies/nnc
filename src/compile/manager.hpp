#ifndef __nnc_compile_manager_HPP__
#define __nnc_compile_manager_HPP__

#include "cpu.hpp"
#include "invoke.hpp"
#include "compile/rtl.hpp"
#include "compile/constant_propagation.hpp"
#include "cpu/rtl.hpp"

namespace nnc::compile {
  class Manager {
  public:
    Manager(const std::string &name = "");
    virtual ~Manager();

    RtlFunction &translate(nnc::executor::CpuExecutor &cpu, const std::string &name);

    virtual void compile(nnc::invoke::FunctionLibrary &lib, RtlFunction &fn) =0;

  protected:
    std::string m_prefix;

    std::map<std::string, RtlFunction> m_functions;
  };

  class DefaultManager : public Manager {
  public:
    DefaultManager(const std::string &name = "");
    virtual ~DefaultManager();

    virtual void compile(nnc::invoke::FunctionLibrary &lib, RtlFunction &fn);

  protected:
    virtual void schedule(RtlFunction &fn,
                          nnc::invoke::FunctionLibrary &lib) =0;

  private:
    struct FunctionContext {
      FunctionContext(RtlFunction &fn);

      nnc::compile::ConstantPropagation cprop1;
    };
    std::map<std::string, FunctionContext> m_contexts;
  };
}

#endif
