#include "compile/encoding.hpp"

namespace nnc::exception {
  InvalidOpcode::InvalidOpcode(const char *msg)
    : m_msg(msg) {
  }

  InvalidOpcode::~InvalidOpcode() {
  }

  const char *InvalidOpcode::what() const noexcept { return m_msg; }

  VariableNotInClass::VariableNotInClass(const compile::RtlVariablePtr &var,
                                         const compile::RegClass &cls)
    : m_var(var), m_cls(cls) {
  }

  VariableNotInClass::~VariableNotInClass() {
  }

  const char *VariableNotInClass::what() const noexcept { return "variable not in class"; }
}

namespace nnc::compile {
  class NeedsMovesVisitor : public RtlOperandVisitor {
  public:
    NeedsMovesVisitor(RtlFunction &fn, const std::multimap<RtlVariablePtr, VirtualRegister> &assignments)
      : RtlOperandVisitor(fn) {
      std::cerr << "Needs moves " << assignments.size() << std::endl;
      for ( const auto &a: assignments ) {
        std::cerr << "Add " << a.first << std::endl;
        m_needsMoves.insert(a.first);
      }
    }
    virtual ~NeedsMovesVisitor() { }

    virtual void operand(const std::string &name, RtlVariablePtr var, bool input = true, bool output = false) override {
      if ( output ) {
        auto it(m_needsMoves.find(var));
        if ( it != m_needsMoves.end() )
          m_needsMoves.erase(it);
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t sz) override {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) override {
    }

    inline const std::set<RtlVariablePtr> vars() const { return m_needsMoves; }

  private:
    std::set<RtlVariablePtr> m_needsMoves;
  };

  void encodeBasicBlock(std::ostream &out, const RtlBasicBlock &block, const RegisterTracker &t, const InsnEncoder &e) {
    int time(0);
    for ( const auto &op: block ) {
      auto inputs(t.mapper(time - 1));
      if ( (time + 1) < t.states().size() ) {
        // Check for any new assignments
        const RegState &st(t.states()[time + 1]);
        NeedsMovesVisitor needsMoves(block.function(), st.newAssignments);

        // Any output operand does not need a move
        op->operands(needsMoves);

        for ( const auto &var: needsMoves.vars() ) {
          auto existing(inputs.lookupVar(var));
          if ( existing.empty() ) continue;

          auto range(st.newAssignments.equal_range(var));
          for ( auto it(range.first);
                it != range.second;
                ++it ) {
            // This is a new assignment for it->first
            e.copy(out, inputs, it->first, it->second);
          }
        }
      }

      RtlEmitOp *emitted(dynamic_cast<RtlEmitOp *>(op.get()));
      if ( emitted ) {
        emitted->encode(inputs, t.mapper(time), out);
      }

      ++time;
    }
  }

  void InsnEncoder::preamble(std::ostream &out) const {
  }

  void InsnEncoder::postamble(std::ostream &out) const {
  }
}
