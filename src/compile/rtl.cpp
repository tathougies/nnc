#include "compile/rtl.hpp"
#include "compile/rtl_types.hpp"
#include "compile/rtl_ops.hpp"
#include "compile/rtl_type_factory_impl.hpp"
#include "compile/dump.hpp"

#include <cassert>
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
      assert(arg);
      if (!arg) {
        std::cerr << "Bad arg" << std::endl;
        exit(1);
      }
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

    bool RtlBasicBlock::has_jump_to(const RtlBlockName &to) const {
      auto call(std::find_if(jumps().begin(),
                             jumps().end(),
                             [&](const auto &j) {
                               return j.second.to() == to; }));
      return call != jumps().end();
    }

    RtlJump &RtlBasicBlock::get_jump_to(const RtlBlockName &to) {
      auto call(std::find_if(jumps().begin(),
                             jumps().end(),
                             [&](const auto &j) {
                               return j.second.to() == to; }));
      return call->second;
    }

    const RtlJump &RtlBasicBlock::get_jump_to(const RtlBlockName &to) const {
      auto call(std::find_if(jumps().begin(),
                             jumps().end(),
                             [&](const auto &j) {
                               return j.second.to() == to; }));
      return call->second;
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
            if (srcIntType->width() < destFloatType->width()) {
              auto orig(from);
              from = function().variable(from->namePrefix(),
                                         function().types().intType(srcIntType->signedness(), destFloatType->width()));
              if (srcIntType->signedness() == RtlUnsigned) {
                emplace_op<RtlZeroExtendOp>(orig, from);
              } else if (srcIntType->signedness() == RtlSigned) {
                emplace_op<RtlSignExtendOp>(orig, from);
              }
            } else if (srcIntType->width() > destFloatType->width()) {
              throw std::runtime_error("Can't do cast from larger int to smaller float");
            }

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

    RtlBasicBlock::jumps_iterator RtlBasicBlock::jumps() {
      return jumps_iterator(m_jumps);
    }

    RtlBasicBlock::const_jumps_iterator RtlBasicBlock::jumps() const {
      return const_jumps_iterator(m_jumps);
    }

    bool RtlBasicBlock::isEntryPoint() const {
      return function().entry().name() == name();
    }

    RtlBlockName RtlBlockName::first() {
      return RtlBlockName(0);
    }

    RtlBlockName RtlBlockName::last() {
      return RtlBlockName(INT_MAX);
    }

    RtlFunction::RtlFunction(const std::string &name)
      : m_functionName(name),
        m_entry(m_blocks.end()),
        m_types(typesForCurrentArch()) {
    }

    RtlFunction::~RtlFunction() {
    }

    RtlBasicBlock &RtlFunction::block(bool entry) {
      RtlBlockName nm(m_blocks.size());
      auto &blk(m_blocks.emplace_back(*this, nm));

      if ( m_entry == m_blocks.end() || entry ) {
        m_entry = m_blocks.end();
        -- m_entry;
      }

      return blk;
    }

    const RtlBasicBlock &RtlFunction::block(const RtlBlockName &blockName) const {
      auto it(m_blocks.begin());
      std::advance(it, blockName.index());
      return *it;
    }

    RtlBasicBlock &RtlFunction::block(const RtlBlockName &blockName) {
      auto it(m_blocks.begin());
      std::advance(it, blockName.index());
      return *it;
    }

    decltype(RtlFunction::m_blocks)::iterator RtlFunction::findBlock(const RtlBlockName &nm) {
      auto it(m_blocks.begin());
      std::advance(it, nm.index());
      return it;
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
      RtlBasicBlockDumper dump(out);
      dump.dump(*this);
    }
  }
}

