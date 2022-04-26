#include "cpu_op.hpp"

#include <iomanip>
#include <iterator>

namespace nnc {
  namespace exception {
    class UnknownDataType : public std::exception {
    public:
      virtual const char *what() const noexcept {
        return "Unknown data type";
      }
    };
  }

  namespace executor {
    CpuOpVisitor::~CpuOpVisitor() {
    }

    CpuOp::~CpuOp() {
    }

    void CpuOp::visit(CpuOpVisitor &v) const {
      v.op(*this);
    }

    void CpuOp::print(std::ostream &out) const {
      out << "<unknown op>" << std::endl;
    }

    CpuCastOp::CpuCastOp(const DataType &from, const DataType &to,
                         std::shared_ptr<CpuTensorInput> src,
                         std::shared_ptr<CpuTensorInput> dest)
      : m_from(from), m_to(to), m_source(src), m_dest(dest) {
    }

    CpuCastOp::~CpuCastOp() {
    }

    void CpuCastOp::print(std::ostream &out) const {
      m_dest->print(out) << " = cast<" <<
        m_from.name() << ", " << m_to.name() << ">(";
      m_source->print(out) << ");" << std::endl;
    }

    CpuConstantOp::CpuConstantOp(const DataType &dt, const void *d, std::size_t sz,
                                 std::shared_ptr<CpuTensorInput> dest)
      : m_type(dt), m_data(sz), m_dest(dest) {
      std::copy_n((std::uint8_t *)d, sz, m_data.begin());
    }

    CpuConstantOp::~CpuConstantOp() {
    }

    void CpuConstantOp::makeRtlOp(compile::RtlFunction &fn,
                                  CpuRtlInputTranslator &h) const {
      auto destvar(fn.variable("dest", m_type.rtlType(fn.types())));
      h.block()->emplace_op<compile::RtlConstantOp>(m_type.rtlType(fn.types()),
                                                    m_data.data(), m_data.size(),
                                                    destvar);
      h.tensor(m_dest, destvar);
    }

    void CpuConstantOp::print(std::ostream &out) const {
      auto flags(out.flags());

      m_dest->print(out) << " = const<" << m_data.size() << ">[" << std::hex << std::setw(2) << std::setfill('0');

      std::copy(m_data.begin(), m_data.end(), std::ostream_iterator<int>(out, " "));

      out.flags(flags);
      out << "];" << std::endl;
    }

    CpuLoopOp::CpuLoopOp(int size, const CpuOpList &ops)
      : m_size(size), m_ops(ops) {
    }

    CpuLoopOp::~CpuLoopOp() {
    }

    const CpuOpList &CpuLoopOp::ops() const {
      return m_ops;
    }

    void CpuLoopOp::addOp(const std::shared_ptr<CpuOp> &op) {
      m_ops.push_back(op);
    }

    void CpuLoopOp::visit(CpuOpVisitor &v) const {
      v.newLoop(m_size);
      for ( const auto &op : m_ops )
        op->visit(v);
      v.endLoop();
    }

    void CpuLoopOp::print(std::ostream &out) const {
      CpuOpPrinter printer(out);
      visit(printer);
    }

    CpuBinOp::CpuBinOp(const CpuTensorPtr &dest, const CpuTensorPtr &a, const CpuTensorPtr &b)
      : m_a(a), m_b(b), m_dest(dest) {
    }

    CpuBinOp::~CpuBinOp() {
    }

    const DataType &CpuBinOp::resultType() const {
      auto destIn(std::dynamic_pointer_cast<BasicTensorInput>(m_dest));
      if ( destIn ) return destIn->dataType();

      auto aIn(std::dynamic_pointer_cast<BasicTensorInput>(m_a));
      if ( aIn ) return aIn->dataType();

      auto bIn(std::dynamic_pointer_cast<BasicTensorInput>(m_b));
      if ( bIn ) return bIn->dataType();

      throw exception::UnknownDataType();
    }

    void CpuBinOp::print(std::ostream &out) const {
      m_dest->print(out) << " = " << opName() << "(";
      m_a->print(out) << ", ";
      m_b->print(out) << ");" << std::endl;
    }

    CpuTensorPtr CpuBinOp::a() const { return m_a; }
    CpuTensorPtr CpuBinOp::b() const { return m_b; }
    CpuTensorPtr CpuBinOp::dest() const { return m_dest; }

    CpuBinRtlOp::CpuBinRtlOp(compile::RtlArithmetic a)
      : m_bop(a) {
    }

    CpuBinRtlOp::~CpuBinRtlOp() {
    }

    void CpuBinRtlOp::makeRtlOp(compile::RtlFunction &fn,
                                CpuRtlInputTranslator &h) const {
      auto avar(h.tensor(a()));
      auto bvar(h.tensor(b()));

      auto destvar(fn.variable("dest", resultType().rtlType(fn.types())));

      h.block()->emplace_op<compile::RtlArithOp>(m_bop, avar, bvar, destvar);
      h.tensor(dest(), destvar);
    }

    CpuAddOp::CpuAddOp(const CpuTensorPtr &dest,
                       const CpuTensorPtr &a,
                       const CpuTensorPtr &b)
      : CpuBinOp(dest, a, b),
        CpuBinRtlOp(compile::RtlAdd) {
    }

    CpuAddOp::~CpuAddOp() {
    }

    const char *CpuAddOp::opName() const {
      return "add";
    }

    CpuMulOp::CpuMulOp(const CpuTensorPtr &dest,
                       const CpuTensorPtr &a,
                       const CpuTensorPtr &b)
      : CpuBinOp(dest, a, b),
        CpuBinRtlOp(compile::RtlMul) {
    }

    CpuMulOp::~CpuMulOp() {
    }

    const char *CpuMulOp::opName() const {
      return "mul";
    }

    CpuOpPrinter::CpuOpPrinter(std::ostream &out)
      : m_indent(0), m_out(out) {
    }

    CpuOpPrinter::~CpuOpPrinter() {
    }

    void CpuOpPrinter::indent() {
      std::fill_n(std::ostream_iterator<char>(m_out), m_indent * 2, ' ');
    }

    void CpuOpPrinter::newLoop(int size) {
      indent();
      m_out << "for i" << m_indent << " = 0.." << size - 1 << std::endl;
      m_indent++;
    }

    void CpuOpPrinter::endLoop() {
      m_indent--;
      indent();
      m_out << "end" << std::endl;
    }

    void CpuOpPrinter::op(const CpuOp &op) {
      indent();
      op.print(m_out);
    }
  }
}
