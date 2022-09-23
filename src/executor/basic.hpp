#ifndef __nnc_basic_HPP__
#define __nnc_basic_HPP__

#include "tensor.hpp"

#include <memory>
#include <string>

namespace nnc {
  namespace executor {
    class BasicTensorInput {
    public:
      BasicTensorInput(const std::string &baseName);
      virtual ~BasicTensorInput();

      virtual bool isScalar() const =0;
      virtual int dims() const =0;
      virtual const DataType &dataType() const =0;

      inline const std::string &baseName() const { return m_baseName; }

    private:
      std::string m_baseName;
    };

    typedef std::shared_ptr<BasicTensorInput> BasicTensorInputPtr;

    class BasicTensorOps {
    public:
      virtual ~BasicTensorOps();

      virtual std::shared_ptr<BasicTensorInput> input(const std::string &name) const =0;
      virtual std::shared_ptr<BasicTensorInput> index(const std::shared_ptr<BasicTensorInput> &base,
                                                      const std::vector< std::shared_ptr<BasicTensorInput> > &ix) const =0;

      virtual int loopCount() const =0;
      virtual std::shared_ptr<BasicTensorInput> loopIndex(int dim) const =0;

      virtual std::shared_ptr<BasicTensorOps> loop(int size) =0;

      virtual void cast(const DataType &from, const DataType &to,
                        const std::shared_ptr<BasicTensorInput> &src,
                        const std::shared_ptr<BasicTensorInput> &dest) =0;
      virtual void constant(void *d, std::size_t sz,
                            const std::shared_ptr<BasicTensorInput> &dest) =0;

      virtual void add(const BasicTensorInputPtr &a,
                       const BasicTensorInputPtr &b,
                       const BasicTensorInputPtr &dest) =0;
      virtual void product(const BasicTensorInputPtr &a,
                           const BasicTensorInputPtr &b,
                           const BasicTensorInputPtr &dest) =0;
      virtual void reshape(const TensorShape &shape,
                           const BasicTensorInputPtr &a,
                           const BasicTensorInputPtr &dest) =0;

      virtual std::shared_ptr<BasicTensorOps> outerLoop() =0;

      std::shared_ptr<BasicTensorOps> elementWise(const TensorShape &shp);
      virtual std::shared_ptr<BasicTensorInput> fullyIndexed(const std::shared_ptr<BasicTensorInput> &base) const;
      void fullIndex(std::vector< std::shared_ptr<BasicTensorInput> > &ix) const;

      virtual BasicTensorInputPtr tensor(const DataType &dt, const TensorShape &sh) const =0;
      virtual BasicTensorInputPtr scalar(const std::string &basename, const DataType &dt) const;
    };

    class BasicTensorOp {
    public:
      virtual ~BasicTensorOp();

      virtual void compile(BasicTensorOps &inputs,
                           std::shared_ptr<BasicTensorInput> dest) =0;
    };
  }
}

#endif
