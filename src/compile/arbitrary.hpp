#ifndef __nnc_compile_arbitrary_HPP__
#define __nnc_compile_arbitrary_HPP__

#include "compile/regalloc.hpp"
#include "compile/rtl_ops_base.hpp"

namespace nnc::compile {
  class RtlArbitraryOp : public RtlOp, public RtlRegisterOp {
  public:
    RtlArbitraryOp(const RtlVariablePtr &a,
                   const RegClass &rc);
    virtual ~RtlArbitraryOp();

    virtual void regclasses(RegClassDeclarer &decl) const;
    virtual void operands(RtlOperandVisitor &v) const;
    virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

    inline const RegClass &regclass() const { return m_class; }
    inline const RtlVariablePtr &var() const { return m_var; }

  private:
    std::shared_ptr<RtlVariable> m_var;
    const RegClass &m_class;
  };
}

#endif
