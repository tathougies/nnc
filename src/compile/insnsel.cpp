#include "compile/insnsel.hpp"
#include "compile/rtl_ops.hpp"
#include "compile/arbitrary.hpp"

namespace nnc {
  namespace compile {
    InsnSelRule::~InsnSelRule() {
    }

    SelectedInsn::~SelectedInsn() {
    }

    RtlAliasOp::RtlAliasOp(std::shared_ptr<RtlVariable> a,
                           std::shared_ptr<RtlVariable> b)
      : RtlOp("alias"), m_a(a), m_b(b) {
    }

    RtlAliasOp::~RtlAliasOp() {
    }

    void RtlAliasOp::operands(RtlOperandVisitor &v) const {
      v.operand("in", m_a);
      v.operand("out", m_b, false, true);
    }

    void RtlAliasOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "in" ) m_a = var;
      else if ( nm == "out" ) m_b = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

    RtlArbitraryOp::RtlArbitraryOp(const RtlVariablePtr &a,
                                   const RegClass &rc)
      : RtlOp("arbitrary"), m_var(a), m_class(rc) {
    }

    RtlArbitraryOp::~RtlArbitraryOp() {
    }

    void RtlArbitraryOp::operands(RtlOperandVisitor &v) const {
      v.operand("var", m_var);
    }

    void RtlArbitraryOp::operand(const std::string &nm, RtlVariablePtr var) {
      if ( nm == "var" )
        m_var = var;
      else
        throw exception::RtlOpArgDoesNotExist(nm);
    }

    void RtlArbitraryOp::regclasses(RegClassDeclarer &decl) const {
      decl.regclass("var", m_var, m_class);
    }
  }
}
