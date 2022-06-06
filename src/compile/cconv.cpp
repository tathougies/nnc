#include "compile/cconv.hpp"

namespace nnc::compile {
  CallingConvention::~CallingConvention() {
  }

  VirtualRegister CallingConvention::regForArg(const RtlVariablePtr &v) {
    auto it(std::find_if(m_args.begin(), m_args.end(),
                         [v](const auto &slot) { return slot.first == v; }));
    if ( it == m_args.end() ) {
      VirtualRegister reg(assignRegToArg(v));
      m_args.emplace_back(v, reg);
      return reg;
    } else
      return it->second;
  }

  void CallingConvention::buildPreamble(RtlOpBuilder &b) const {
  }

  void CallingConvention::buildPostamble(RtlOpBuilder &b) const {
  }

  void CallingConvention::operator() (RtlBasicBlock &blk) {
    for ( const auto &var: blk.inputs() )
      regForArg(var);
  }
}
