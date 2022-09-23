#ifndef __nnc_compile_dump_HPP__
#define __nnc_compile_dump_HPP__

#include <ostream>

#include "compile/rtl.hpp"

namespace nnc::compile {
  class RtlBasicBlockDumper {
  public:
    RtlBasicBlockDumper(std::ostream &out);
    virtual ~RtlBasicBlockDumper();

    void dump(RtlBasicBlock &block);
    void dump(RtlFunction &fn);
    virtual void dumpOp(RtlFunction &fn, const RtlBasicBlock *block, RtlOp &op, int index);
    inline std::ostream &out() const { return m_stream; }

  protected:
    virtual void dumpVar(const RtlBasicBlock *from, int opix, const RtlVariablePtr &var);
    virtual void dumpJumpArg(const RtlBasicBlock &from, const RtlVariablePtr &var);
    virtual void dumpBlockArg(const RtlBasicBlock &block, const RtlVariablePtr &var);
    virtual void dumpType(const std::shared_ptr<RtlType> &type);
    virtual void dumpBlockName(const RtlBlockName &blk);
    virtual void dumpJump(const RtlBasicBlock &blk, const RtlVariablePtr &cond, const RtlJump &jump);
    virtual void dumpReturn();
    virtual void dumpConstant(const std::shared_ptr<RtlType> &ty, const void *l,
                              std::size_t lsz);

  private:
    std::ostream &m_stream;

    friend class OperandPrinter;
  };

  class OperandPrinter : public virtual RtlOperandVisitor {
  public:
    OperandPrinter(RtlFunction &fn, const RtlBasicBlock *blk, int opix, RtlBasicBlockDumper &dump);
    virtual ~OperandPrinter();

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) override;
    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) override;
    virtual void operand(const std::string &name, const RtlBlockName &dest) override;

  protected:
    inline std::ostream &out() const { return m_dump.out(); }

    void next();

    RtlBasicBlockDumper &m_dump;
    bool m_first;

    const RtlBasicBlock *m_block;
    int m_opix;
  };
}

#endif
