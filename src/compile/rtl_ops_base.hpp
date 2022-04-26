#ifndef __nnc_compile_rtl_ops_base_HPP__
#define __nnc_compile_rtl_ops_base_HPP__

#include <memory>
#include <string>

#include "compile/rtl_types.hpp"

namespace nnc {
  namespace compile {
    class RtlBlockName;
    class RtlFunction;

    class RtlVariable : public std::enable_shared_from_this<RtlVariable> {
    public:
      RtlVariable(const std::string &name,
                  std::shared_ptr<RtlType> type);
      ~RtlVariable();

      inline const std::string &name() { return repr()->m_name; }
      inline std::shared_ptr<RtlType> type() { return repr()->m_type; }
      std::string namePrefix() const;

      std::shared_ptr<RtlVariable> repr();

      void alias(std::shared_ptr<RtlVariable> a);

      inline bool isSame(std::shared_ptr<RtlVariable> o) {
        if ( o.get() == this ) return true;
        else if ( repr().get() != this )
          return repr()->isSame(o->repr());
        else
          return false;
      }

    protected:
      std::shared_ptr<RtlVariable> rawRepr();

    private:
      std::string m_name;
      std::shared_ptr<RtlType> m_type;
      std::shared_ptr<RtlVariable> m_alias;
    };

    class RtlOperandVisitor {
    public:
      RtlOperandVisitor(RtlFunction &fn);

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) =0;
      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) =0;
      virtual void operand(const std::string &name, const RtlBlockName &dest) =0;

      inline RtlFunction &function() const { return m_function; }

    private:
      RtlFunction &m_function;
    };

    class RtlOp {
    public:
      RtlOp(const char *insn);
      virtual ~RtlOp();

      inline const char *mnemonic() const { return m_mnemonic; }

      virtual void operands(RtlOperandVisitor &v) const;

      // Set operand
      virtual bool isPure() const;
      virtual void operand(const std::string &nm, std::shared_ptr<RtlVariable> var) =0;

    private:
      const char *m_mnemonic;
    };
  }
}

#endif
