#include "compile/rtl_ops.hpp"

#include <sstream>
#include <iostream>

namespace nnc {
  namespace exception {
    RtlOpArgDoesNotExist::RtlOpArgDoesNotExist(const std::string &nm)
      : m_name(nm) {
    }

    RtlOpArgDoesNotExist::~RtlOpArgDoesNotExist() {
    }

    const char *RtlOpArgDoesNotExist::what() const noexcept {
      return m_name.c_str(); //"op arg does not exist";
    }
  }

  namespace compile {
    RtlOperandVisitor::RtlOperandVisitor(RtlFunction &fn)
      : m_function(fn) {
    }

    RtlOp::RtlOp(const char *insn)
      : m_mnemonic(insn) {
    }

    RtlOp::~RtlOp() {
    }

    bool RtlOp::isPure() const { return true; }

    void RtlOp::operands(RtlOperandVisitor &v) const {
    }

    /* RtlCmpOp */
    const char *RtlCmpOp::cmpMnemonic(RtlComparison c) {
      switch (c) {
      case RtlLessThan: return "lt";
      case RtlGreaterThan: return "gt";
      case RtlEqual: return "eq";
      default: return nullptr;
      }
    }

    RtlCmpOp::RtlCmpOp(RtlComparison c, std::shared_ptr<RtlVariable> left,
                       std::shared_ptr<RtlVariable> right,
                       std::shared_ptr<RtlVariable> dest)
      : RtlOp(cmpMnemonic(c)), m_left(left), m_right(right), m_dest(dest) {
    }

    RtlCmpOp::~RtlCmpOp() {
    }

    void RtlCmpOp::operands(RtlOperandVisitor &v) const {
      v.operand("left", m_left);
      v.operand("right", m_right);
      v.operand("dest", m_dest, false, true);
    }

    void RtlCmpOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "left" ) m_left = var;
      else if ( nm == "right" ) m_right = var;
      else if ( nm == "dest" ) m_dest = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

    /* RtlArithOp */
    const char *RtlArithOp::arithMnemonic(RtlArithmetic a) {
      switch (a) {
      case RtlAdd: return "add";
      case RtlSub: return "sub";
      case RtlMul: return "mul";
      case RtlDiv: return "div";
      case RtlRem: return "rem";

      case RtlLogicalOr: return "lor";
      case RtlLogicalAnd: return "land";
      case RtlLogicalNot: return "lnot";

      case RtlBitwiseOr: return "or";
      case RtlBitwiseAnd: return "and";
      case RtlBitwiseNot: return "comp";
      default: return nullptr;
      }
    }

    RtlArithOp::RtlArithOp(RtlArithmetic a,
                           std::shared_ptr<RtlVariable> left,
                           std::shared_ptr<RtlVariable> right,
                           std::shared_ptr<RtlVariable> dest)
      : RtlOp(arithMnemonic(a)), m_op(a), m_left(left), m_right(right), m_dest(dest) {
    }

    RtlArithOp::~RtlArithOp() {
    }

    void RtlArithOp::operands(RtlOperandVisitor &v) const {
      v.operand("left", m_left);
      v.operand("right", m_right);
      v.operand("dest", m_dest, false, true);
    }

    void RtlArithOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "left" ) m_left = var;
      else if ( nm == "right" ) m_right = var;
      else if ( nm == "dest" ) m_dest = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

    /* RtlConstantOp */
    RtlConstantOp::RtlConstantOp(std::shared_ptr<RtlType> constType,
                                 const void *ptr, std::size_t sz,
                                 std::shared_ptr<RtlVariable> dest)
      : RtlOp("const"), m_type(constType), m_data(sz), m_dest(dest) {
      const std::uint8_t *ptr8((const std::uint8_t *)ptr);
      std::copy(ptr8, ptr8 + sz, m_data.begin());
    }

    RtlConstantOp::~RtlConstantOp() {
    }

    void RtlConstantOp::operands(RtlOperandVisitor &v) const {
      v.operand("const", m_type, m_data.data(), m_data.size());
      v.operand("dest", m_dest, false, true);
    }

    void RtlConstantOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "dest" ) m_dest = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

    /* RtlLoadOp */
    RtlLoadOp::RtlLoadOp(std::shared_ptr<RtlVariable> addr, std::shared_ptr<RtlVariable> dest)
      : RtlOp("load"), m_addr(addr), m_dest(dest) {
    }

    RtlLoadOp::~RtlLoadOp() {
    }

    void RtlLoadOp::operands(RtlOperandVisitor &v) const {
      v.operand("addr", m_addr);
      v.operand("dest", m_dest, false, true);
    }

    void RtlLoadOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "addr" ) m_addr = var;
      else if ( nm == "dest" ) m_dest = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

    /* RtlStoreOp */
    RtlStoreOp::RtlStoreOp(std::shared_ptr<RtlVariable> addr, std::shared_ptr<RtlVariable> val)
      : RtlOp("store"), m_addr(addr), m_val(val) {
    }

    RtlStoreOp::~RtlStoreOp() {
    }

    bool RtlStoreOp::isPure() const {
      return false;
    }

    void RtlStoreOp::operands(RtlOperandVisitor &v) const {
      v.operand("addr", m_addr);
      v.operand("value", m_val);
    }

    void RtlStoreOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "addr" ) m_addr = var;
      else if ( nm == "value" ) m_val = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

    /* RtlCastLikeOp */
    RtlCastLikeOp::RtlCastLikeOp(const char *mnemonic,
                                 std::shared_ptr<RtlVariable> src,
                                 std::shared_ptr<RtlVariable> dest)
      : RtlOp(mnemonic), m_src(src), m_dest(dest) {
    }

    RtlCastLikeOp::~RtlCastLikeOp() {
    }

    void RtlCastLikeOp::operands(RtlOperandVisitor &v) const {
      v.operand("in", m_src);
      v.operand("out", m_dest, false, true);
    }

    void RtlCastLikeOp::operand(const std::string &nm, std::shared_ptr<RtlVariable> var) {
      if ( nm == "in" ) m_src = var;
      else if ( nm == "out" ) m_src = var;
      else throw exception::RtlOpArgDoesNotExist(nm);
    }

#define IMPL_CASTLIKE_OP(OpName, Nm)                    \
    OpName::OpName(std::shared_ptr<RtlVariable> src,    \
                   std::shared_ptr<RtlVariable> dest)   \
      : RtlCastLikeOp(Nm, src, dest) {                  \
    }                                                   \
                                                        \
    OpName::~OpName() {                                 \
    }

    IMPL_CASTLIKE_OP(RtlZeroExtendOp, "zext");
    IMPL_CASTLIKE_OP(RtlSignExtendOp, "sext");
    IMPL_CASTLIKE_OP(RtlTruncateOp, "trunc");
    IMPL_CASTLIKE_OP(RtlFpToUiOp, "fptoui");
    IMPL_CASTLIKE_OP(RtlUiToFpOp, "uitofp");
    IMPL_CASTLIKE_OP(RtlFpToSiOp, "fptosi");
    IMPL_CASTLIKE_OP(RtlSiToFpOp, "sitofp");
    IMPL_CASTLIKE_OP(RtlPtrToIntOp, "ptrtoint");
    IMPL_CASTLIKE_OP(RtlIntToPtrOp, "inttoptr");
    IMPL_CASTLIKE_OP(RtlMovOp, "mov");

#define NO_CONSTANT_PROPAGATION(OpName)                                 \
    void OpName::propagate(IConstantPropagator &p) const {              \
      return;                                                           \
    }

    NO_CONSTANT_PROPAGATION(RtlSignExtendOp);
    NO_CONSTANT_PROPAGATION(RtlTruncateOp);
    NO_CONSTANT_PROPAGATION(RtlFpToUiOp);
    NO_CONSTANT_PROPAGATION(RtlUiToFpOp);
    NO_CONSTANT_PROPAGATION(RtlFpToSiOp);
    NO_CONSTANT_PROPAGATION(RtlSiToFpOp);
    NO_CONSTANT_PROPAGATION(RtlPtrToIntOp);
    NO_CONSTANT_PROPAGATION(RtlIntToPtrOp);

    void RtlMovOp::propagate(IConstantPropagator &p) const {
      if (p.isConstant(source())) {
        const auto &constant(p.getConstant(source()));

        p.setConstant(dest(), source()->type(), constant.data());
      }
    }

    class IntWidthVisitor : public RtlTypeVisitor {
    public:
      IntWidthVisitor() : m_width(-1) {
      }
      virtual ~IntWidthVisitor() { }

      virtual void intType(RtlIntSignedness s, RtlIntWidth w) { m_signedness = s;  m_width = w; }
      virtual void pointerType(std::shared_ptr<RtlType> u) { };
      virtual void floatType(RtlFloatWidth w) { }

      inline bool isInt() const { return m_width >= 0; }
      inline int width() const { return m_width; }
      inline bool isUnsigned() const { return m_signedness == RtlUnsigned && isInt(); }

      inline RtlIntSignedness signedness() const { return m_signedness; }

    private:
      RtlIntSignedness m_signedness;
      int m_width;
    };

    void RtlZeroExtendOp::propagate(IConstantPropagator &p) const {
      if ( p.isConstant(source()) ) {
        const PropagatedConstant &c(p.getConstant(source()));
        union {
          std::uint64_t i;
          std::uint8_t d[8];
        };

        i = interpretAsInt(c.data());

        IntWidthVisitor v;
        dest()->type()->visit(v);

        if ( v.isInt() ) {
          std::size_t byteSz(((v.width() + 7) / 8));
          if ( byteSz <= sizeof(d) ) {
            std::span<std::uint8_t> dat(d, byteSz);
            p.setConstant(dest(), dest()->type(), dat);
          }
        }
      }
    }

    template<typename T>
    T binArith(RtlArithmetic op, T l, T r) {
      switch ( op ) {
      case RtlAdd:
        return l + r;

      case RtlSub:
        return l - r;

      case RtlMul:
        return l * r;

      case RtlDiv:
        return l / r;

      case RtlRem:
        return l % r;

      case RtlLogicalOr:
        return l || r;

      case RtlLogicalAnd:
        return l && r;

      case RtlBitwiseOr:
        return l | r;

      case RtlBitwiseAnd:
        return l & r;
      }
      return 0;
    }

    union intarith {
      std::uint64_t u;
      std::int64_t i;
      std::uint8_t d[8];
    };

    void RtlArithOp::propagate(IConstantPropagator &p) const {
      std::span<const std::uint8_t> lc, rc;
      if ( p.isConstant(left()) ) {
        lc = p.getConstant(left()).data();
      }

      if ( p.isConstant(right()) ) {
        rc = p.getConstant(right()).data();
      }

      // TODO identity
      if ( !lc.data() || !rc.data() ) {
        return;
      }

      IntWidthVisitor lty, rty;

      left()->type()->visit(lty);
      right()->type()->visit(rty);
      if ( lty.isInt() && rty.isInt() &&
           lty.width() == rty.width() ) {
        if ( lty.signedness() == rty.signedness() ) {
          std::size_t byteSz((lty.width() + 7) / 8);
          if ( byteSz <= 8 ) {
            intarith l, r, res;

            l.u = interpretAsInt(lc);
            r.u = interpretAsInt(rc);

            switch ( lty.signedness() ) {
            case RtlUnsigned:
              res.u = binArith<std::uint64_t>(op(), l.u, r.u);
              break;

            case RtlTwosComplement:
              res.i = binArith<std::int64_t>(op(), l.i, r.i);
              break;

            default:
              return;
            }

            std::span<std::uint8_t> dat(res.d, byteSz);
            p.setConstant(dest(), dest()->type(), dat);
          }
        }
      }
    }
  }
}
