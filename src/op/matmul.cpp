#include "op/matmul.hpp"

#include <iostream>

namespace nnc {
  namespace error {
    class NotEnoughDimsForMatMul : public Error {
    public:
      NotEnoughDimsForMatMul(const std::string &which, int dims)
        : m_which(which), m_dims(dims) {
      }
      virtual ~NotEnoughDimsForMatMul() {
      }

      virtual std::ostream &report(std::ostream &o) const {
        o << "Matrix multiplications require at least two dimensions, but argument '"
          << m_which << "' has only " << m_dims;

        return o;
      }

    private:
      std::string m_which;
      int m_dims;
    };

    class MatrixSizeIncompatible : public Error {
    public:
      MatrixSizeIncompatible(int lrows, int lcols,
                             int rrows, int rcols)
        : m_lrows(lrows), m_lcols(lcols),
          m_rrows(rrows), m_rcols(rcols) {
      }
      virtual ~MatrixSizeIncompatible() {
      }

      virtual std::ostream &report(std::ostream &o) const {
        o << "Incompatible sizes for matrix multiplication" << std::endl;
        o << "  Argument 'left'  has size " << m_lrows << " x " << m_lcols << std::endl;
        o << "  Argument 'right' has size " << m_rrows << " x " << m_rcols;
        return o;
      }

    private:
      int m_lrows, m_lcols, m_rrows, m_rcols;
    };
  }

  namespace op {
    MatMul::MatMul(Tensor &a, Tensor &b)
      : m_left(&a), m_right(&b),
        m_dataType(nullptr),
        m_rows(-1), m_cols(-1) {
    }

    MatMul::MatMul()
      : m_left(nullptr), m_right(nullptr),
        m_dataType(nullptr),
        m_rows(-1), m_cols(-1) {
    }

    MatMul::~MatMul() {
    }

    bool MatMul::hasInput(const std::string &name) const {
      if ( name == "left" ) return m_left;
      else if ( name == "right" ) return m_right;
      else throw exception::InputDoesNotExist(name);
    }

    Tensor &MatMul::left() const {
      if ( !m_left ) throw exception::OperationInputNotReady("left");
      return *m_left;
    }

    Tensor &MatMul::right() const {
      if ( !m_right ) throw exception::OperationInputNotReady("right");
      return *m_right;
    }

    Tensor &MatMul::input(const std::string &name) {
      if ( name == "left" ) return left();
      else if ( name == "right" ) return right();
      else throw exception::InputDoesNotExist(name);
    }

    void MatMul::inputs(InputVisitor &v) const {
      v.input("left", "The left hand side of the matrix multiplication");
      v.input("right", "The right hand side of the matrix multiplication");
    }

    void MatMul::check(error::ErrorReporter &e) {
      if ( !m_left )
        e.reportError(std::make_shared<exception::OperationInputNotReady>("left"));

      if ( !m_right )
        e.reportError(std::make_shared<exception::OperationInputNotReady>("right"));

      if ( m_left && m_right ) {
        if ( m_left->dataType() != m_right->dataType() ) {
          e.reportError(std::make_shared<error::DataTypeMismatch>("left", m_left->dataType(),
                                                                  "right", m_right->dataType()));
          return;
        }

        m_dataType = &m_left->dataType();

        // Validate shapes
        if ( m_left->shape().dims() < 2 )
          e.reportError(std::make_shared<error::NotEnoughDimsForMatMul>("left", m_left->shape().dims()));

        if ( m_right->shape().dims() < 2 )
          e.reportError(std::make_shared<error::NotEnoughDimsForMatMul>("right", m_right->shape().dims()));

        if ( m_left->shape().dims() < 2 ||
             m_right->shape().dims() < 2 )
          return;

        if ( m_left->shape().dims() != m_right->shape().dims() ) {
          e.reportError(std::make_shared<error::DimensionMismatch>("matmul",
                                                                   "left", m_left->shape().dims(),
                                                                   "right", m_right->shape().dims()));
          return;
        }

        std::vector<int> extraDims;
        for ( int d = 2; d < m_left->shape().dims(); ++d ) {
          if ( m_left->shape().sizeInDimension(d) != m_right->shape().sizeInDimension(d) ) {
            e.reportError(std::make_shared<error::SizeMismatch>("left", m_left->shape(),
                                                                "right", m_right->shape(),
                                                                d));
            return;
          }

          extraDims.push_back(m_left->shape().sizeInDimension(d));
        }

        int leftRows(m_left->shape().sizeInDimension(1));
        int leftCols(m_left->shape().sizeInDimension(0));
        int rightRows(m_right->shape().sizeInDimension(1));
        int rightCols(m_right->shape().sizeInDimension(0));

        if ( leftCols != rightRows ) {
          e.reportError(std::make_shared<error::MatrixSizeIncompatible>(leftRows, leftCols, rightRows, rightCols));
          return;
        }

        m_rows = leftRows;
        m_cols = rightCols;

        std::swap(m_nonMatrixDims, extraDims);
      }
    }

    const DataType &MatMul::dataType() const {
      if ( m_dataType ) return *m_dataType;
      else throw exception::OperationNotReady();
    }

    const TensorShape &MatMul::shape() const {
      if ( isReady() ) return *this;
      else throw exception::OperationNotReady();
    }

    int MatMul::dims() const {
      if ( m_rows > 0 && m_cols > 0 )
        return m_nonMatrixDims.size() + 2;
      else
        throw exception::OperationNotReady();
    }

    int MatMul::sizeInDimension(int dim) const {
      if ( m_rows > 0 && m_cols > 0 ) {
        if ( dim == 0 ) return m_cols;
        else if ( dim == 1 ) return m_rows;
        else if ( dim < dims() ) {
          return m_nonMatrixDims[dim - 2];
        } else
          throw exception::InvalidDimension(dim, dims());
      } else
        throw exception::OperationNotReady();
    }

    bool MatMul::isReady() const {
      return m_left && m_right && m_rows > 0 && m_cols > 0 && m_dataType;
    }

    void MatMul::compile(executor::BasicTensorOps &inputs,
                         std::shared_ptr<executor::BasicTensorInput> dest) {
      std::cerr << "Compiling matmul" << std::endl;
      auto loop(inputs.elementWise(shape()));
      std::vector<std::uint8_t> zero;

      dataType().zeroValue(zero);

      auto accum(loop->scalar("accum", dataType()));
      loop->constant(zero.data(), zero.size(), accum);

      std::vector< std::shared_ptr<executor::BasicTensorInput> > index;

      int innerDimension(m_left->shape().sizeInDimension(0));

      {
        auto sumLoop(loop->loop(innerDimension));

        auto product(sumLoop->scalar("prod", dataType()));

        // Left index, columns is the inner dimension
        auto left(sumLoop->index(inputs.input("left"),{ sumLoop->loopIndex(0), sumLoop->loopIndex(-3), sumLoop->loopIndex(-1) }));
        auto right(sumLoop->index(inputs.input("right"), { sumLoop->loopIndex(0), sumLoop->loopIndex(-1), sumLoop->loopIndex(-2) }));

        sumLoop->product(left, right, product);
        sumLoop->add(accum, product, accum);
      }

      loop->cast(dataType(), dataType(), accum, loop->fullyIndexed(dest));
    }
  }
}
