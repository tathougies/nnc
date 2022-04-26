#include "tensor.hpp"
#include "exception.hpp"

namespace nnc {
  DataType::~DataType() {
  }

  bool DataType::operator!=(const DataType &dt) const {
    return !(*this == dt);
  }

  TensorShape::~TensorShape() {
  }

  int TensorShape::totalSize() const {
    int ret = 1;
    for ( int i = 0; i < dims(); ++i )
      ret *= sizeInDimension(i);
    return ret;
  }

  std::vector<int> TensorShape::asVector() const {
    std::vector<int> ret(dims());

    for ( int i = 0; i < dims(); ++i )
      ret[i] = sizeInDimension(i);

    return ret;
  }

  bool TensorShape::operator==(const TensorShape &other) const {
    if ( dims() == other.dims() ) {
      for ( int i = 0; i < dims(); ++i ) {
        if ( sizeInDimension(i) != other.sizeInDimension(i) ) return false;
      }
      return true;
    } else
      return false;
  }

  int ScalarShape::dims() const {
    return 0;
  }

  int ScalarShape::sizeInDimension(int dim) const {
    throw exception::InvalidDimension(dim, 0);
    return 1;
  }

  ScalarShape::~ScalarShape() {
  }

  Tensor::~Tensor() {
  }

  size_t standardShapeIndex(const TensorShape &shape, const int *ix) {
    size_t ret = 0, stride = 1;

    for ( int dim = 0;
          dim < shape.dims();
          ++dim ) {
      ret += ix[dim] * stride;
      stride *= shape.sizeInDimension(dim);
    }

    return ret;
  }
}
