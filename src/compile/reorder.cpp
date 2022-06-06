#include "compile/reorder.hpp"

namespace nnc::compile {
  void reorderDataDeps(RtlFunction &fn) {
    for ( const auto &block : fn.blocks() ) {
      reorderDataDeps(*block.second);
    }
  }

  void reorderDataDeps(RtlBasicBlock &blk) {
    // For each operation, calculate all variables it produces
    //
    // Then, for any impure operation, or any argument to a block
    // jump, make sure the operation is inserted.

    std::list<std::unique_ptr<RtlOp>> ops;
    std::map<std::shared_ptr<RtlVariable>, decltype(ops)::iterator> producers;

    for ( auto &op: blk )
      ops.emplace_back(std::move(op));

    blk.clear();

    for ( auto op(ops.begin()); op != ops.end(); ++op ) {
      reorder::OutputCollector c;
      // Add the outputs of this operator to the producers list
      op->operands(c);

      for ( const auto &output: c.outputs() ) {
        producers.insert(std::make_pair(output, op));
      }
    }

    for ( const auto &op: ops ) {
      if ( !op.isPure() ) {
        reorder::InputCollector c;
        op.operands(c);

        // Collect the inputs
        for ( const auto &input: c.inputs() ) {
          
        }
      }
    }

    for ( ) {
      //Check for all arguments in the jump or the condition
    }
  }
}
