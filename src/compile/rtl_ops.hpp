#ifndef __nnc_compile_rtl_ops_HPP__
#define __nnc_compile_rtl_ops_HPP__

#include "compile/rtl_types.hpp"
#include "compile/rtl_ops_base.hpp"
#include "compile/rtl.hpp"
#include "compile/constant_propagation.hpp"

namespace nnc {
  namespace exception {
    class RtlOpArgDoesNotExist : public std::exception {
    public:
      RtlOpArgDoesNotExist(const std::string &nm);
      virtual ~RtlOpArgDoesNotExist();

      virtual const char *what() const noexcept;

      inline const std::string &name() const { return m_name; }

    private:
      std::string m_name;
    };
  }

  namespace compile {
    enum RtlComparison {
      RtlLessThan,
      RtlGreaterThan,
      RtlEqual
    };

    enum RtlArithmetic {
      RtlAdd,
      RtlSub,
      RtlMul,
      RtlDiv,
      RtlRem,

      RtlLogicalOr,
      RtlLogicalAnd,
      RtlLogicalNot,

      RtlBitwiseOr,
      RtlBitwiseAnd,
      RtlBitwiseNot
    };

    class RtlConstantOp : public RtlOp {
    public:
      RtlConstantOp(std::shared_ptr<RtlType> constType,
                    const void *ptr, std::size_t sz,
                    std::shared_ptr<RtlVariable> dest);
      virtual ~RtlConstantOp();

      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

      inline std::shared_ptr<RtlType> type() const { return m_type; }
      inline std::shared_ptr<RtlVariable> dest() const { return m_dest; }

    private:
      std::shared_ptr<RtlType> m_type;
      std::vector<std::uint8_t> m_data;
      std::shared_ptr<RtlVariable> m_dest;
    };

    class RtlLoadOp : public RtlOp {
    public:
      RtlLoadOp(std::shared_ptr<RtlVariable> addrSrc,
                std::shared_ptr<RtlVariable> dest);
      virtual ~RtlLoadOp();

      inline std::shared_ptr<RtlVariable> addr() const { return m_addr; }
      inline std::shared_ptr<RtlVariable> dest() const { return m_dest; }

      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

    private:
      std::shared_ptr<RtlVariable> m_addr, m_dest;
    };

    class RtlStoreOp : public RtlOp {
    public:
      RtlStoreOp(std::shared_ptr<RtlVariable> addrSrc,
                 std::shared_ptr<RtlVariable> dest);
      virtual ~RtlStoreOp();

      inline std::shared_ptr<RtlVariable> addr() const { return m_addr; }
      inline std::shared_ptr<RtlVariable> value() const { return m_val; }

      virtual bool isPure() const override;
      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

    private:
      std::shared_ptr<RtlVariable> m_addr, m_val;
    };

    class RtlCastLikeOp : public RtlOp {
    public:
      virtual ~RtlCastLikeOp();

      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

      inline std::shared_ptr<RtlVariable> source() const { return m_src; }
      inline std::shared_ptr<RtlVariable> dest() const { return m_dest; }

    protected:
      RtlCastLikeOp(const char *mnemonic,
                    std::shared_ptr<RtlVariable> src,
                    std::shared_ptr<RtlVariable> dest);

    private:
      std::shared_ptr<RtlVariable> m_src, m_dest;
    };

#define DECL_CASTLIKE_OP(OpName)                                \
    class OpName : public RtlCastLikeOp                         \
                 , public RtlHasConstProp {                     \
    public:                                                     \
      OpName(std::shared_ptr<RtlVariable> src,                  \
             std::shared_ptr<RtlVariable> dest);                \
      virtual ~OpName();                                        \
                                                                \
      virtual void propagate(IConstantPropagator &p) const;     \
    };

    DECL_CASTLIKE_OP(RtlZeroExtendOp);
    DECL_CASTLIKE_OP(RtlSignExtendOp);
    DECL_CASTLIKE_OP(RtlTruncateOp);
    DECL_CASTLIKE_OP(RtlFpToUiOp);
    DECL_CASTLIKE_OP(RtlUiToFpOp);
    DECL_CASTLIKE_OP(RtlFpToSiOp);
    DECL_CASTLIKE_OP(RtlSiToFpOp);
    DECL_CASTLIKE_OP(RtlPtrToIntOp);
    DECL_CASTLIKE_OP(RtlIntToPtrOp);
    DECL_CASTLIKE_OP(RtlMovOp);

    class RtlCmpOp : public RtlOp {
    public:
      RtlCmpOp(RtlComparison c,
               std::shared_ptr<RtlVariable> left,
               std::shared_ptr<RtlVariable> right,
               std::shared_ptr<RtlVariable> dest);
      virtual ~RtlCmpOp();

      inline std::shared_ptr<RtlVariable> left() const { return m_left; }
      inline std::shared_ptr<RtlVariable> right() const { return m_right; }

      inline std::shared_ptr<RtlVariable> dest() const { return m_dest; }

      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

      static const char *cmpMnemonic(RtlComparison c);
    private:
      std::shared_ptr<RtlVariable> m_left, m_right, m_dest;
    };

    class RtlArithOp : public RtlOp,
                       public RtlHasConstProp {
    public:
      RtlArithOp(RtlArithmetic a,
                 std::shared_ptr<RtlVariable> left,
                 std::shared_ptr<RtlVariable> right,
                 std::shared_ptr<RtlVariable> dest);
      virtual ~RtlArithOp();

      inline std::shared_ptr<RtlVariable> left() const { return m_left; }
      inline std::shared_ptr<RtlVariable> right() const { return m_right; }
      inline std::shared_ptr<RtlVariable> dest() const { return m_dest; }

      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

      static const char *arithMnemonic(RtlArithmetic a);

      virtual void propagate(IConstantPropagator &p) const;

      inline RtlArithmetic op() const { return m_op; }

    private:
      RtlArithmetic m_op;
      std::shared_ptr<RtlVariable> m_left, m_right, m_dest;
    };
  }
}

#endif
