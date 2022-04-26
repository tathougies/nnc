#ifndef __nnc_constant_propagation_HPP__
#define __nnc_constant_propagation_HPP__

#include "compile/rtl.hpp"
#include "compile/rtl_ops_base.hpp"

#include <memory>
#include <span>

namespace nnc::compile {
  class RtlConstantOp;

  class PropagatedConstant : public RtlOperandVisitor {
  public:
    PropagatedConstant(RtlFunction &fn, RtlConstantOp &op);
    PropagatedConstant(RtlFunction &fn, const std::shared_ptr<RtlVariable> &var,
                       const std::shared_ptr<RtlType> &ty,
                       const std::span<std::uint8_t> &value);
    virtual ~PropagatedConstant();

    inline std::shared_ptr<RtlVariable> var() const { return m_var; }
    inline std::shared_ptr<RtlType> type() const { return m_type; }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var,
                         bool input = true, bool output = false);
    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty,
                         const void *l, std::size_t lsz);
    virtual void operand(const std::string &name, const RtlBlockName &dest);

    std::span<const std::uint8_t> data() const;

  private:
    std::shared_ptr<RtlVariable> m_var;
    std::shared_ptr<RtlType> m_type;
    std::vector<std::uint8_t> m_const;
  };

  class IConstantPropagator {
  public:
    virtual const PropagatedConstant &getConstant(const std::shared_ptr<RtlVariable> &var) const=0;
    virtual void setConstant(const std::shared_ptr<RtlVariable> &var, const std::shared_ptr<RtlType> &ty,
                             const std::span<std::uint8_t> &value) =0;

    bool isConstant(const std::shared_ptr<RtlVariable> &var) const;
  };

  class RtlHasConstProp {
  public:
    virtual void propagate(IConstantPropagator &p) const =0;
  };

  class ConstantPropagation : public IConstantPropagator {
  public:
    ConstantPropagation(RtlFunction &fn);
    virtual ~ConstantPropagation();

    void propagateBlock(std::shared_ptr<RtlBasicBlock> block);
    void operator() ();

    virtual const PropagatedConstant &getConstant(const std::shared_ptr<RtlVariable> &var) const;
    virtual void setConstant(const std::shared_ptr<RtlVariable> &var, const std::shared_ptr<RtlType> &ty,
                             const std::span<std::uint8_t> &value);

    inline RtlFunction &function() const { return m_function; }

  private:
    RtlFunction &m_function;
    std::map<std::shared_ptr<RtlVariable>, PropagatedConstant> m_constants;
  };
}

#endif
