#ifndef __nnc_graph_HPP__
#define __nnc_graph_HPP__

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "tensor.hpp"
#include "memory_tensor.hpp"
#include "error_reporter.hpp"

namespace nnc {
  namespace exception {
    class TensorAlreadyExists : public std::exception {
    public:
      TensorAlreadyExists(const std::string &name);
      virtual ~TensorAlreadyExists();

      virtual const char *what() const noexcept;

      inline const std::string &tensorName() const { return m_name; }

    private:
      std::string m_name;
    };

    class InputNotReady : public std::exception {
    public:
      InputNotReady(const std::string &nodeName, const std::string &inputName);
      virtual ~InputNotReady();

      virtual const char *what() const noexcept;

      inline const std::string &nodeName() const { return m_nodeName; }
      inline const std::string &inputName() const { return m_inputName; }

    private:
      std::string m_nodeName, m_inputName;
    };
  };

  namespace graph {
    class Graph;

    class TensorRef {
    public:
      virtual ~TensorRef();

      virtual const Tensor &tensor() const =0;
      virtual Tensor &tensor() =0;

      virtual bool isInput() const;
    };

    template<typename T>
    class UniqueTensorRef : public TensorRef {
    public:
      template<typename... Args>
      UniqueTensorRef(Args &&... args)
        : m_tensor(args...) {
      }

      virtual const Tensor &tensor() const { return m_tensor; }
      virtual Tensor &tensor() { return m_tensor; }

      T &underlying() { return m_tensor; }

    private:
      T m_tensor;
    };

    template<typename datatype, int ...ds>
    class InputTensor : public Tensor, public TensorRef {
    public:
      virtual ~InputTensor() { }

      virtual const TensorShape &shape() const { return m_layout; }
      virtual const DataType &dataType() const { return m_type; }

      virtual const Tensor &tensor() const { return *this; }
      virtual Tensor &tensor() { return *this; }
      virtual bool isInput() const { return true; }

    private:
      ConstantLayout<ds...> m_layout;
      datatype m_type;
    };

    class NodeVisitor {
    public:
      virtual ~NodeVisitor();

      virtual void visit(const std::string &name, Tensor &t) =0;
    };

    class DfsVisitor : public NodeVisitor {
    public:
      DfsVisitor(Graph &g, NodeVisitor &v);
      virtual ~DfsVisitor();

      virtual void visit(const std::string &name, Tensor &t);

      inline Graph &graph() const { return m_graph; }

    private:
      Graph &m_graph;
      NodeVisitor &m_underlying;

      std::unordered_set<Tensor *> m_visited;
    };

    /**
     * A graph is a full set of operations on some set of inputs and
     * outputs.
     *
     * Arbitrary tensors can be added to the graph. Some tensors are the
     * result of operations on other tensors.
     *
     * When you add a tensor to a graph all its dependencies are added
     * as well. Thus the graph is closed.
     *
     * Certain tensors can be marked as 'outputs', which means their
     * results are germaine to the user invoking this graph.
     */
    class Graph {
    public:
      Graph(const std::string &name = "");
      ~Graph();

      inline const std::string &name() const { return m_name; }
      void name(const std::string &n);

      void visit(NodeVisitor &v) const;
      std::optional<std::string> lookupTensorName(Tensor *t) const;

      /**
       * Import an external tensor into this graph. The tensor's memory
       * is managed by the application.
       */
      void importTensor(Tensor &t, const std::string &name = "");

      /**
       * Create a new tensor in this graph whose memory is managed by
       * the graph. The tensor is destroyed when the graph is
       */
      template<typename T, typename... Args>
      T &addTensor(Args &&... args) {
        auto &it(addTensorInternal(this->nextName(),
                                   std::make_unique< UniqueTensorRef<T> > (std::forward<Args>(args)...)));
        auto &ref(dynamic_cast<UniqueTensorRef<T>&>(it));
        return ref.underlying();
      }

      template<typename T, typename... Args>
      T &addNamedTensor(const std::string &name, Args &&... args) {

        auto &it(addTensorInternal(name, std::make_unique< UniqueTensorRef<T> >(std::forward<Args>(args)...)));

        auto &ref(dynamic_cast<UniqueTensorRef<T>&>(it));

        return ref.underlying();
      }

      template<typename T, int... ds>
      Tensor &addInputTensor(const std::string &name) {
        return addTypedInputTensor<DefaultDataType<T>, ds...>(name);
      }

      template<typename T, int ...ds>
      Tensor &addTypedInputTensor(const std::string &name) {
        auto &ref(addTensorInternal(name, std::make_unique< InputTensor<T, ds...> >()));
        return ref.tensor();
      }

      /* Check the graph and return true if there were no errors */
      bool check();
      bool check(error::ErrorReporter &e);

      /* Add an output */
      void addOutput(Tensor &t);
      void addOutput(const std::string &name);
      bool isInput(Tensor &t) const;

      inline const std::unordered_set<std::string> outputs() const { return m_outputs; }

    protected:
      std::string nextName() const;

      TensorRef &addTensorInternal(const std::string &name,
                                   std::unique_ptr<TensorRef> &&ptr);
      void addReferences(Tensor &t, const std::string &name);

    private:
      std::string m_name;
      std::unordered_map<std::string, std::unique_ptr<TensorRef> > m_references;
      std::unordered_map<Tensor*, std::string> m_tensors;
      std::unordered_set<std::string> m_outputs;

      bool m_checked;

      friend class InputGraphVisitor;
    };
  }
}

#endif
