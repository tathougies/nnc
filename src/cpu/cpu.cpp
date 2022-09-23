#include "cpu.hpp"
#include "compile/rtl_ops.hpp"
#include "cpu/rtl.hpp"
#include "cpu_op.hpp"
#include "executor/basic.hpp"
#include "op.hpp"

#include <sstream>

namespace nnc {
namespace exception {
LoopIndexNotWritable::LoopIndexNotWritable(int dim) : m_dim(dim) {}

LoopIndexNotWritable::~LoopIndexNotWritable() {}

const char *LoopIndexNotWritable::what() const noexcept {
  return "loop index not writable";
}

CpuLoopIndexOutOfBounds::CpuLoopIndexOutOfBounds(int dim) : m_dim(dim) {}

CpuLoopIndexOutOfBounds::~CpuLoopIndexOutOfBounds() {}

const char *CpuLoopIndexOutOfBounds::what() const noexcept {
  return "loop index out of bounds";
}

CpuValueError::CpuValueError(const std::string &s) : m_message(s) {}

CpuValueError::~CpuValueError() {}

const char *CpuValueError::what() const noexcept { return m_message.c_str(); }

CpuValueError
invalidCpuInput(std::shared_ptr<executor::BasicTensorInput> base) {
  std::stringstream s;
  s << "Invalid cpu input. '" << typeid(*base).name()
    << "' is not a valid CPU input";
  return CpuValueError(s.str());
}

CpuValueError indicesNotScalar(
    const std::vector<std::shared_ptr<executor::BasicTensorInput>> &ix) {
  return CpuValueError("Indices are not scalar");
}

CpuValueError
invalidCpuIndex(std::shared_ptr<executor::BasicTensorInput> base) {
  std::stringstream s;
  s << "'" << typeid(*base).name() << "' is not a valid CPU index type";
  return CpuValueError(s.str());
}

CpuValueError cpuLoopIndexOutOfBounds(int dim, std::size_t dims) {
  std::stringstream s;
  s << "Request for dimension " << dim << " in a tensor of dimension " << dims;
  return CpuValueError(s.str());
}
} // namespace exception

namespace executor {
class CpuScalarInput : public BasicTensorInput,
                       public CpuInput,
                       public CpuScalarTensor {
public:
  CpuScalarInput(CpuExecutor &e, const CpuScalarIndex &index)
      : BasicTensorInput(e[index].baseName()), m_cpu(e), m_index(index) {}

  virtual std::ostream &print(std::ostream &out) const override {
    return (out << "$" << baseName() << "_" << m_index.index());
  }

  virtual bool isScalar() const override { return true; }
  virtual int dims() const override { return 0; }
  virtual const DataType &dataType() const override {
    return m_cpu[m_index].dataType();
  }

  virtual std::shared_ptr<compile::RtlVariable> load(const std::string &basename,
                                                     compile::RtlFunction &fn, CpuRtlInputTranslator &h) override {
    return h.scalar(baseName(), m_index.index(), dataType().rtlType(fn.types()));
  }

  CpuScalarIndex scalarIndex() const { return m_index; }

private:
  CpuExecutor &m_cpu;
  CpuScalarIndex m_index;
};

class CpuTensorRefInput : public BasicTensorInput, public CpuTensorInput {
public:
  CpuTensorRefInput(CpuExecutor &e, const std::string &baseName,
                    const CpuTensorIndex &i)
      : BasicTensorInput(baseName), m_cpu(e), m_index(i) {}
  virtual ~CpuTensorRefInput() {}

  CpuExecutor &cpu() const { return m_cpu; }

  virtual const CpuTensorIndex &index() const { return m_index; }

  virtual std::ostream &print(std::ostream &out) const {
    out << "%v" << m_index.index();
    return out;
  }

  virtual bool isScalar() const { return false; }

  virtual int dims() const {
    return m_cpu[m_cpu[m_index].memoryIndex()].dims();
  }

  virtual const DataType &dataType() const {
    return m_cpu[m_cpu[m_index].memoryIndex()].dataType();
  }

protected:
  CpuExecutor &m_cpu;
  CpuTensorIndex m_index;
};

class CpuIndexedTensorRefInput : public CpuTensorRefInput,
                                 public virtual CpuMemoryTensor {
public:
  CpuIndexedTensorRefInput(const std::shared_ptr<CpuTensorRefInput> &base,
                           const std::string &baseName,
                           const std::vector<std::shared_ptr<CpuInput>> &ix)
      : CpuTensorRefInput(base->cpu(), baseName, base->index()), m_ix(ix) {}
  virtual ~CpuIndexedTensorRefInput() {}

  std::shared_ptr<compile::RtlVariable> derefTensor(compile::RtlFunction &fn,
                                                    CpuRtlInputTranslator &h) {
    auto inputPtr(h.input(baseName(), index().index(),
                          dataType().rtlType(fn.types())->pointer()));

    if (m_ix.size() == 0) {
      // This is just a scalar tensor. No need to do any indexing
      return inputPtr;
    } else {
      auto elementType(dataType().rtlType(fn.types()));
      std::uint32_t sz(elementType->size());

      auto szVar(
          fn.variable("elsz", fn.types().intType(compile::RtlUnsigned, 32)));
      h.block().emplace_op<compile::RtlConstantOp>(szVar->type(), &sz,
                                                   sizeof(sz), szVar);

      auto szVarSz(fn.variable("elsz", fn.types().intptr()));
      h.block().cast(szVar, szVarSz);

      auto inputPtrInt(fn.variable("input", fn.types().intptr()));
      h.block().cast(inputPtr, inputPtrInt);

      auto ixVar(inputPtrInt);
      MemTensor tensorLayout(m_cpu[index()]);

      for (int i = 0; i < m_ix.size(); ++i) {
        auto ref(m_ix[m_ix.size() - i - 1]);
        AxisStride &axisStride(tensorLayout[i]);
        auto dimIx(h.tensor("dimix", ref));

        std::cerr << "Accessing dim ";
        ref->print(std::cerr)
            << "(translates to " << dimIx->name() << ")" << std::endl;
        std::cerr << "Axis " << axisStride.length
                  << " stride: " << axisStride.stride << std::endl;

        auto dimIxVar(fn.variable("dimix", fn.types().intptr()));
        h.block().cast(dimIx, dimIxVar);

        std::uint32_t stride(axisStride.stride);
        auto strideVar32(fn.variable(
            "stride", fn.types().intType(compile::RtlUnsigned, 32)));
        h.block().emplace_op<compile::RtlConstantOp>(
            strideVar32->type(), &stride, sizeof(stride), strideVar32);

        auto strideVar(fn.variable("stride", fn.types().intptr()));
        h.block().cast(strideVar32, strideVar);

        auto sizedStrideVar(fn.variable("sized_stride", fn.types().intptr()));
        h.block().emplace_op<compile::RtlArithOp>(compile::RtlMul, strideVar,
                                                  szVarSz, sizedStrideVar);

        auto offsetVar(fn.variable("offset", fn.types().intptr()));
        h.block().emplace_op<compile::RtlArithOp>(
            compile::RtlMul, sizedStrideVar, dimIxVar, offsetVar);

        auto nextIxVar(fn.variable("ix", fn.types().intptr()));
        h.block().emplace_op<compile::RtlArithOp>(compile::RtlAdd, ixVar,
                                                  offsetVar, nextIxVar);

        ixVar = nextIxVar;
      }

      auto ixVarPtr(fn.variable("ptr", inputPtr->type()));
      h.block().cast(ixVar, ixVarPtr);
      return ixVarPtr;
    }
  }

  virtual void load(const std::string &basename,
                    compile::RtlFunction &fn,
                    CpuRtlInputTranslator &h,
                    std::shared_ptr<compile::RtlVariable> var) override {
      auto where(derefTensor(fn, h));
      h.block().emplace_op<compile::RtlLoadOp>(where, var);
  }

  virtual void store(compile::RtlFunction &fn, CpuRtlInputTranslator &h,
                     std::shared_ptr<compile::RtlVariable> v) {
    auto where(derefTensor(fn, h));
    h.block().emplace_op<compile::RtlStoreOp>(where, v);
  }

  virtual std::ostream &print(std::ostream &out) const {
    bool first(true);

    CpuTensorRefInput::print(out);
    out << "[";

    for (auto ref : m_ix) {
      if (!first)
        out << ", ";

      ref->print(out);

      first = false;
    }

    out << "]";

    return out;
  }

  virtual bool isScalar() const { return true; }
  virtual int dims() const { return 0; }

private:
  std::vector<std::shared_ptr<CpuInput>> m_ix;
};

class CpuLoopIndexInput : public BasicTensorInput,
                          public CpuInput,
                          public virtual CpuScalarTensor {
public:
  CpuLoopIndexInput(int dim) : BasicTensorInput("dim"), m_dim(dim) {}
  virtual ~CpuLoopIndexInput() {}

  virtual std::ostream &print(std::ostream &out) const {
    out << "$i" << m_dim;
    return out;
  }

  virtual bool isScalar() const { return true; }
  virtual int dims() const { return 0; }
  virtual const DataType &dataType() const { return m_dataType; }

  virtual std::shared_ptr<compile::RtlVariable> load(const std::string &basename,
                                                     compile::RtlFunction &fn, CpuRtlInputTranslator &h) {
    return h.loopIndex(m_dim);
  }

private:
  int m_dim;

  static DefaultDataType<std::uint32_t> m_dataType;
};

DefaultDataType<std::uint32_t> CpuLoopIndexInput::m_dataType;

class CpuTensorOps : public BasicTensorOps,
                     public virtual CpuLoop,
                     public std::enable_shared_from_this<CpuTensorOps> {
public:
  CpuTensorOps(CpuExecutor &cpu, op::TensorOp &op)
      : m_cpu(cpu), m_outer(nullptr), m_op(op) {}
  CpuTensorOps(std::shared_ptr<CpuTensorOps> base, int nextLoop)
      : m_cpu(base->m_cpu), m_outer(base), m_op(base->m_op),
        m_loop(base->m_loop) {
    m_loop.push_back(std::make_pair(
        std::make_shared<CpuLoopIndexInput>(m_loop.size()), nextLoop));
  }

  virtual ~CpuTensorOps() {
    if (m_loop.empty()) {
      if (m_outer)
        m_outer->addOps(m_ops);
      else
        m_cpu.addOps(m_ops);
    } else {
      if (!ops().empty())
        m_outer->addOp(
            std::make_shared<CpuLoopOp>(m_loop.back().second, ops()));
    }
  }

  virtual std::shared_ptr<BasicTensorInput>
  input(const std::string &name) const {
    auto &tensorInput(m_op.input(name));
    auto tensorIt(m_cpu.m_tensor_map.find(&tensorInput));

    if (tensorIt == m_cpu.m_tensor_map.end())
      throw exception::OperationInputNotReady(name);

    return std::make_shared<CpuTensorRefInput>(m_cpu, name,
                                               tensorIt->second.index());
  }

  virtual std::shared_ptr<BasicTensorInput>
  index(const std::shared_ptr<BasicTensorInput> &base,
        const std::vector<std::shared_ptr<BasicTensorInput>> &ix) const {
    auto allScalar(std::all_of(ix.begin(), ix.end(),
                               [](const std::shared_ptr<BasicTensorInput> &ix) {
                                 return ix->isScalar();
                               }));

    if (!allScalar)
      throw exception::indicesNotScalar(ix);

    auto cpuBase(std::dynamic_pointer_cast<CpuTensorRefInput>(base));
    if (!cpuBase)
      throw exception::invalidCpuInput(base);

    std::vector<std::shared_ptr<CpuInput>> cpuIxs;

    std::transform(ix.begin(), ix.end(), std::back_inserter(cpuIxs),
                   [](const std::shared_ptr<BasicTensorInput> &i) {
                     auto cpuIx(std::dynamic_pointer_cast<CpuInput>(i));
                     if (!cpuIx)
                       throw exception::invalidCpuIndex(i);

                     return cpuIx;
                   });

    return std::make_shared<CpuIndexedTensorRefInput>(cpuBase, base->baseName(),
                                                      cpuIxs);
  }

  virtual BasicTensorInputPtr tensor(const DataType &dt,
                                     const TensorShape &sh) const {
    auto tensorIx(m_cpu.newMemoryTensor(dt, sh));
    return std::make_shared<CpuTensorRefInput>(m_cpu, "temp", tensorIx);
  }

  virtual BasicTensorInputPtr scalar(const std::string &basename,
                                     const DataType &dt) const {
    return std::make_shared<CpuScalarInput>(m_cpu,
                                            m_cpu.newScalar(basename, dt));
  }

  virtual std::shared_ptr<BasicTensorOps> loop(int size) {
    return std::make_shared<CpuTensorOps>(shared_from_this(), size);
  }

  virtual std::shared_ptr<BasicTensorOps> outerLoop() {
    return shared_from_this();
  }

  virtual int loopCount() const { return m_loop.size(); }

  virtual std::shared_ptr<BasicTensorInput> loopIndex(int dim) const {
    if (dim < m_loop.size() && dim >= 0) {
      return m_loop[dim].first;
    } else if (dim < 0) {
      int ix(m_loop.size() + dim);
      if (ix < 0) {
        throw exception::cpuLoopIndexOutOfBounds(dim, m_loop.size());
      }
      return m_loop[ix].first;
    } else
      throw exception::cpuLoopIndexOutOfBounds(dim, m_loop.size());
  }

  virtual void cast(const DataType &from, const DataType &to,
                    const std::shared_ptr<BasicTensorInput> &src,
                    const std::shared_ptr<BasicTensorInput> &destp) {
    std::shared_ptr<CpuInput> ref(std::dynamic_pointer_cast<CpuInput>(src));
    std::shared_ptr<CpuInput> dest(std::dynamic_pointer_cast<CpuInput>(destp));

    addOp(std::make_shared<CpuCastOp>(from, to, ref, dest));
  }

  virtual void constant(void *d, std::size_t sz,
                        const std::shared_ptr<BasicTensorInput> &destp) {
    auto dest(std::dynamic_pointer_cast<CpuInput>(destp));
    addOp(std::make_shared<CpuConstantOp>(dest->dataType(), d, sz, dest));
  }

  virtual void reshape(const TensorShape &shape, const BasicTensorInputPtr &ap,
                       const BasicTensorInputPtr &destp) {
    std::shared_ptr<CpuTensorRefInput> a(
        std::dynamic_pointer_cast<CpuTensorRefInput>(ap));
    std::shared_ptr<CpuTensorRefInput> dest(
        std::dynamic_pointer_cast<CpuTensorRefInput>(destp));

    const auto &tensor(m_cpu[dest->index()]);

    // If the tensor is contiguous and in the expected order, then
    // just reuse the memory
    if (tensor.isContiguous()) {
      // Copy not required, re-use memory
      m_cpu[dest->index()] =
          MemTensor(tensor.memoryIndex(), m_cpu.batchSize(), shape);
    } else {
      // Copy required
      throw exception::NotImplemented("Copying reshaped tensor");
    }
  }

  virtual void add(const BasicTensorInputPtr &ap, const BasicTensorInputPtr &bp,
                   const BasicTensorInputPtr &destp) {
    std::shared_ptr<CpuInput> a(std::dynamic_pointer_cast<CpuInput>(ap));
    std::shared_ptr<CpuInput> b(std::dynamic_pointer_cast<CpuInput>(bp));
    std::shared_ptr<CpuInput> dest(std::dynamic_pointer_cast<CpuInput>(destp));

    addOp(std::make_shared<CpuAddOp>(dest, a, b));
  }

  virtual void product(const BasicTensorInputPtr &ap,
                       const BasicTensorInputPtr &bp,
                       const BasicTensorInputPtr &destp) {
    std::shared_ptr<CpuInput> a(std::dynamic_pointer_cast<CpuInput>(ap));
    std::shared_ptr<CpuInput> b(std::dynamic_pointer_cast<CpuInput>(bp));
    std::shared_ptr<CpuInput> dest(std::dynamic_pointer_cast<CpuInput>(destp));

    addOp(std::make_shared<CpuMulOp>(dest, a, b));
  }

  virtual const CpuOpList &ops() const { return m_ops; }

  virtual void addOp(const std::shared_ptr<CpuOp> &op) { m_ops.push_back(op); }

private:
  CpuExecutor &m_cpu;
  std::shared_ptr<CpuLoop> m_outer;
  op::TensorOp &m_op;

  std::vector<std::pair<std::shared_ptr<BasicTensorInput>, int>> m_loop;

  CpuOpList m_ops;
};

class CpuNodeBuilder : public graph::NodeVisitor {
public:
  CpuNodeBuilder(CpuExecutor &g) : m_cpu(g) {}

  virtual ~CpuNodeBuilder() {}

  virtual void visit(const std::string &name, Tensor &t) {
    std::cerr << "Compiling " << name << std::endl;

    std::size_t totalSize(t.shape().totalSize() * t.dataType().size() *
                          m_cpu.batchSize());
    std::cerr << "Allocating tensor of size " << totalSize << std::endl;

    // Request memory for the given tensor
    auto memReq(m_cpu.newMemoryRequest(t.dataType(), t.shape()));

    try {
      op::TensorOp &op(dynamic_cast<op::TensorOp &>(t));
      std::cerr << "This tensor is an operation" << std::endl;

      // Attempt to do the operation

      // Lookup operation inputs
      // Attempt to compile the operation...

      auto inputs(std::make_shared<CpuTensorOps>(m_cpu, op));
      auto mainLoop(inputs->loop(m_cpu.batchSize()));
      std::shared_ptr<BasicTensorInput> tensorInput;

      // Some operations are CPU specific

      // Some operations are basic operations.
      auto basicOp(dynamic_cast<BasicTensorOp *>(&op));
      if (basicOp) {
        CpuTensorIndex tensorIx(m_cpu.newMemoryTensor(memReq));

        auto dest(std::make_shared<CpuTensorRefInput>(m_cpu, "dest", tensorIx));
        basicOp->compile(*mainLoop, dest);
        ;

        tensorInput = dest;
        goto done;
      }

      std::cerr << "Can't compile" << std::endl;
      return;

    done:
      if (tensorInput) {
        auto cpuInput(
            std::dynamic_pointer_cast<CpuTensorRefInput>(tensorInput));
        if (cpuInput)
          m_cpu.m_tensor_map.insert(std::make_pair(&t, cpuInput->index()));
      }

      return;
    } catch (std::bad_cast &e) {
      std::cerr << "This tensor is not an operation" << std::endl;

      m_cpu.m_tensor_map.insert(
          std::make_pair(&t, m_cpu.newMemoryTensor(memReq)));
    }
  }

private:
  CpuExecutor &m_cpu;
};

MemTensor::MemTensor(const MemTensor &t)
    : m_index(t.m_index), m_batchSize(t.m_batchSize), m_stride(t.m_stride) {}

MemTensor::MemTensor(const CpuMemIndex &sourceIndex, std::size_t batchSize,
                     const TensorShape &shape)
    : m_index(sourceIndex), m_batchSize(batchSize) {
  int stride = 1;

  for (int i = 0; i < shape.dims(); ++i) {
    int len(shape.sizeInDimension(i));
    m_stride.emplace_back(AxisStride(len, stride));
    stride *= len;
  }
  m_stride.emplace_back(AxisStride(batchSize, stride));
}

void MemTensor::swap(MemTensor &a) {
  std::swap(m_index, a.m_index);
  std::swap(m_batchSize, a.m_batchSize);
  std::swap(m_stride, a.m_stride);
}

MemTensor &MemTensor::operator=(const MemTensor &t) {
  MemTensor c(t);
  swap(c);
  return *this;
}

AxisStride &MemTensor::operator[](int i) {
  std::cerr << "Attempting to access dimension " << i
            << " of tensor layout of dimension " << m_stride.size()
            << std::endl;
  return m_stride[i];
}

bool MemTensor::isContiguous() const {
  int stride = 1;

  for (int i = 0; i < m_stride.size(); ++i) {
    if (stride != m_stride[i].stride)
      return false;
    stride *= m_stride[i].length;
  }

  return true;
}

int MemTensor::dims() const { return m_stride.size() + 1; }

int MemTensor::sizeInDimension(int d) const {
  if (d < dims())
    return m_stride[d].length;
  else
    throw exception::InvalidDimension(d, dims());
}

CpuLoop::~CpuLoop() {}

void CpuLoop::addOps(const CpuOpList &ops) {
  for (auto &op : ops)
    addOp(op);
}

void CpuLoop::visitAll(CpuOpVisitor &v) const {
  for (const auto &op : ops()) {
    op->visit(v);
  }
}

CpuExecutor::CpuExecutor(graph::Graph &g, std::size_t batchSize)
    : Executor(g), m_batchSize(batchSize) {}

CpuExecutor::~CpuExecutor() {}

std::shared_ptr<Invocation> CpuExecutor::invocation() { return nullptr; }

void CpuExecutor::build() {
  // For each output tensor, we attempt to just build it without storing it

  // TODO only do this for output tensors

  CpuNodeBuilder builder(*this);
  graph::DfsVisitor visitor(graph(), builder);
  graph().visit(visitor);
}

std::size_t CpuExecutor::batchSize() const { return m_batchSize; }

MemRequirement &CpuExecutor::operator[](const CpuMemIndex &ix) {
  return m_mem_requirements[ix.index()];
}

MemTensor &CpuExecutor::operator[](const CpuTensorIndex &ix) {
  return m_mem_tensors[ix.index()];
}

Scalar &CpuExecutor::operator[](const CpuScalarIndex &ix) {
  return m_scalars[ix.index()];
}

CpuMemIndex CpuExecutor::newMemoryRequest(const DataType &dt,
                                          const TensorShape &shp) {
  m_mem_requirements.emplace_back(dt, shp, batchSize());
  return CpuMemIndex(m_mem_requirements.size() - 1);
}

CpuTensorIndex CpuExecutor::newMemoryTensor(const DataType &dt,
                                            const TensorShape &shp) {
  CpuMemIndex ix(newMemoryRequest(dt, shp));
  return newMemoryTensor(ix);
}

CpuTensorIndex CpuExecutor::newMemoryTensor(const CpuMemIndex &memIx) {
  auto &req(m_mem_requirements[memIx.index()]);
  m_mem_tensors.emplace_back(memIx, m_batchSize, req);
  return CpuTensorIndex(m_mem_tensors.size() - 1);
}

CpuScalarIndex CpuExecutor::newScalar(const std::string &basename,
                                      const DataType &dt) {
  m_scalars.emplace_back(basename, dt);
  return CpuScalarIndex(m_scalars.size() - 1);
}

const CpuOpList &CpuExecutor::ops() const { return m_ops; }

void CpuExecutor::addOp(const std::shared_ptr<CpuOp> &op) {
  m_ops.push_back(op);
}

// Scalar
Scalar::Scalar(const std::string &baseName, const DataType &dt)
    : m_baseName(baseName), m_dataType(dt) {}

// Mem requirement
MemRequirement::MemRequirement(const DataType &dt, const TensorShape &shp,
                               std::size_t batchSize)
    : m_dataType(dt), m_maxSize(dt.size() * shp.totalSize() * batchSize),
      m_dims(shp.asVector()) {}

int MemRequirement::dims() const { return m_dims.size(); }

int MemRequirement::sizeInDimension(int dim) const { return m_dims[dim]; }
} // namespace executor
} // namespace nnc

//  [stride0 0] [1 0 offset0]  [ix0]
//  [0 stride1] [0 1 offset1]  [ix1]
//                             [ 1 ]
//
//  2 x 2 ... 2x3
//
//  [stride0 0       (offset0 * stride0)] [ix0]
//  [0       stride1 (offset1 * stride1)] [ix1]
//                                        [ 1 ]
