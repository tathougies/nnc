#include "compile/manager.hpp"

namespace nnc::compile {
  Manager::Manager(const std::string &name)
    : m_prefix(name) {
  }

  Manager::~Manager() {
  }

  RtlFunction &Manager::translate(nnc::executor::CpuExecutor &cpu, const std::string &name) {
    decltype(m_functions)::iterator it =
      m_functions.emplace(std::piecewise_construct,
                          std::forward_as_tuple(name),
                          std::forward_as_tuple(name)).first;

    nnc::executor::CpuRtlTranslator translator(it->second);
    cpu.visitAll(translator);
    translator.finish();

    m_entries.emplace(name, translator.entry()->name());

    return it->second;
  }

  DefaultManager::DefaultManager(const std::string &name)
    : Manager(name) {
  }

  DefaultManager::~DefaultManager() {
  }

  void DefaultManager::compile(nnc::invoke::FunctionLibrary &lib, nnc::compile::RtlFunction &fn) {
    decltype(m_contexts)::iterator it(m_contexts.find(fn.functionName()));
    if ( it == m_contexts.end() ) {
      it = m_contexts.emplace(std::piecewise_construct,
                              std::forward_as_tuple(fn.functionName()),
                              std::forward_as_tuple(fn)).first;
    }

    auto entry(m_entries.find(fn.functionName()));
    if ( entry == m_entries.end() )
      throw std::runtime_error("Could not find entry");

    FunctionContext &ctxt(it->second);

    ctxt.cprop1();

    // Identify loops

    // Strength reduction

    // Constant propagation


    schedule(fn, entry->second, lib);
  }

  DefaultManager::FunctionContext::FunctionContext(RtlFunction &fn)
    : cprop1(fn) {
  }
}
