#include "invoke.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>
#include <iostream>

extern "C" {
#include <sys/mman.h>
#include <unistd.h>
}

namespace nnc::exception {
  using namespace nnc::compile;

  RuntimeArityMismatch::RuntimeArityMismatch(int given, int expected)
    : m_given(given), m_expected(expected) {
  }

  RuntimeArityMismatch::~RuntimeArityMismatch() {
  }

  const char *RuntimeArityMismatch::what() const noexcept {
    return "arity mismatch";
  }

  RuntimeArgTypeMismatch::RuntimeArgTypeMismatch(int argIx, const RtlTypeInfo &expected,
                                                 const std::type_info &actual)
    : m_argIx(argIx), m_expected(expected), m_actual(actual) {
  }

  RuntimeArgTypeMismatch::~RuntimeArgTypeMismatch() {
  }

  const char *RuntimeArgTypeMismatch::what() const noexcept {
    return "arg type mismatch";
  }
}

namespace nnc::invoke {
  FunctionLibrary::FunctionLibrary()
    : m_execRegion(nullptr),
      m_execRegionSize(0),
      m_execRegionCapacity(0) {
  }

  FunctionLibrary::~FunctionLibrary() {
    if ( m_execRegion ) {
      int sts(munmap(m_execRegion, m_execRegionCapacity));
      if ( sts < 0 ) {
        std::cerr << "WARNING: could not unmap region: " << strerror(errno) << std::endl;
      }
    }
  }

  void FunctionLibrary::alloc(std::size_t sz) {
    std::size_t remaining(m_execRegionCapacity - m_execRegionSize);
    if ( remaining >= sz ) return;


    std::size_t pageSize(sysconf(_SC_PAGESIZE));
    std::size_t pageCapacity(((m_execRegionCapacity + sz + pageSize - 1)/pageSize) * pageSize);

    void *newRegion;
    if ( !m_execRegion )
      newRegion = mmap(nullptr, sz, PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    else
      newRegion = mremap(m_execRegion, m_execRegionCapacity, pageCapacity,
                         MREMAP_MAYMOVE);

    if ( newRegion == MAP_FAILED ) {
      std::stringstream err;
      err << "Could not alloc executable space: " << strerror(errno);
      throw std::runtime_error(err.str());
    }

    m_execRegion = newRegion;
    m_execRegionCapacity = pageCapacity;
  }

  void FunctionLibrary::link(nnc::compile::GenericFunctionScheduler &fn,
                             const std::string &namePrefix) {
    std::stringstream d;
    std::map<nnc::compile::RtlBlockName, std::ptrdiff_t> blockPosns;

    fn.encode(d, blockPosns);

    // Copy the function data over
    std::string data(d.str());
    alloc(data.size());

    std::uint8_t *fnBase((std::uint8_t *) fnPtr(m_execRegionSize));
    std::copy(data.begin(), data.end(),
              fnBase);

    for ( const auto &entry: fn.entryPoints() ) {
      auto position(blockPosns.find(entry.second));
      if ( position == blockPosns.end() ) continue;

      auto block(fn.function().block(entry.second));
      if ( !block ) continue;

      std::string newName(namePrefix + entry.first);
      m_fns.emplace(newName, Function(*this, m_execRegionSize + position->second, *block));
    }

    m_execRegionSize += data.size();
  }

  const Function &FunctionLibrary::operator[] (const std::string &name) const {
    auto it(m_fns.find(name));
    if ( it == m_fns.end() ) {
      throw std::runtime_error("No such function");
    } else {
      return it->second;
    }
  }

  Function::Function(FunctionLibrary &lib,
                     std::ptrdiff_t offs,
                     const nnc::compile::RtlBasicBlock &blk)
    : m_library(lib), m_offset(offs) {
    for ( const auto &input: blk.inputs() ) {
      nnc::compile::RtlTypeInfo info(*input->type());
      m_args.push_back(info);
    }
  }
}
