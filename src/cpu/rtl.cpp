#include <cassert>
#include <sstream>

#include "cpu/rtl.hpp"
#include "cpu/cpu_op.hpp"
#include "compile/dump.hpp"
#include "compile/rtl.hpp"
#include "compile/rtl_ops.hpp"
#include "compile/controlflow.hpp"
#include "compile/dataflow.hpp"
#include "compile/ssa.hpp"

namespace nnc {
  namespace exception {
    class InputTypeMismatch : public std::exception {
    public:
      InputTypeMismatch(int varIx, std::shared_ptr<compile::RtlType> act, std::shared_ptr<compile::RtlType> exp)
        : m_var(varIx),
          m_actual(act),
          m_expected(exp) {
        std::stringstream expl;
        expl << "The input " << varIx << " had already been used as a value of type " << act << ", but we expected " << exp;

        m_expl = expl.str();
      }

      virtual ~InputTypeMismatch() {}

      virtual const char *what() const noexcept {
        return m_expl.c_str();
      }

      inline int var() const { return m_var; }
      inline std::shared_ptr<compile::RtlType> actual() const { return m_actual; }
      inline std::shared_ptr<compile::RtlType> expected() const { return m_expected; }
    private:
      int m_var;
      std::shared_ptr<compile::RtlType> m_actual, m_expected;

      std::string m_expl;
    };
  }

  namespace executor {
    CpuRtlLoop::CpuRtlLoop(compile::RtlBasicBlock &head,
                           std::shared_ptr<compile::RtlVariable> ix)
      : m_head(head),
        m_index(ix) {
    }

    CpuRtlTranslator::CpuRtlTranslator(compile::RtlFunction &fn)
      : m_function(fn), m_curBlock(nullptr), m_entry(nullptr) {
    }

    CpuRtlTranslator::~CpuRtlTranslator() {
    }

    void CpuRtlTranslator::op(const CpuOp &op) {
      try {
        auto &rtlOp(dynamic_cast<const executor::CpuRtlOp&>(op));

        rtlOp.makeRtlOp(m_function, *this);
      } catch (std::bad_cast &e) {
        // TODo throw

        std::cerr << "WARNING: could not translate ";
        CpuOpPrinter p(std::cerr);
        op.visit(p);
        std::cerr << " to rtl" << std::endl;
      }
    }

    void CpuRtlTranslator::newLoop(int size) {
      // Make new loop index variable
      auto indexVar(function().variable("i",
                                        function().types().intType(compile::RtlUnsigned, 32)));
      std::uint32_t zero(0), sz(size);

      curBlock().emplace_op<compile::RtlConstantOp>(indexVar->type(), &zero, sizeof(zero), indexVar);

      auto &loopTestBlock(function().block());
      curBlock().jump(loopTestBlock.name());

      // Now implement the loop test.
      auto ltVar(function().variable("test", function().types().intType(compile::RtlUnsigned, 32)));
      auto maxVar(function().variable("test", function().types().intType(compile::RtlUnsigned, 32)));

      loopTestBlock.emplace_op<compile::RtlConstantOp>(maxVar->type(), &sz, sizeof(sz), maxVar);
      loopTestBlock.emplace_op<compile::RtlCmpOp>(compile::RtlLessThan, indexVar, maxVar, ltVar);

      auto &loopBodyBlock(function().block());
      loopTestBlock.jump(ltVar, loopBodyBlock.name());

      m_curBlock = &loopBodyBlock;

      m_loopHeads.emplace_back(loopTestBlock, indexVar);
    }

    void CpuRtlTranslator::endLoop() {
      auto &loop(m_loopHeads.back());
      m_loopHeads.pop_back();

      auto oneVar(function().variable("one", loop.index()->type()));
      std::uint32_t one(1);
      curBlock().emplace_op<compile::RtlConstantOp>(oneVar->type(), &one, sizeof(one), oneVar);
      curBlock().emplace_op<compile::RtlArithOp>(compile::RtlAdd, loop.index(), oneVar, loop.index());
      curBlock().jump(loop.head().name());

      auto &loopEndBlock(function().block());
      loop.head().jump(loopEndBlock.name());
      m_curBlock = &loopEndBlock;
    }

    compile::RtlBasicBlock &CpuRtlTranslator::curBlock() {
      if ( !m_curBlock ) {
        m_curBlock = &function().block();
      }
      if ( !m_entry )
        m_entry = m_curBlock;
      return *m_curBlock;
    }

    compile::RtlBasicBlock &CpuRtlTranslator::entry() {
      if ( !m_entry ) {
        return curBlock();
      } else
        return *m_entry;
    }

    compile::RtlBasicBlock &CpuRtlTranslator::block() {
        return curBlock();
    }

    std::shared_ptr<compile::RtlVariable> CpuRtlTranslator::tensor(const std::string &basename,
                                                                   executor::CpuInputPtr tensor,
                                                                   bool for_reading,
                                                                   bool for_writing) {
      auto memTensor(std::dynamic_pointer_cast<CpuMemoryTensor>(tensor));
      auto scalarTensor(std::dynamic_pointer_cast<CpuScalarTensor>(tensor));
      auto basicTensor(std::dynamic_pointer_cast<executor::BasicTensorInput>(tensor));

      assert(for_reading || for_writing);
      assert(basicTensor);
      assert(memTensor || scalarTensor);

      if (scalarTensor) {
        return scalarTensor->load(basename, function(), *this);
      } else if (for_reading) {
        auto var(function().variable(basename, basicTensor->dataType().rtlType(function().types())));
        memTensor->load(basename, function(), *this, var);
        return var;
      } else
        return nullptr; // Mem tensors cannot be directly written to
    }

    void
    CpuRtlTranslator::store_tensor(CpuInputPtr tensor,
                                   std::shared_ptr<compile::RtlVariable> v) {
      auto basicTensor(std::dynamic_pointer_cast<executor::BasicTensorInput>(tensor));
      auto memTensor(std::dynamic_pointer_cast<CpuMemoryTensor>(tensor));

      assert(basicTensor);
      assert(memTensor);

      memTensor->store(function(), *this, v);
    }

    std::shared_ptr<compile::RtlVariable> CpuRtlTranslator::input(const std::string &baseName, int ix, std::shared_ptr<nnc::compile::RtlType> ty) {
      auto existing(m_inputs.find(ix));
      if ( existing == m_inputs.end() ) {
        std::stringstream nm;
        nm << baseName << ix;

        auto input(entry().addInput(nm.str(),  ty));

        m_inputs.insert(std::make_pair(ix, input));

        return input;
      } else {
        auto var(existing->second);
        if ( var->type() != ty ) {
          throw exception::InputTypeMismatch(ix, var->type(), ty);
        }
        return existing->second;
      }
    }

    std::shared_ptr<compile::RtlVariable>
    CpuRtlTranslator::scalar(const std::string &baseName, int scalarIx,
                             std::shared_ptr<nnc::compile::RtlType> ty) {
      std::cerr << "Translate scalar " << scalarIx;
      auto existing(m_scalars.find(scalarIx));
      if (existing == m_inputs.end()) {
        std::stringstream nm;
        nm << baseName << scalarIx;

        auto input(m_function.variable(nm.str(), ty));
        m_scalars.insert(std::make_pair(scalarIx, input));

        std::cerr << " to " << input->name() << std::endl;
        return input;
      } else {
        std::cerr << " to " << existing->second->name() << std::endl;
        return existing->second;
      }
    }

    std::shared_ptr<compile::RtlVariable> CpuRtlTranslator::loopIndex(int dim) {
      if ( dim >= m_loopHeads.size() )
        throw exception::CpuLoopIndexOutOfBounds(dim);

      return m_loopHeads[dim].index();
    }

    void CpuRtlTranslator::finish() {
      compile::RtlBasicBlockDumper d(std::cerr);

      std::cerr << "Before SSA:" << std::endl;
      d.dump(m_function);

      // Annotate all call expressions with the arguments they need for proper control flow
      compile::ControlFlowAnalysis controlFlow(m_function);
      compile::DataflowAnalysis dataFlow(m_function);
      compile::SsaMapper aliases;

      // Now iterate through every block, and op. For each use of a
      // variable, look up the block the variable originates from. If
      // it's not this block, then we traverse the call chain of the entire
      for ( auto &block: m_function.blocks() ) {
        compile::SsaRewriter rewriter(block, aliases, controlFlow, dataFlow);
        rewriter();
      }
    }
  }
}
