#ifndef __nnc_reorder_HPP__
#define __nnc_reorder_HPP__

namespace nnc::compile {
  // Reorders instructions so that all instructions producing data occur before any consuming them
  void reorderDataDeps(RtlBasicBlock &blk);
  void reorderDataDeps(RtlFunction &fn);
}

#endif
