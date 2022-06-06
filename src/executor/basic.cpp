#include "executor/basic.hpp"

#include <iostream>

namespace nnc {
  namespace executor {
    BasicTensorInput::~BasicTensorInput() {
    }

    BasicTensorOps::~BasicTensorOps() {
    }

    BasicTensorOp::~BasicTensorOp() {
    }

    std::shared_ptr<BasicTensorOps> BasicTensorOps::elementWise(const TensorShape &shp) {
      std::shared_ptr<BasicTensorOps> loop(outerLoop());
      int dims(shp.dims());

      std::cerr << "element wise " << dims << std::endl;
      for ( int i = 0; i < dims; ++i ) {
        loop = loop->loop(shp.sizeInDimension(i));
      }

      return loop;
    }

    void BasicTensorOps::fullIndex(std::vector< std::shared_ptr<BasicTensorInput> > &ix) const {
      int dims(loopCount());
      for ( int i = 0; i < dims; ++i )
        ix.push_back(loopIndex(i));
    }

    std::shared_ptr<BasicTensorInput> BasicTensorOps::fullyIndexed(const std::shared_ptr<BasicTensorInput> &base) const {
      std::vector< std::shared_ptr<BasicTensorInput> > ix;

      fullIndex(ix);

      //      if ( base->dims() < loopCount() )
      //        ix.erase(ix.begin(), ix.end() - base->dims());

      return index(base, ix);
    }

    BasicTensorInputPtr BasicTensorOps::scalar(const DataType &dt) const {
      ScalarShape sh;
      auto t(tensor(dt, sh));
      return fullyIndexed(t);
    }
  }
}
