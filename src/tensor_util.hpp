#ifndef __nnc_tensor_util_HPP__
#define __nnc_tensor_util_HPP__

#include "tensor.hpp"

#include <iostream>

namespace nnc {
  template<typename T>
  int nextIndex(const Tensor &t, T &index) {
    const TensorShape &shape(t.shape());
    int dim = 0;

    for ( dim = 0; dim < shape.dims(); ++dim ) {
      index[dim]++;
      if ( index[dim] > shape.sizeInDimension(dim) ) {
        index[dim] = 0;
      } else
        break;
    }

    return dim;
  }

  size_t standardShapeIndex(const TensorShape &shape, const int *ix);
}

#endif
