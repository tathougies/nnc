#include "compile/dump.hpp"

namespace nnc::compile {
  RtlBasicBlockDumper::RtlBasicBlockDumper(std::ostream &out)
    : m_stream(out) {
  }

  RtlBasicBlockDumper::~RtlBasicBlockDumper() {
  }

  void RtlBasicBlockDumper::dump(RtlBasicBlock &block) {
    dumpBlockName(block.name());
    out() << "(";
    bool first(true);
    for ( const auto &arg: block.inputs() ) {
      if (!first) out() << ", ";
      dumpBlockArg(block, arg);
      first = false;
    }
    out() << "):" << std::endl;

    int opix(0);
    for ( const auto &op: block ) {
      dumpOp(block.function(), &block, *op, opix);
      out() << std::endl;
      ++opix;
    }

    for ( const auto &jump: block.jumps() ) {
      if ( jump.first ) {
        dumpJump(block, jump.first, jump.second);
      }
    }

    if ( block.has_jump() ) {
      dumpJump(block, nullptr, block.jump());
    } else
      dumpReturn();
  }

  void RtlBasicBlockDumper::dumpReturn() {
    out() << "  return" << std::endl;
  }

  void RtlBasicBlockDumper::dumpJump(const RtlBasicBlock &from, const RtlVariablePtr &cond, const RtlJump &jump) {
    out() << "  call ";
    dumpBlockName(jump.to());
    out() << "(";

    bool first(true);
    for ( const auto &arg: jump.arguments() ) {
      if ( !first ) out() << ", ";
      first = false;

      dumpJumpArg(from, arg);
    }

    out() << ")";

    if ( cond ) {
      out() << " if ";
      dumpVar(&from, from.endTime(), cond);
    }

    out() << std::endl;
  }

  void RtlBasicBlockDumper::dumpVar(const RtlBasicBlock *block, int opix, const RtlVariablePtr &var) {
    out() << var->name();
  }

  void RtlBasicBlockDumper::dumpJumpArg(const RtlBasicBlock &from, const RtlVariablePtr &var) {
    out() << "(";
    dumpType(var->type());
    out() << ") ";
    dumpVar(&from, from.endTime(), var);
  }

  void RtlBasicBlockDumper::dumpBlockArg(const RtlBasicBlock &block, const RtlVariablePtr &var) {
    out() << "(";
    dumpType(var->type());
    out() << ") ";
    dumpVar(&block, -1, var);
  }

  void RtlBasicBlockDumper::dumpType(const std::shared_ptr<RtlType> &type) {
    type->output(out());
  }

  void RtlBasicBlockDumper::dumpBlockName(const RtlBlockName &blk) {
    out() << "block" << blk.index();
  }

  void RtlBasicBlockDumper::dumpOp(RtlFunction &fn, const RtlBasicBlock *block, RtlOp &op, int index) {
    OperandPrinter p(fn, block, index, *this);
    out() << "  " << op.mnemonic() << "(";
    op.operands(p);
    out() << ")";;
  }

  void RtlBasicBlockDumper::dump(RtlFunction &f) {
    std::cerr << "Function " << f.functionName() << ":" << std::endl;
    for ( auto &b: f.blocks() )
      dump(b);
  }

  void RtlBasicBlockDumper::dumpConstant(const std::shared_ptr<RtlType> &ty, const void *l,
                                         std::size_t lsz) {
    out() << "[";
    dumpType(ty);
    out() << ">[";

    std::uint8_t *c((std::uint8_t *) l);
    std::copy(c, c + lsz,
              std::ostream_iterator<std::uint32_t>(out(), " "));
    out() << "]";
  }

  OperandPrinter::OperandPrinter(RtlFunction &fn, const RtlBasicBlock *block, int opix, RtlBasicBlockDumper &dump)
    : RtlOperandVisitor(fn)
    , m_dump(dump), m_first(true)
    , m_block(block), m_opix(opix) {
  }

  OperandPrinter::~OperandPrinter() {
  }

  void OperandPrinter::operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
    next();

    out() << name;
    if ( input && output ) out() << "<->";
    else if ( output ) out() << "->";
    else out() << "=";

    m_dump.dumpVar(m_block, m_opix, var);
  }

  void OperandPrinter::operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    next();

    out() << name << "=";
    m_dump.dumpConstant(ty, l, lsz);
  }

  void OperandPrinter::operand(const std::string &name, const RtlBlockName &dest) {
    next();
    out() << name << "=";
    m_dump.dumpBlockName(dest);
  }


  void OperandPrinter::next() {
    if ( m_first ) { m_first = false; }
    else {
      out() << ", ";
    }
  }
}
