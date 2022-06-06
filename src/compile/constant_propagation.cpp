#include "compile/constant_propagation.hpp"
#include "compile/unused.hpp"
#include "compile/rtl_ops.hpp"

#include <iostream>
#include <set>

namespace nnc::exception {
  class VariableNotConstant : public std::exception {
  public:
    VariableNotConstant(std::shared_ptr<compile::RtlVariable> v)
      : m_var(v) {
    }
    virtual ~VariableNotConstant() { }

    virtual const char *what() const noexcept { return "variable not constant"; }

    inline std::shared_ptr<compile::RtlVariable> var() const { return m_var;}

  private:
    std::shared_ptr<compile::RtlVariable> m_var;
  };
}

namespace nnc::compile {
  class OpOutputChecker : public RtlOperandVisitor {
  public:
    OpOutputChecker(const ConstantPropagation &p)
      : RtlOperandVisitor(p.function())
      , m_constants(static_cast<const IConstantPropagator &>(p))
      , m_allOutputs(true) {
    }

    virtual ~OpOutputChecker() {
    }

    inline bool allOutputsWritten() const { return m_allOutputs; }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
      if ( !allOutputsWritten() ) return;

      bool isConstant(m_constants.isConstant(var));
      if ( output ) {
        if ( isConstant ) {
          auto newVar(function().variable(var->namePrefix(), var->type()));
          m_redundantOutputs.insert(std::make_pair(name, newVar));
        } else {
          m_allOutputs = false;
        }
      } else if ( input && isConstant ) {
        const PropagatedConstant &c(m_constants.getConstant(var));
        m_constantInputs.emplace(std::piecewise_construct, std::forward_as_tuple(var), std::forward_as_tuple(c));
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType>, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &nm) {
    }

  private:
    const IConstantPropagator &m_constants;
    bool m_allOutputs;
    std::map<std::string, std::shared_ptr<RtlVariable>> m_redundantOutputs;
    std::map<std::shared_ptr<RtlVariable>, const PropagatedConstant &> m_constantInputs;

  public:
    inline const decltype(m_redundantOutputs) &redundantOutputs() const { return m_redundantOutputs; }
    inline const decltype(m_constantInputs) &constantInputs() const { return m_constantInputs; }
  };

  class ConstantPropagator : public IConstantPropagator {
  public:
    ConstantPropagator(IConstantPropagator &prop)
      : m_added(false), m_constants(prop) {
    }

    virtual const PropagatedConstant &getConstant(const std::shared_ptr<RtlVariable> &var) const {
      return m_constants.getConstant(var);
    }

    virtual void setConstant(const std::shared_ptr<RtlVariable> &var, const std::shared_ptr<RtlType> &ty,
                             const std::span<std::uint8_t> &value) {
      m_added = true;
      m_constants.setConstant(var, ty, value);
    }

    inline bool added() const { return m_added; }

  private:
    bool m_added;
    IConstantPropagator &m_constants;
  };

  PropagatedConstant::PropagatedConstant(RtlFunction &fn, RtlConstantOp &op)
    : RtlOperandVisitor(fn) {
    op.operands(*this);
  }

  PropagatedConstant::PropagatedConstant(RtlFunction &fn,
                                         const std::shared_ptr<RtlVariable> &var,
                                         const std::shared_ptr<RtlType> &ty,
                                         const std::span<std::uint8_t> &value)
    : RtlOperandVisitor(fn), m_var(var), m_type(ty), m_const(value.size()) {
    m_const.resize(value.size());
    std::copy(value.begin(), value.end(), m_const.begin());
  }

  PropagatedConstant::~PropagatedConstant() {
  }

  void PropagatedConstant::operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
    if ( name == "dest" ) m_var = var;
  }

  void PropagatedConstant::operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    if ( name == "const" ) {
      m_const.resize(lsz);

      auto p((std::uint8_t *)l);
      std::copy(p, p + lsz, m_const.begin());
      m_type = ty;
    }
  }

  void PropagatedConstant::operand(const std::string &name, const RtlBlockName &dest) { return; }

  std::span<const std::uint8_t> PropagatedConstant::data() const {
    return std::span<const std::uint8_t>(m_const.begin(), m_const.size());
  }

  bool IConstantPropagator::isConstant(const std::shared_ptr<RtlVariable> &var) const {
    try {
      getConstant(var);
      return true;
    } catch ( const exception::VariableNotConstant &e) {
      return false;
    }
  }

  ConstantPropagation::ConstantPropagation(RtlFunction &fn)
    : m_function(fn) {
  }

  ConstantPropagation::~ConstantPropagation() {
  }

  const PropagatedConstant &ConstantPropagation::getConstant(const std::shared_ptr<RtlVariable> &var) const {
    auto it(m_constants.find(var));
    if ( it != m_constants.end() ) {
      return it->second;
    }
    throw exception::VariableNotConstant(var);
  }

  void ConstantPropagation::setConstant(const std::shared_ptr<RtlVariable> &var,
                                        const std::shared_ptr<RtlType> &ty,
                                        const std::span<std::uint8_t> &value) {
    auto it(m_constants.find(var));
    if ( it == m_constants.end() ) {
      m_constants.emplace(std::piecewise_construct,
                          std::forward_as_tuple(var),
                          std::forward_as_tuple(function(), var, ty, value));
    }
  }

  void ConstantPropagation::propagateBlock(std::shared_ptr<RtlBasicBlock> block) {
    bool newOps;

    do {
      newOps = false;
      for ( const auto &opPtr : *block ) {
        RtlOp &op(*opPtr);
        try {
          RtlConstantOp &constOp(dynamic_cast<RtlConstantOp &>(op));
          PropagatedConstant constant(m_function, constOp);
          std::shared_ptr<RtlVariable> var(constant.var());

          if ( m_constants.find(var) != m_constants.end() ) {
            //std::cerr << "Warning: constant output written twice" << std::endl;
          } else {
            m_constants.emplace(var, std::move(constant));
          }
        } catch ( const std::bad_cast &e ) {
          // Otherwise, check if this is a propagatable op
          try {
            RtlHasConstProp &propper(dynamic_cast<RtlHasConstProp &>(op));
            ConstantPropagator propagator(*this);
            propper.propagate(propagator);

            if ( propagator.added() ) {
              newOps = true;
            }
          } catch ( const std::bad_cast &e ) {
            continue;
          }
        }
      }

      // Now copy over all ops that are not already made redundant
      std::list<std::unique_ptr<RtlOp>> remainingOps;

      // All variables for which we've added an RtlConstantOp
      std::set<std::shared_ptr<RtlVariable>> m_constsAdded;
      for ( auto &opPtr : *block ) {
        RtlConstantOp *constOp(dynamic_cast<RtlConstantOp *>(opPtr.get()));
        if ( constOp ) {
          m_constsAdded.insert(constOp->dest());
          remainingOps.emplace_back(std::move(opPtr));
        } else if ( !opPtr->isPure() ) {
          remainingOps.emplace_back(std::move(opPtr));
        } else {
          OpOutputChecker checker(*this);
          opPtr->operands(checker);
          if ( !checker.allOutputsWritten() ) {
            // Any registers who are output only and not needed are removed
            std::unique_ptr<RtlOp> rewrittenOp(std::move(opPtr));

            // These are outputs which we have consntants for, but this instruction writes anyway
            for ( const auto &redundant: checker.redundantOutputs() ) {
              rewrittenOp->operand(redundant.first, redundant.second);
            }

            for ( const auto &constant: checker.constantInputs() ) {
              auto it(m_constsAdded.find(constant.first));
              if ( it == m_constsAdded.end() ) {
                std::span<const std::uint8_t> d(constant.second.data());
                remainingOps.push_back(std::make_unique<RtlConstantOp>(constant.second.type(),
                                                                       constant.second.data().data(), constant.second.data().size(),
                                                                       constant.first));
                m_constsAdded.insert(constant.first);
              }
            }

            remainingOps.emplace_back(std::move(rewrittenOp));
          }
        }
      }

      // Delete ops
      block->replaceOps(remainingOps.begin(), remainingOps.end());
    } while (newOps);
  }

  void ConstantPropagation::operator() () {
    for ( const auto &block: function().blocks() ) {
      propagateBlock(block.second);
    }

    std::cerr << "Before cull" << std::endl;
    function().dump(std::cerr);
    UnusedVarCuller culler(function());
    culler();
  }
}
