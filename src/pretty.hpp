#ifndef __nnc_pretty_HPP__
#define __nnc_pretty_HPP__

#include <iostream>
#include <iterator>

#include "tensor_util.hpp"

namespace nnc {
  template<typename Tensor>
  class Pretty {
  public:
    Pretty(Tensor &t) : m_tensor(t) { }

    inline Tensor &tensor() const { return m_tensor; }

    int maxInDimension(int dim) const {
      return -1;
    }

  private:
    Tensor &m_tensor;
  };

  template<typename Tensor>
  Pretty<Tensor> pretty(Tensor &t) { return Pretty<Tensor>(t); }

  template<typename Tensor>
  std::ostream &operator<<(std::ostream &out, const Pretty<Tensor> &p) {
    int dims(p.tensor().shape().dims());
    typename Tensor::view_type view(p.tensor().view());

    std::vector<int> ix(dims);
    std::fill(ix.begin(), ix.end(), 0);

    std::fill_n(std::ostream_iterator<char>(out), dims, '[');

    for (;;) {
      int nextDim;
      out << (int)view.index(view.transformIndex(ix.data()));

      nextDim = nextIndex(p.tensor(), ix);
      std::fill_n(std::ostream_iterator<char>(out), nextDim, ']');
      out << ",";
      if ( nextDim > 0 ) {
        out << std::endl;
        std::fill_n(std::ostream_iterator<char>(out), dims - nextDim, ' ');
      }
      std::fill_n(std::ostream_iterator<char>(out), nextDim, '[');
    }

    std::fill_n(std::ostream_iterator<char>(out), dims, ']');

    return out;
  }
}

#endif
