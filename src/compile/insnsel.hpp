#ifndef __nnc_compile_insnsel_HPP__
#define __nnc_compile_insnsel_HPP__

#include "compile/rtl.hpp"
#include "compile/rtl_ops_base.hpp"

#include <memory>
#include <string>

namespace nnc {
  namespace compile {
    class InsnSelector;

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

    class RegClass {
    public:
      virtual std::string name() const =0;
    };

    class Register {
    public:
      virtual std::string name() const =0;
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
      virtual void matchOp(RtlOp *op) =0;
    };

    class SelectedInsn {
    public:
      virtual ~SelectedInsn();

      /* Used to 'mark off' operations that this instruction will handle */
      virtual void consumedOperations(RtlOpMarker &marker) const =0;

      virtual void build(RtlBasicBlock &compiled) const =0;
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
