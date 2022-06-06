#include "compile/x86_64/manager.hpp"
#include "arch/x86_64/schedule.hpp"
#include "compile/linear_alloc.hpp"

namespace nnc::arch::x86_64::compile {
  using namespace ::nnc::compile;
  DefaultManager::DefaultManager(const std::string &nm)
    : ::nnc::compile::DefaultManager(nm) {
  }

  DefaultManager::~DefaultManager() {
  }

  void DefaultManager::schedule(RtlFunction &fn,
                                const RtlBlockName &entry,
                                ::nnc::invoke::FunctionLibrary &lib) {
    schedule::scheduler s(fn);
    s.scheduleEntryPoint(fn.block(entry), fn.functionName());

    RegisterAllocator<LinearAllocator> regalloc(s);
    regalloc();

    lib.link(s, m_prefix);
  }
}
