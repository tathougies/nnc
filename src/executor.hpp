#ifndef __nnc_executor_HPP__
#define __nnc_executor_HPP__

#include "graph.hpp"

namespace nnc {
  namespace executor {
    class Executor;

    class Invocation {
    public:
      Invocation(Executor &e);
      virtual ~Invocation();

      inline Executor &executor() const { return m_executor; }

      virtual void input(const std::string &name, Tensor &value) =0;
      virtual void input(Tensor &t, Tensor &value);

      virtual bool isComplete() const =0;

    private:
      Executor &m_executor;
    };

    class Executor {
    public:
      Executor(graph::Graph &g);
      virtual ~Executor();

      virtual std::shared_ptr<Invocation> invocation() =0;

      inline graph::Graph &graph() const { return m_graph; }

    private:
      graph::Graph &m_graph;
    };
  }
}

#endif
