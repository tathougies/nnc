#ifndef __nnc_cpu_HPP__
#define __nnc_cpu_HPP__

#include <map>
#include <list>

#include "executor.hpp"
#include "executor/basic.hpp"

namespace nnc {
  namespace exception {
    class LoopIndexNotWritable : public std::exception {
    public:
      LoopIndexNotWritable(int dim);
      virtual ~LoopIndexNotWritable();

      virtual const char *what() const noexcept;

    private:
      int m_dim;
    };

    class CpuLoopIndexOutOfBounds : public std::exception {
    public:
      CpuLoopIndexOutOfBounds(int dim);
      virtual ~CpuLoopIndexOutOfBounds();

      virtual const char *what() const noexcept;

    private:
      int m_dim;
    };

    class CpuValueError : public std::exception {
    public:
      CpuValueError(const std::string &s);
      virtual ~CpuValueError();

      virtual const char *what() const noexcept;

    private:
      std::string m_message;
    };
  }

  namespace executor {
    class MemRequirement : public TensorShape {
    public:
      MemRequirement(const DataType &dt, const TensorShape &s, std::size_t batchSize);
      MemRequirement(const DataType &dt, const std::vector<int> &dims);

      virtual int dims() const;
      virtual int sizeInDimension(int dim) const;

      inline const DataType &dataType() const { return m_dataType; }

    private:
      const DataType &m_dataType;
      std::size_t m_maxSize;
      std::vector<int> m_dims;
    };

    struct CpuMemIndex {
    public:
      inline CpuMemIndex(int ix) : m_index(ix) { }

      inline int index() const { return m_index; }

    private:
      int m_index;
    };

    struct CpuTensorIndex {
    public:
      inline CpuTensorIndex(int ix) : m_index(ix) { }

      inline int index() const { return m_index; }

    private:
      int m_index;
    };

    class CpuInput {
    public:
      virtual std::ostream &print(std::ostream &out) const =0;
    };

    class CpuTensorInput : public CpuInput {
    public:
      virtual const CpuTensorIndex &index() const =0;
    };

    struct AxisStride {
    public:
      inline AxisStride(int l, int s) : length(l), stride(s) { };

      int length;
      int stride; // For each increase in this dim, how many elements do we continue forward
    };

    class MemTensor : public virtual TensorShape {
    public:
      MemTensor(const MemTensor &t);
      MemTensor(const CpuMemIndex &sourceIndex, const TensorShape &shape);

      MemTensor &operator=(const MemTensor &t);
      void swap(MemTensor &a);

      bool isContiguous() const;

      inline CpuMemIndex memoryIndex() const { return m_index; }

      AxisStride &operator[](int i);

      virtual int dims() const;
      virtual int sizeInDimension(int i) const;

    private:
      CpuMemIndex m_index;

      std::vector<AxisStride> m_stride;
    };

//    class MemFixup {
//    public:
//
//    };
    class CpuOp;

    class CpuOpVisitor {
    public:
      virtual ~CpuOpVisitor();

      virtual void op(const CpuOp &op) =0;;
      virtual void newLoop(int size) =0;
      virtual void endLoop() =0;
    };

    class CpuOp {
    public:
      virtual ~CpuOp();

      virtual void visit(CpuOpVisitor &v) const;
      virtual void print(std::ostream &out) const;
    };

    typedef std::list< std::shared_ptr<CpuOp> > CpuOpList;

    class CpuLoop {
    public:
      virtual ~CpuLoop();

      virtual const CpuOpList &ops() const =0;
      virtual void addOp(const std::shared_ptr<CpuOp> &op) =0;
      virtual void addOps(const CpuOpList &ops);

      void visitAll(CpuOpVisitor &v) const;
    };

    class CpuExecutor : public Executor, public virtual CpuLoop {
    public:
      CpuExecutor(graph::Graph &g, std::size_t batchSize = 1);
      virtual ~CpuExecutor();

      virtual std::shared_ptr<Invocation> invocation();

      void batchSize(std::size_t s);
      std::size_t batchSize() const;

      void build();

      CpuMemIndex newMemoryRequest(const DataType &dt, const TensorShape &shape);

      CpuTensorIndex newMemoryTensor(const DataType &dt, const TensorShape &shape);
      CpuTensorIndex newMemoryTensor(const CpuMemIndex &memIx);

      MemRequirement &operator[](const CpuMemIndex &ix);
      MemTensor &operator[](const CpuTensorIndex &ix);

      virtual const CpuOpList &ops() const;
      virtual void addOp(const std::shared_ptr<CpuOp> &op);

    protected:
      std::size_t m_batchSize;

      std::vector<MemRequirement> m_mem_requirements;

      std::vector<MemTensor> m_mem_tensors;

      std::map<Tensor *, CpuTensorIndex> m_tensor_map;
      //      std::vector<MemFixup> m_fixups;

      CpuOpList m_ops;

      friend class CpuTensorOps;
      friend class CpuNodeBuilder;
    };
  }
}

#endif
