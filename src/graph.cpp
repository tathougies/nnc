#include "graph.hpp"
#include "error_reporter.hpp"
#include "op.hpp"

#include <sstream>

namespace nnc {
  namespace exception {
    TensorAlreadyExists::TensorAlreadyExists(const std::string &name)
      : m_name(name) {
    }

    TensorAlreadyExists::~TensorAlreadyExists() {
    }

    const char *TensorAlreadyExists::what() const noexcept {
      return "TensorAlreadyExists";
    }

    InputNotReady::InputNotReady(const std::string &nodeName, const std::string &inputName)
      : m_nodeName(nodeName), m_inputName(inputName) {
    }

    InputNotReady::~InputNotReady() {
    }

    const char *InputNotReady::what() const noexcept {
      return "Input not ready";
    }
  }

  namespace graph {
    class UnmanagedTensorRef : public TensorRef {
    public:
      UnmanagedTensorRef(Tensor &t)
        : m_tensor(t) {
      }
      virtual ~UnmanagedTensorRef() {}

      virtual const Tensor &tensor() const { return m_tensor; }
      virtual Tensor &tensor() { return m_tensor; }

    private:
      Tensor &m_tensor;
    };

    class InputGraphVisitor : public op::InputVisitor {
    public:
      InputGraphVisitor(Graph &g, op::TensorOp &op, const std::string &nodeName)
        : m_graph(g),
          m_op(op),
          m_nodeName(nodeName) {
      }

      virtual ~InputGraphVisitor() { }

      virtual void input(const std::string &name, const std::string &description, bool required) {
        // Can't add operation if it's not complete
        bool hasInput(m_op.hasInput(name));

        if ( required & !hasInput )
          throw exception::InputNotReady(m_nodeName, name);

        if ( !hasInput ) return;

        auto &tensor(m_op.input(name));
        auto existing(m_graph.m_tensors.find(&tensor));
        if ( existing == m_graph.m_tensors.end() ) {
          // Add the node to the graph
          m_graph.addTensorInternal(m_graph.nextName(), std::make_unique<UnmanagedTensorRef>(tensor));
        }
      }

    private:
      Graph &m_graph;
      op::TensorOp &m_op;
      const std::string &m_nodeName;
    };

    class CheckGraphVisitor : public NodeVisitor {
    public:
      CheckGraphVisitor(Graph &g, error::ErrorReporter &e);
      virtual ~CheckGraphVisitor() { }

      virtual void visit(const std::string &name, Tensor &t);

    private:
      Graph &m_graph;
      error::ErrorReporter &m_errors;

      std::unordered_set<Tensor *> m_checked;
    };

    class InputCheckVisitor : public op::InputVisitor {
    public:
      InputCheckVisitor(Graph &u, CheckGraphVisitor &graphChecker, op::TensorOp *op)
        : m_underlying(u), m_graph(graphChecker), m_op(*op) {

      }
      virtual ~InputCheckVisitor() { }

      virtual void input(const std::string &name, const std::string &description, bool required) {
        // This is only run on nodes inside a graph, so they are already checked for being complete
        if ( required || m_op.hasInput(name) ) {
          auto &input(m_op.input(name));
          const auto &mName(m_underlying.lookupTensorName(&input));

          if ( mName )
            m_graph.visit(*mName, input);
        }
      }

    private:
      Graph &m_underlying;
      CheckGraphVisitor &m_graph;
      op::TensorOp &m_op;
    };

    CheckGraphVisitor::CheckGraphVisitor(Graph &g, error::ErrorReporter &e)
      : m_graph(g), m_errors(e) {
    }

    void CheckGraphVisitor::visit(const std::string &name, Tensor &t) {
      if ( m_checked.find(&t) == m_checked.end() ) {
        op::TensorOp *op(dynamic_cast<op::TensorOp *>(&t));

        if ( op ) { // TODO report node name
          InputCheckVisitor checker(m_graph, *this, op);
          op->inputs(checker);
          op->check(m_errors);
        }

        m_checked.insert(&t);
      }
    }

    class DfsInputVisitor : public op::InputVisitor {
    public:
      DfsInputVisitor(DfsVisitor &v, op::TensorOp &op)
        : m_visitor(v), m_op(op) {
      }
      virtual ~DfsInputVisitor() { }

      void input(const std::string &name, const std::string &description, bool required) {
        if ( !required && !m_op.hasInput(name) ) return;

        auto *input(&m_op.input(name));
        auto mName(m_visitor.graph().lookupTensorName(input));
        if ( mName ) {
          m_visitor.visit(*mName, *input);
        }
      }

    private:
      DfsVisitor &m_visitor;
      op::TensorOp &m_op;
    };

    TensorRef::~TensorRef() {
    }

    bool TensorRef::isInput() const {
      return false;
    }

    NodeVisitor::~NodeVisitor() {
    }

    DfsVisitor::DfsVisitor(Graph &g, NodeVisitor &v)
      : m_graph(g), m_underlying(v) {
    }

    DfsVisitor::~DfsVisitor() {
    }

    void DfsVisitor::visit(const std::string &name, Tensor &t) {
      if ( m_visited.find(&t) == m_visited.end() ) {
        m_visited.insert(&t);

        // If it's an op, make sure to visit the input nodes first
        op::TensorOp *op(dynamic_cast<op::TensorOp *>(&t));
        if ( op ) {
          DfsInputVisitor dfs(*this, *op);
          op->inputs(dfs);
        }

        m_underlying.visit(name, t);
      }
    }

    class ExternalTensorRef : public TensorRef {
    public:
      ExternalTensorRef(Tensor &t)
        : m_tensor(t) {
      }

      virtual ~ExternalTensorRef() { }

      virtual const Tensor &tensor() const { return m_tensor; }
      virtual Tensor &tensor() { return m_tensor; }

    private:
      Tensor &m_tensor;
    };

    Graph::Graph(const std::string &name)
      : m_name(name), m_checked(false) {
    }

    Graph::~Graph() {
    }

    std::string Graph::nextName() const {
      std::stringstream s;
      s << "node_" << m_references.size();
      return s.str();
    }

    void Graph::addReferences(Tensor &t, const std::string &name) {
      auto it(m_tensors.find(&t));
      if ( it == m_tensors.end() ) {
        // This tensor has not been added yet, do so now
        m_tensors.insert(std::make_pair(&t, name));

        // Now, walk the reference tree, if this is an operation
        op::TensorOp *op(dynamic_cast<op::TensorOp *>(&t));
        if ( !op ) return;

        // Otherwise, we're an op, so traverse all inputs
        InputGraphVisitor visitor(*this, *op, name);
        op->inputs(visitor);
      }
    }

    void Graph::importTensor(Tensor &t, const std::string &name) {
      if ( name.size() == 0 ) {
        importTensor(t, nextName());
      } else {
        addTensorInternal(name, std::make_unique<ExternalTensorRef>(t));
      }
    }

    TensorRef &Graph::addTensorInternal(const std::string &name,
                                        std::unique_ptr<TensorRef> &&ptr) {
      auto i(m_references.find(name));
      if ( i != m_references.end() )
        throw exception::TensorAlreadyExists(name);

      auto it(m_references.insert(std::make_pair(name, std::move(ptr))));
      // Now add all tensors this refers to
      addReferences(it.first->second->tensor(), name);

      return *it.first->second;
    }

    bool Graph::check() {
      error::NullErrorReporter e;
      return check(e);
    }

    bool Graph::check(error::ErrorReporter &errors) {
      error::SummaryErrorReporter summary;
      error::TeeErrorReporter tee(errors, summary);

      CheckGraphVisitor visitor(*this, tee);
      visit(visitor);

      return summary.errors() == 0;
    }

    void Graph::addOutput(Tensor &t) {
      auto mName(lookupTensorName(&t));
      if ( mName )
        addOutput(*mName);
      else {
        importTensor(t);
        addOutput(t);
      }
    }

    void Graph::addOutput(const std::string &name) {
      m_outputs.insert(name);
    }

    bool Graph::isInput(Tensor &t) const {
      auto mName(lookupTensorName(&t));
      if ( mName ) {
        auto ref(m_references.find(*mName));
        if ( ref == m_references.end() ) return false;

        return ref->second->isInput();
      } else
        return false;
    }

    void Graph::visit(NodeVisitor &v) const {
      // Visit every single node in this graph
      for ( const auto &tensor : m_references )
        v.visit(tensor.first, tensor.second->tensor());
    }

    std::optional<std::string> Graph::lookupTensorName(Tensor *t) const {
      const auto &it(m_tensors.find(t));
      if ( it == m_tensors.end() ) return std::optional<std::string> ();
      else return std::make_optional(it->second);
    }
  }
}
