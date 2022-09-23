#ifndef __nnc_compile_regalloc_HPP__
#define __nnc_compile_regalloc_HPP__

#include <memory>
#include <map>
#include <vector>
#include <iostream>

#include "compile/dump.hpp"
#include "compile/regset.hpp"
#include "compile/registers.hpp"
#include "compile/rtl.hpp"
#include "compile/controlflow.hpp"

namespace nnc::compile {
  class RegClassDeclarer {
  public:
    virtual void regclass(const std::string &argNm, const RtlVariablePtr &var, const RegClass &cls) =0;

    // Declare that var1 and var2 must have the same register in common
    virtual void intersect(const RtlVariablePtr &var1, const RtlVariablePtr &var2, const RegClass &cls) =0;
  };

  /* class for ops that need registers to work */
  class RtlRegisterOp {
  public:
    virtual void regclasses(RegClassDeclarer &decl) const =0;
  };

  class RegisterAllocatorBase {
  public:
    RegisterAllocatorBase(RtlFunction &s, RegisterFile &regs);
    virtual ~RegisterAllocatorBase();

    inline RtlFunction &function() const { return m_function; }
    inline RegisterFile &registerFile() const { return m_registers; }

    virtual RegisterTracker &registersFor(const RtlBlockName &nm) =0;

  private:
    RtlFunction &m_function;
    RegisterFile &m_registers;
  };

  class RtlRegisterBlockDumper : public RtlBasicBlockDumper {
  public:
    RtlRegisterBlockDumper(RegisterAllocatorBase &regs,
                           std::ostream &out);
    virtual ~RtlRegisterBlockDumper();

    virtual void dumpOp(RtlFunction &fn, const RtlBasicBlock *block, RtlOp &op, int index);

  protected:
    virtual void dumpVar(const RtlBasicBlock *from, int opix, const RtlVariablePtr &var) override;

  private:
    RegisterAllocatorBase &m_registers;
  };

  template<typename Allocator>
  class RegisterAllocator : public RegisterAllocatorBase {
  public:
    RegisterAllocator(RtlFunction &s, RegisterFile &regs)
      : RegisterAllocatorBase(s, regs) {
    }

    virtual RegisterTracker &registersFor(const RtlBlockName &nm) override {
      return allocatorFor(nm).tracker();
    }

    Allocator &allocatorFor(const RtlBasicBlock &block) {
      return allocatorFor(block.name());
    }

    Allocator &allocatorFor(const RtlBlockName &nm) {
      auto it(m_allocs.find(nm));
      if ( it == m_allocs.end() ) {
        std::tie(it, std::ignore) =
          m_allocs.emplace(std::piecewise_construct, std::forward_as_tuple(nm),
                           std::forward_as_tuple(function().block(nm), registerFile()));
      }
      return it->second;
    }

    void operator() () {
      ControlFlowAnalysis cfa(function());

      // For each block, build up lifetimes
      for ( auto &block: function().bfs() ) {
        std::cerr << "Allocating block" << block.name().index() << std::endl;
        Allocator &allocator(allocatorFor(block));

        resolveBlockArgLocations(block, cfa, allocator.tracker());

        allocator();
      }

      // Now we have all blocks scheduled. However, there are a few
      // things. Namely, we need to make sure to save callee saved
      // registers
    }

  protected:
    void resolveBlockArgLocations(RtlBasicBlock &block, const ControlFlowAnalysis &cfa,
                                  RegisterTracker &tracker) {
        // For all blocks that call into us, collect the argument
        // locations. If they're the same, then automatically use that
        // register. Otherwise, apply a heuristic to choose the best
        // one (always picking a register, for now)
      //        std::vector<std::multimap<RtlBlockName, VirtualRegister>> argLocs;
        std::vector<std::map<VirtualRegister, int>> argLocs;
        argLocs.resize(block.arity());

        std::vector<RtlBlockName> preds;
        cfa.findPredecessors(block.name(), preds);

        std::cerr << "Find calls for " << block.name().index() << std::endl;
        for ( const RtlBlockName &nm: preds ) {
          RtlBasicBlock &preBlock(function().block(nm));

          std::cerr << "Try block " << preBlock.name().index() << std::endl;
          // Find the jump with this name
          auto &call(preBlock.get_jump_to(block.name()));

          for ( auto [ arg, locs ] = std::tuple { call.arguments().begin(),
                                               argLocs.begin() };
                arg != call.arguments().end();
                arg++, locs++ ) {
            std::cerr << "Check call from " << preBlock.name().index() << " to " << block.name().index() << "(" <<
              (*arg)->name() << "): ";
            auto regs(allocatorFor(preBlock).tracker().assignment(preBlock.endTime(), *arg));
            for (const auto &reg : regs) {
              auto locCounter(locs->find(reg));
              if ( locCounter == locs->end() )
                locCounter = locs->emplace(reg, 0).first;

              locCounter->second++;
            }

            std::copy(regs.begin(), regs.end(), std::ostream_iterator<VirtualRegister>(std::cerr, " "));
            std::cerr << std::endl;
          }
        }

        for ( auto [ argLoc, input ] = std::tuple {argLocs.begin(), block.inputs().begin()};
              argLoc != argLocs.end();
              ++argLoc, ++input ) {
          auto it(std::max_element(argLoc->begin(), argLoc->end(),
                                   [](const auto &a, const auto &b) {
                                     return a.second < b.second; }));
          if ( it == argLoc->end() )
            std::cerr << "No register allocated for input " << (*input)->name() << std::endl;
          else {
            std::cerr << "Assigning " << it->first << " to input " << (*input)->name() << std::endl;
            tracker.assign(-1, *input, it->first);
          }
        }

    }

  private:
    std::map<RtlBlockName, Allocator> m_allocs;
  };
}

#endif
