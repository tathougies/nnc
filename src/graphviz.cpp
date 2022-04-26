#include "graphviz.hpp"
#include "tensor.hpp"
#include "op.hpp"

namespace nnc {
  namespace graphviz {
    class DotNodeVisitor : public op::InputVisitor {
    public:
      DotNodeVisitor(graph::Graph &g, const std::string &name, std::ostream &out, op::TensorOp &t)
        : m_graph(g), m_name(name), m_out(out), m_tensor(t) {
      }
      virtual ~DotNodeVisitor() {
      }

      virtual void input(const std::string &name, const std::string &description, bool required) {
        if ( m_tensor.hasInput(name) ) {
          auto *input(&m_tensor.input(name));
          auto mName(m_graph.lookupTensorName(input));
          if ( mName )
            m_out << "  \"" << *mName << "\" -> \"" << m_name << "\";" << std::endl;
        }
      }

    private:
      graph::Graph &m_graph;
      const std::string &m_name;
      std::ostream &m_out;
      op::TensorOp &m_tensor;
    };

    DotOutput::DotOutput(graph::Graph &g, std::ostream &out)
      : m_graph(g), m_out(out) {
      out << "digraph D {" << std::endl;;
    }

    DotOutput::~DotOutput() {
      m_out << "}" << std::endl;
    }

    void DotOutput::visit(const std::string &name, Tensor &t) {
      m_out << "  \"" << name << "\";" << std::endl;

      auto *op(dynamic_cast<op::TensorOp*>(&t));
      if ( op ) {
        DotNodeVisitor v(m_graph, name, m_out, *op);
        op->inputs(v);
      }
    }
  }
}
