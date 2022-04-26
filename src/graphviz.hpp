#ifndef __nnc_graphviz_HPP__
#define __nnc_graphviz_HPP__

#include "graph.hpp"

namespace nnc {
  namespace graphviz {
    class DotOutput : public graph::NodeVisitor {
    public:
      DotOutput(graph::Graph &g, std::ostream &out);
      virtual ~DotOutput();

      virtual void visit(const std::string &name, Tensor &t);

    private:
      graph::Graph &m_graph;
      std::ostream &m_out;
    };
  }
}

#endif
