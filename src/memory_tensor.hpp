#ifndef __memory_tensor_HPP__
#define __memory_tensor_HPP__

#include "tensor.hpp"
#include "data_type.hpp"
#include "exception.hpp"
#include "tensor_util.hpp"

#include <array>
#include <vector>
#include <typeinfo>
#include <numeric>
#include <algorithm>

namespace nnc {
  template<int... ds>
  class ConstantLayout;

  template<int dim, int... ds>
  class ConstantLayout<dim, ds...> : public TensorShape {
  public:
    virtual ~ConstantLayout() {};

    virtual int dims() const { return ConstantLayout<ds...>().dims() + 1; }
    virtual int sizeInDimension(int d) const {
      if ( d == dims() - 1 ) return dim;
      else {
        ConstantLayout<ds...> next;
        try {
          return next.sizeInDimension(d);
        } catch (const exception::InvalidDimension &) {
          throw exception::InvalidDimension(d, dims());
        }
      }
    }
  };

  template<int dim>
  class ConstantLayout<dim> : public TensorShape {
  public:
    virtual ~ConstantLayout() {};

    virtual int dims() const { return 1; }
    virtual int sizeInDimension(int d) const {
      if ( d == 0 ) return dim;
      else throw exception::InvalidDimension(d, 1);
    }
  };

  template<>
  class ConstantLayout<> : public TensorShape {
  public:
    virtual ~ConstantLayout () { }

    virtual int dims() const { return 0; }
    virtual int sizeInDimension(int d) const {
      throw exception::InvalidDimension(d, 0);
    }
    virtual int totalSize() const { return 1; }
  };

  template<int d, int... ds>
  struct TotalSize {
    static constexpr int value = d * TotalSize<ds...>::value;
  };

  template<int d>
  struct TotalSize<d> {
    static constexpr int value = d;
  };

  template<typename T, typename Allocator = std::allocator<T>, int... ds>
  class ConstTensor : public Tensor, public TensorView<T> {
  public:
    typedef T value_type;

    ConstTensor(const std::initializer_list<T> &inits, const Allocator &alloc = Allocator())
      : m_data(TotalSize<ds...>::value, alloc) {
      std::fill(m_data.begin(), m_data.end(), 0);
      std::copy(inits.begin(), inits.begin() + std::min(inits.size(), m_data.size()), m_data.begin());
    }

    ConstTensor(const Allocator &alloc = Allocator())
      : m_data(TotalSize<ds...>::value, alloc) {
      std::fill(m_data.begin(), m_data.end(), 0);
    }

    virtual ~ConstTensor() {};

    virtual const TensorShape &shape() const { return m_layout; }
    virtual const DataType &dataType() const { return m_type; }

    virtual size_t transformIndex(const int *dims) const {
      return standardShapeIndex(shape(), dims);
    }

    virtual T index(size_t ix) const { return m_data[ix]; }

  private:
    ConstantLayout<ds...> m_layout;
    DefaultDataType<T> m_type;

    std::vector<T, Allocator> m_data;
  };

  template<int d>
  class InMemoryLayout : public TensorShape, public std::array<int, d> {
  public:
    InMemoryLayout(const std::array<int, d> &sizes)
      : std::array<int, d>(sizes) {
      std::reverse(std::begin(*this), std::end(*this));
    }
    InMemoryLayout() {
      std::fill(this->begin(), this->end(), 0);
    }

    InMemoryLayout(InMemoryLayout &&layout)
      : std::array<int, d>(std::move(layout)) {
    }

    virtual int dims() const { return d; }
    virtual int sizeInDimension(int i) const {
      if ( i < dims() ) return (*this)[i];
      else throw exception::InvalidDimension(i, d);
    }
  };

  template<typename T, int d, typename Allocator = std::allocator<T>>
  class InMemoryTensor : public Tensor, public TensorView<T> {
  public:
    typedef T value_type;
    typedef InMemoryTensor<T, d, Allocator> &view_type;

    InMemoryTensor(const std::array<int, d> &sizes, const Allocator &alloc = Allocator())
      : m_data(calcSize(sizes), alloc),
        m_layout(sizes) {
      std::fill(m_data.begin(), m_data.end(), 0);
    }

    InMemoryTensor(const Allocator &alloc = Allocator())
      : m_data(alloc) {
    }

    virtual const TensorShape &shape() const { return m_layout; }
    virtual const DataType &dataType() const { return m_type; }

    virtual size_t transformIndex(const int *dims) const {
      return standardShapeIndex(shape(), dims);
    }

    virtual T index(size_t ix) const { return m_data[ix]; }

    // Views
    view_type view() { return *this; }

    void resize(const std::array<int, d> &sizes) {
      std::vector<T, Allocator> new_data(calcSize(sizes), m_data.get_allocator());
      InMemoryLayout<d> new_layout(sizes);

      std::fill(new_data.begin(), new_data.end(), 0);
      std::swap(new_data, m_data);
      std::swap(new_layout, m_layout);
    }

  protected:
    int calcSize(const std::array<int, d> &sizes) {
      return std::accumulate(sizes.begin(), sizes.end(), 1,
                             [] (int a, int b) { return a * b; });
    }

    std::vector<T, Allocator> m_data;

  private:
    InMemoryLayout<d> m_layout;
    DefaultDataType<T> m_type;
  };
}

#endif
