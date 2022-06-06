#ifndef __nnc_compile_regalloc_HPP__
#define __nnc_compile_regalloc_HPP__

#include <memory>
#include <map>
#include <vector>
#include <iostream>

#include "compile/genericschedule.hpp"
#include "compile/rtl.hpp"

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

  template<typename Allocator>
  class RegisterAllocator {
  public:
    RegisterAllocator(GenericFunctionScheduler &s)
      : m_sched(s) {
    }

    RtlFunction &function() const { return m_sched.function(); }

    void operator() () {
      std::deque<nnc::compile::RtlBlockName> ordered(nnc::compile::orderBlocksNoLoops(m_sched));
      std::cerr << "Ordered blocks: ";
      std::transform(ordered.begin(), ordered.end(), std::ostream_iterator<int>(std::cerr, " "), [](const auto &b) { return b.index(); });

      // For each block, build up lifetimes
      for ( auto &blockNm: ordered ) {
        auto &block(m_sched.blockScheduler(blockNm));
        auto [it, inserted] =
          m_allocs.emplace(std::piecewise_construct, std::forward_as_tuple(block.destBlock()->name()),
                           std::forward_as_tuple(block));

        // For all blocks that call into us, collect the argument
        // locations. If they're the same, then automatically use that
        // register. Otherwise, apply a heuristic to choose the best
        // one (always picking a register, for now)
        std::vector<std::multimap<RtlBlockName, VirtualRegister>> argLocs;
        argLocs.resize(block.destBlock()->arity());;

        std::cerr << "Find calls for " << blockNm.index() << std::endl;
        m_sched.predecessors(&block, [blockNm, &argLocs, &block](const RtlBlockName &nm, GenericScheduler *c) {
          auto preBlock(c->destBlock());
          std::cerr << "Try block " << preBlock->name().index() << std::endl;
          // Find the jump with this name
          auto call(std::find_if(preBlock->jumps().begin(),
                                 preBlock->jumps().end(),
                                 [blockNm](const auto &j) { return j.second.to() == blockNm; }));
          if ( call == preBlock->jumps().end() )
            return;

          for ( auto [ arg, locs ] = std::tuple { call->second.arguments().begin(),
                                               argLocs.begin() };
                arg != call->second.arguments().end();
                arg++, locs++ ) {
            std::cerr << "Check call from " << c->destBlock()->name().index() << " to " << block.destBlock()->name().index() << ": ";
            auto regs(c->registers().assignment(c->destBlock()->endTime(), *arg));
            for ( const auto &reg: regs)
              locs->emplace(std::make_pair(c->destBlock()->name(), reg));

            std::copy(regs.begin(), regs.end(), std::ostream_iterator<VirtualRegister>(std::cerr, " "));
            std::cerr << std::endl;
          }
        });

        std::cerr << "For block " << blockNm.index() << ": ";
        for ( auto [it, i] = std::tuple {argLocs.begin(), 0};
              it != argLocs.end();
              ++it, ++i ) {
          std::cerr << "  Arg " << i << " (" << block.destBlock()->inputs()[i]->name() << "): ";
          for ( const auto &p: *it ) {
            std::cerr << p.second << "(from block" << p.first.index() << "), ";
          }
          std::cerr << std::endl;
        }

        for ( auto [ argLoc, input ] = std::tuple {argLocs.begin(), block.destBlock()->inputs().begin()};
              argLoc != argLocs.end();
              ++argLoc, ++input ) {
          if ( argLoc->size() == 0 ) {
            std::cerr << "No register allocated for input " << (*input)->name() << std::endl;
          } else if ( argLoc->size() == 1 ) {
            auto allocated(argLoc->begin());
            VirtualRegister vreg(allocated->second);
            block.registers().assign(-1, *input, vreg);
          } else {
            std::cerr << "Need to resolve argument contention " << (*input)->name() << std::endl;
          }
        }

        Allocator &allocator(it->second);
        allocator();
      }

      // Now we have all blocks scheduled. However, there are a few
      // things. Namely, we need to make sure to save callee saved
      // registers
    }

  private:
    GenericFunctionScheduler &m_sched;
    std::map<RtlBlockName, Allocator> m_allocs;
  };
}

#endif
