#ifndef __nnc_arch_x86_64_manager_HPP__
#define __nnc_arch_x86_64_manager_HPP__

#include "compile/manager.hpp"

#include "arch/x86_64/schedule.hpp"

namespace nnc::arch::x86_64::compile {
  class DefaultManager : public ::nnc::compile::DefaultManager {
  public:
    DefaultManager(const std::string &nm = "");
    virtual ~DefaultManager();

  private:
    virtual void schedule(::nnc::compile::RtlFunction &fn,
                          const ::nnc::compile::RtlBlockName &entry,
                          ::nnc::invoke::FunctionLibrary &lib);
  };
}

#endif
