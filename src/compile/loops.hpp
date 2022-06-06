#ifndef __nnc_compile_loops_HPP__
#define __nnc_compile_loops_HPP__

namespace nnc::compile {
  class Loop {
  public:
    RtlBlockName preamble() const;
    RtlBlockName check() const;
    RtlBlockName postamble() const;

    RtlBlockName body() const;
  };

  class LoopAnalysis {
  public:
    LoopAnalysis(RtlFunction &fn);

    inline RtlFunction &function() const { return m_function; }

    void reanalyze();

  private:
    std::list<Loop> m_loops;
  };
}

#endif
