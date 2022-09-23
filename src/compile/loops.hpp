#ifndef __nnc_compile_loops_HPP__
#define __nnc_compile_loops_HPP__

#include <set>

#include "compile/rtl.hpp"

namespace nnc::compile {
  class Loop {
  public:
    RtlBlockName preamble() const;
    RtlBlockName check() const;
    RtlBlockName postamble() const;

    RtlBlockName body() const;

  private:
    Loop();

    void addBlockToBody(const RtlBlockName &blk);
    void addEntry(const RtlBlockName &blk, const RtlBlockName &to);

    /**
     * Blocks that can enter into this loop, and which block they
     * enter into
     */
    std::set<std::pair<RtlBlockName, RtlBlockName>> m_entry;

    /**
     * Blocks that execute as part of this loop
     */
    std::set<RtlBlockName> m_body;

    friend class LoopAnalysis;
  };

  class LoopAnalysis {
  public:
    LoopAnalysis(RtlFunction &fn);

    inline RtlFunction &function() const { return m_function; }
    inline const std::list<Loop> &loops() const { return m_loops; }

    void reanalyze();

  private:
    RtlFunction &m_function;
    std::list<Loop> m_loops;
    
  };
}

#endif
