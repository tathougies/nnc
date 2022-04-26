#include "executor.hpp"

namespace nnc {
  namespace executor {
    Executor::Executor(graph::Graph &g)
      : m_graph(g) {
    }

    Executor::~Executor() {
    }

    Invocation::Invocation(Executor &e)
      : m_executor(e) {
    }

    Invocation::~Invocation() {
    }

    void Invocation::input(Tensor &t, Tensor &value) {
      auto mName(m_executor.graph().lookupTensorName(&t));
      if ( mName )
        input(*mName, value);
      else {
        m_executor.graph().importTensor(t);
        input(t, value);
      }
    }
  }
}
