#include "compile/rtl.hpp"
#include "compile/rtl_types.hpp"
#include "compile/rtl_ops.hpp"
#include "compile/rtl_type_factory_impl.hpp"

#include <cctype>
#include <climits>
#include <experimental/iterator>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nnc {
  namespace exception {
    BadRtlCast::BadRtlCast(std::shared_ptr<compile::RtlType> from,
                           std::shared_ptr<compile::RtlType> to)
      : m_from(from), m_to(to) {
    }

    BadRtlCast::~BadRtlCast() {
    }

    const char *BadRtlCast::what() const noexcept {
      return "bad rtl cast";
    }

    DuplicateJump::DuplicateJump(const compile::RtlBlockName &to)
      : m_to(to) {
    }

    DuplicateJump::~DuplicateJump() {
    }

    const char *DuplicateJump::what() const noexcept {
      return "duplicate jump";
    }

    NoSuchJump::NoSuchJump(std::shared_ptr<compile::RtlVariable> cond)
      : m_cond(cond) {
    }

    NoSuchJump::~NoSuchJump() {
    }

    const char *NoSuchJump::what() const noexcept {
      return "no such jump";
    }
  }

  namespace compile {
    OperandPrinter::OperandPrinter(RtlFunction &fn, std::ostream &out)
      : RtlOperandVisitor(fn)
      , m_out(out), m_first(true) {
    }

    OperandPrinter::~OperandPrinter() {
    };

    void OperandPrinter::operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) {
        next();
        m_out << name;
        if ( input && output ) m_out << "<->";
        else if ( output ) m_out << "->";
        else m_out << "=";
        var->type()->output(m_out);
        m_out << " " << var->name();
      }

    void OperandPrinter::operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
        next();
        m_out << name << "=<";
        ty->output(m_out);

        m_out << ">[";
        std::uint8_t *c((std::uint8_t *) l);
        std::copy(c, c + lsz,
                  std::ostream_iterator<std::uint32_t>(m_out, " "));
        m_out << "]";
      }

    void OperandPrinter::operand(const std::string &name, const RtlBlockName &dest) {
      next();
      m_out << name << "=block" << dest.index();
    }


    void OperandPrinter::next() {
      if ( m_first ) { m_first = false; }
      else {
        m_out << ", ";
      }
    }

    RtlVariable::RtlVariable(const std::string &name,
                             std::shared_ptr<RtlType> type)
      : m_name(name), m_type(type) {
    }

    RtlVariable::~RtlVariable() {
    }

    std::string RtlVariable::namePrefix() const {
      std::string::const_iterator prefixEnd(m_name.end());
      prefixEnd--;

      while ( prefixEnd != m_name.begin() ) {
        if ( isdigit(*prefixEnd)  ) {
          prefixEnd--;
          continue;
        } else if ( *prefixEnd == '_' ) {
          return std::string(m_name.begin(), prefixEnd);
        } else {
          return m_name;
        }
      }
      return m_name;
    }

    std::shared_ptr<RtlVariable> RtlVariable::rawRepr() {
      if ( m_alias ) {
        auto next(m_alias->rawRepr());
        if ( next ) {
          m_alias = next;
          return rawRepr();
        }
        return m_alias;
      } else
        return nullptr;
    }

    std::shared_ptr<RtlVariable> RtlVariable::repr() {
      auto r(rawRepr());
      if ( r ) return r;
      else return shared_from_this();
    }

    void RtlVariable::alias(std::shared_ptr<RtlVariable> a) {
      auto self(shared_from_this());
      if ( self == a ) return;

      if ( self < a ) {
        a->repr()->m_alias = self;
      } else {
        self->repr()->m_alias = a;
      }
    }

    /* Jumps */

    RtlJump::RtlJump(const RtlBlockName &to)
      : m_to(to) {
    }

    void RtlJump::push_argument(std::shared_ptr<RtlVariable> arg) {
      m_args.push_back(arg);
    }

    std::shared_ptr<RtlVariable> RtlJump::operator[] (int i) const {
      if ( i < arity() ) return m_args[i];
      else return nullptr;
    }

    /* op builder */
    RtlOpBuilder::~RtlOpBuilder() {
    }

    /* Basic blocks */

    RtlBasicBlock::RtlBasicBlock(RtlFunction &fn, const RtlBlockName &nm)
      : m_function(fn), m_name(nm) {
    }

    RtlBasicBlock::~RtlBasicBlock() {
    }

    RtlFunction &RtlBasicBlock::function() const {
      return m_function;
    }

    RtlBasicBlock::iterator RtlBasicBlock::begin() {
      return m_ops.begin();
    }

    RtlBasicBlock::iterator RtlBasicBlock::end() {
      return m_ops.end();
    }

    RtlBasicBlock::const_iterator RtlBasicBlock::begin() const {
      return m_ops.begin();
    }

    RtlBasicBlock::const_iterator RtlBasicBlock::end() const {
      return m_ops.end();
    }

    RtlJump &RtlBasicBlock::jump(std::shared_ptr<RtlVariable> cond, const RtlBlockName &dest) {
      auto inserted(m_jumps.insert(std::make_pair(cond, dest)));

      if ( inserted.second ) {
        return inserted.first->second;
      } else
        throw exception::DuplicateJump(dest);
    }

    RtlJump &RtlBasicBlock::jump(std::shared_ptr<RtlVariable> cond) {
      auto it(m_jumps.find(cond));
      if ( it == m_jumps.end() )
        throw exception::NoSuchJump(cond);
      else
        return it->second;
    }

    bool RtlBasicBlock::has_jump(std::shared_ptr<RtlVariable> cond) const {
      return m_jumps.find(cond) != m_jumps.end();
    }

    std::shared_ptr<RtlVariable> RtlBasicBlock::addInput(const std::string &namePrefix,
                                                         std::shared_ptr<RtlType> type) {
      auto input(m_function.variable(namePrefix, type));
      m_inputs.push_back(input);
      return input;
    }

    void RtlBasicBlock::cast(std::shared_ptr<RtlVariable> from, std::shared_ptr<RtlVariable> to) {
      auto srcType(from->type());
      auto destType(to->type());

      if ( srcType == destType ) {
        from->alias(to);
        return;
      } else {
        auto srcIntType(std::dynamic_pointer_cast<RtlIntType>(srcType));
        auto destIntType(std::dynamic_pointer_cast<RtlIntType>(destType));

        if ( srcIntType && destIntType ) {
          if ( destIntType->width() < srcIntType->width() ) {
            // Truncation
            emplace_op<RtlTruncateOp>(from, to);
            return;
          }

          // Extensions
          if ( srcIntType->signedness() == RtlUnsigned ) {
            // Zero extension
            emplace_op<RtlZeroExtendOp>(from, to);
          } else if ( srcIntType->signedness() == RtlTwosComplement ) {
            // Signed extension
            emplace_op<RtlSignExtendOp>(from, to);
            return;
          } else
            throw exception::BadRtlCast(srcType, destType);
        } else if ( srcIntType ) {
          auto destFloatType(std::dynamic_pointer_cast<RtlFloatType>(destType));
          if ( destFloatType ) {
            // Int -> floating point conversion
            if ( srcIntType->signedness() == RtlUnsigned ) {
              emplace_op<RtlUiToFpOp>(from, to);
              return;
            } else if ( srcIntType->signedness() == RtlTwosComplement ) {
              emplace_op<RtlSiToFpOp>(from, to);
              return;
            } else
              throw exception::BadRtlCast(srcType, destType);
          }

          auto destPointerType(std::dynamic_pointer_cast<RtlPointerType>(destType));
          if ( destPointerType ) {
            emplace_op<RtlIntToPtrOp>(from, to);
            return;
          }
        } else if ( destIntType ) {
          auto srcFloatType(std::dynamic_pointer_cast<RtlFloatType>(srcType));
          if ( srcFloatType ) {
            if ( destIntType->signedness() == RtlUnsigned ) {
              emplace_op<RtlFpToUiOp>(from, to);
              return;
            } else if ( destIntType->signedness() == RtlTwosComplement ) {
              emplace_op<RtlFpToSiOp>(from, to);
              return;
            }
          }

          auto srcPointerType(std::dynamic_pointer_cast<RtlPointerType>(srcType));
          if ( srcPointerType ) {
            emplace_op<RtlPtrToIntOp>(from, to);
            return;
          }
        }

        auto srcFloatType(std::dynamic_pointer_cast<RtlFloatType>(srcType));
        auto destFloatType(std::dynamic_pointer_cast<RtlFloatType>(destType));

        if ( srcFloatType && destFloatType ) {
        }

        auto srcPointerType(std::dynamic_pointer_cast<RtlPointerType>(srcType));
        auto destPointerType(std::dynamic_pointer_cast<RtlPointerType>(destType));
        if ( srcPointerType && destPointerType ) {
          // Pointer -> pointer conversion involves pointer -> int, int -> pointer
          auto im(m_function.variable("im", m_function.types().intptr()));
          emplace_op<RtlPtrToIntOp>(from, im);
          emplace_op<RtlIntToPtrOp>(im, to);
          return;
        }
      }
    }

    void RtlBasicBlock::op(std::unique_ptr<RtlOp> &&op) {
      m_ops.push_back(std::move(op));
    }

    void RtlBasicBlock::clear() {
      m_ops.clear();
    }

    RtlJumpIterator RtlBasicBlock::jumps() {
      return RtlJumpIterator(*this, m_jumps.begin());
    }

    RtlJumpIterator::RtlJumpIterator(RtlBasicBlock &blk, const underlying &u)
      : m_block(blk), m_underlying(u), m_unconditional(blk.m_jumps.end()) {
      if ( m_underlying != m_block.m_jumps.end() ) {
        if ( ! m_underlying->first ) {
          m_unconditional = m_underlying;
          ++m_underlying;
        }
      }
    }

    RtlJumpIterator RtlJumpIterator::begin() {
      return m_block.jumps();
    }

    RtlJumpIterator RtlJumpIterator::end() {
      RtlJumpIterator r(m_block, m_block.m_jumps.end());;
      return r;
    }

    bool RtlJumpIterator::operator==(const RtlJumpIterator &i) const {
      return &m_block == &i.m_block && m_underlying == i.m_underlying &&
        m_unconditional == i.m_unconditional;
    }

    RtlJumpIterator &RtlJumpIterator::operator++() {
      if ( m_underlying != m_block.m_jumps.end() ) {
        ++m_underlying;
        if ( m_underlying != m_block.m_jumps.end() ) {
          if ( ! m_underlying->first ) {
            m_unconditional = m_underlying;
            ++m_underlying;
          }
        }
      } else if ( m_unconditional != m_block.m_jumps.end() ) {
        m_unconditional = m_block.m_jumps.end();
      }

      return *this;
    }

    RtlJumpIterator::value_type &RtlJumpIterator::operator*() {
      if ( m_underlying != m_block.m_jumps.end() ) { return *m_underlying; }
      else return *m_unconditional;
    }

    RtlJumpIterator::value_type *RtlJumpIterator::operator->() {
      return &operator*();
    }

    RtlBlockName RtlBlockName::first() {
      return RtlBlockName(0);
    }

    RtlBlockName RtlBlockName::last() {
      return RtlBlockName(INT_MAX);
    }

    RtlFunction::RtlFunction(const std::string &name)
      : m_functionName(name),
        m_types(typesForCurrentArch()) {
    }

    RtlFunction::~RtlFunction() {
    }

    std::shared_ptr<RtlBasicBlock> RtlFunction::block() {
      RtlBlockName nm(m_blocks.size());
      auto blk(std::make_shared<RtlBasicBlock>(*this, nm));
      m_blocks.push_back(blk);
      return blk;
    }

    std::shared_ptr<RtlBasicBlock> RtlFunction::block(const RtlBlockName &blockName) const {
      return m_blocks[blockName.index()];
    }

    std::shared_ptr<RtlVariable> RtlFunction::variable(const std::string &namePrefix,
                                                       std::shared_ptr<RtlType> type) {
      for (int i = 0;; ++i) {
        std::stringstream s;
        s << namePrefix << "_" << i;

        auto varName(s.str());
        auto it(m_variables.find(varName));

        if ( it == m_variables.end() ) {
          auto var(std::make_shared<RtlVariable>(varName, type));
          m_variables.insert(std::make_pair(varName, var));
          return var;
        }
      }
    }

    RtlTypeFactory &RtlFunction::types() {
      return *m_types;
    }

    void RtlFunction::dump(std::ostream &out) {
      out << "Function " << m_functionName << std::endl;
      out << "Basic block count: " << m_blocks.size() << std::endl;

      for ( const auto &blockInfo : m_blocks ) {
        out << "block" << blockInfo->name().index() << "(";
        bool comma(false);
        for ( const auto &i : blockInfo->m_inputs ) {
          if ( comma ) out << ", ";
          out << "(";
          i->type()->output(out);
          out << ") " << i->name();
          comma = true;
        }
        out << "):" << std::endl;

        for ( const auto &op : *blockInfo ) {
          OperandPrinter p(*this, out);

          out << "  " << op->mnemonic() << "(";
          op->operands(p);
          out << ")" << std::endl;
        }

        for ( const auto &jump : blockInfo->jumps() ) {
          if ( jump.first ) {
            out << "  call block" << jump.second.to().index() << "(";
            std::transform(jump.second.arguments().begin(), jump.second.arguments().end(),
                           std::ostream_iterator<std::string>(out, ", "),
                           [] (const std::shared_ptr<RtlVariable> &var) {
                             std::stringstream ss;
                             ss << "(";
                             var->type()->output(ss);
                             ss << ") " << var->name();
                             return ss.str();
                           });
            out << ") when " << jump.first->name() << std::endl;
          }
        }

        if ( blockInfo->has_jump() ) {
          const auto &jump(blockInfo->jump());
          out << "  call block" << jump.to().index() << "(";
          std::transform(jump.arguments().begin(), jump.arguments().end(),
                           std::ostream_iterator<std::string>(out, ", "),
                           [] (const std::shared_ptr<RtlVariable> &var) {
                             std::stringstream ss;
                             ss << "(";
                             var->type()->output(ss);
                             ss << ") " << var->name();
                             return ss.str();
                           });
          out << ")" << std::endl;
        } else
          out << "  return" << std::endl;
      }
    }

    RtlBlockIterator RtlFunction::blocks() {
      return RtlBlockIterator(*this, 0);
    }

    RtlBlockIterator::RtlBlockIterator(RtlFunction &fn, int i)
      : m_function(fn), m_index(i) {
    }

    RtlBlockIterator::RtlBlockIterator(const RtlBlockIterator &o)
      : m_function(o.m_function), m_index(o.m_index) {
    }

    std::pair< RtlBlockName, std::shared_ptr<RtlBasicBlock> > RtlBlockIterator::operator*() const {
        return std::make_pair(RtlBlockName(m_index), m_function.m_blocks[m_index]);
    }

    bool RtlBlockIterator::operator==(const RtlBlockIterator &i) const {
      return (&m_function) == (&i.m_function) &&
        m_index == i.m_index;
    }

    bool RtlBlockIterator::operator<(const RtlBlockIterator &i) const {
      return (&m_function) == (&i.m_function) &&
        m_index < i.m_index;
    }

    bool RtlBlockIterator::operator>(const RtlBlockIterator &i) const {
      return (&m_function) == (&i.m_function) &&
        m_index > i.m_index;
    }

    RtlBlockIterator RtlBlockIterator::operator+(int i) const {
      auto nextIndex(m_index + i);
      if ( nextIndex < 0 )
        return begin();
      else if ( nextIndex >= m_function.m_blocks.size() )
        return end();
      else
        return RtlBlockIterator(m_function, nextIndex);
    }

    RtlBlockIterator &RtlBlockIterator::operator++() {
      RtlBlockIterator next(*this + 1);
      swap(next);
      return *this;
    }

    RtlBlockIterator RtlBlockIterator::operator++(int i) {
      RtlBlockIterator next(*this);
      ++(*this);
      return next;
    }

    void RtlBlockIterator::swap(RtlBlockIterator &i) {
      std::swap(i.m_index, m_index);
    }

    RtlBlockIterator RtlBlockIterator::begin() const {
      return RtlBlockIterator(m_function, 0);
    }

    RtlBlockIterator RtlBlockIterator::end() const {
      return RtlBlockIterator(m_function, m_function.m_blocks.size());
    }

    void dumpOp(RtlOp &o, RtlFunction &f, std::ostream &out) {
      OperandPrinter p(f, out);
      out << o.mnemonic() << "(";
      o.operands(p);
      out << ")";
    }
  }
}
