#ifndef __tensor_HPP__
#define __tensor_HPP__

#include <string>
#include <vector>
#include <array>

#include "compile/rtl_types.hpp"

namespace nnc {
  namespace compile {
    class RtlTypeFactory;
  }

  class DataType {
  public:
    virtual ~DataType();

    virtual const std::string name() const =0;
    virtual const std::string prettyName() const =0;
    virtual size_t size() const =0;

    virtual std::shared_ptr<compile::RtlType> rtlType(compile::RtlTypeFactory &fn) const =0;

    virtual bool operator==(const DataType &dt) const =0;
    virtual bool operator!=(const DataType &dt) const;

    virtual void zeroValue(std::vector<std::uint8_t> &d) const =0;
  };

  class TensorShape {
  public:
    virtual ~TensorShape();

    virtual int dims() const =0;
    virtual int sizeInDimension(int dim) const =0;

    virtual int totalSize() const;

    bool operator==(const TensorShape &other) const;
    inline bool operator!=(const TensorShape &s) const { return !(*this == s); }

    std::vector<int> asVector() const;
  };

  class ScalarShape : public TensorShape {
  public:
    virtual ~ScalarShape();

    virtual int dims() const;
    virtual int sizeInDimension(int dim) const;
  };

  class Tensor {
  public:
    virtual ~Tensor();

    virtual const TensorShape &shape() const =0;
    virtual const DataType &dataType() const =0;
  };

  template<typename T>
  class TensorView {
  public:
    inline size_t transformIndex(const std::initializer_list<int> &ix) const {
      std::vector<int> ixv(ix);
      return transformIndex(ixv);
    }

    inline size_t transformIndex(const std::vector<int> &ix) const {
      return transformIndex(ix.data());
    }

    /**
     * Given a list of integers representing the index into the tensor
     * (presumed to be large enough to fit the dimension), get the
     * integer index that can be passed to index() to lookup the value
     */
    virtual size_t transformIndex(const int *dims) const =0;

    virtual T index(size_t ix) const =0;
  };
}

#endif
