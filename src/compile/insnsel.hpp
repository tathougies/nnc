#ifndef __nnc_compile_insnsel_HPP__
#define __nnc_compile_insnsel_HPP__

#include "compile/rtl.hpp"
#include "compile/rtl_ops_base.hpp"
#include "compile/registers.hpp"

#include <span>
#include <memory>
#include <string>

namespace nnc {
  namespace compile {
    class InsnSelector;

    class RtlAliasOp : public RtlOp {
    public:
      RtlAliasOp(std::shared_ptr<RtlVariable> a,
                 std::shared_ptr<RtlVariable> b);
      virtual ~RtlAliasOp();

      virtual void operands(RtlOperandVisitor &v) const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var);

      inline std::shared_ptr<RtlVariable> src() const { return m_a; }
      inline std::shared_ptr<RtlVariable> dest() const { return m_b; }

    private:
      std::shared_ptr<RtlVariable> m_a, m_b;
    };

    class InterestRecorder {
    public:
      virtual void allInstructions() =0;
      virtual void instruction(const char *nm) =0;
    };

    class InsnSelRule {
    public:
      virtual ~InsnSelRule();

      virtual void apply(InsnSelector &sel, RtlOp &op) const =0;
      virtual void registerInterest(InterestRecorder &r) const =0;
    };

    class StackAllocation {
    public:
      StackAllocation(std::int32_t offs,
                      std::size_t sz);

      inline std::int32_t frameOffset() const { return m_frameOffset; }
      inline std::size_t size() const { return m_size; }

    private:
      std::int32_t m_frameOffset;
      std::size_t m_size;
    };
    
    class RtlOpMarker {
    public:
      virtual void matchOp(RtlOp *op, bool consume = true) =0;
    };

    class RtlOpSelector : public RtlOpBuilder {
    public:
      virtual void alias(const std::shared_ptr<RtlVariable> &left,
                         const std::shared_ptr<RtlVariable> &right) =0;

      // Signal to the selector that the value of this variable is arbitrary
      virtual void arbitrary(const RtlVariablePtr &v, const RegClass &rc) =0;
    };

    class RtlCostedOp {
    public:
      virtual std::uint64_t cost() const =0;
    };

    class SelectedInsn {
    public:
      virtual ~SelectedInsn();

      /* Used to 'mark off' operations that this instruction will handle */
      virtual void consumedOperations(RtlOpMarker &marker) const =0;

      virtual void build(RtlOpSelector &compiled) const =0;
    };

    class InsnSelector {
    public:
      /* Add a new rule to the queue */
      virtual RtlFunction &function() const =0;
      virtual void addRule(std::unique_ptr<InsnSelRule> &&rule) =0;
      virtual void markInsnSchedulable(std::unique_ptr<SelectedInsn> &&rule) =0;
    };
  }
}

#endif
