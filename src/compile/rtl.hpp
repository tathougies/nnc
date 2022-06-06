#ifndef __nnc_compile_rtl_HPP__
#define __nnc_compile_rtl_HPP__

#include <memory>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "compile/rtl_types.hpp"
#include "compile/rtl_ops_base.hpp"

namespace nnc {
  namespace compile {
    class RtlBlockName {
    public:
      inline int index() const { return m_name; }

      inline bool operator<(const RtlBlockName &o) const { return m_name < o.m_name; }
      inline bool operator>(const RtlBlockName &o) const { return m_name > o.m_name; }
      inline bool operator<=(const RtlBlockName &o) const { return m_name <= o.m_name; }
      inline bool operator>=(const RtlBlockName &o) const { return m_name >= o.m_name; }
      inline bool operator==(const RtlBlockName &o) const { return m_name == o.m_name; }
      inline bool operator!=(const RtlBlockName &o) const { return m_name != o.m_name; }

      static RtlBlockName first();
      static RtlBlockName last();

    private:
      inline RtlBlockName(int nm) : m_name(nm) { }

      int m_name;

      friend class RtlFunction;
      friend class RtlBlockIterator;
    };
  }

  namespace exception {
    class BadRtlCast : public std::exception {
    public:
      BadRtlCast(std::shared_ptr<compile::RtlType> from,
                 std::shared_ptr<compile::RtlType> to);
      virtual ~BadRtlCast();

      virtual const char *what() const noexcept;

      inline std::shared_ptr<compile::RtlType> from() const { return m_from; }
      inline std::shared_ptr<compile::RtlType> to() const { return m_to; }

    private:
      std::shared_ptr<compile::RtlType> m_from, m_to;
    };

    class DuplicateJump : public std::exception {
    public:
      DuplicateJump(const compile::RtlBlockName &to);
      virtual ~DuplicateJump();

      virtual const char *what() const noexcept;

      inline const compile::RtlBlockName &to() const { return m_to; }

    private:
      compile::RtlBlockName m_to;
    };

    class NoSuchJump : public std::exception {
    public:
      NoSuchJump(std::shared_ptr<compile::RtlVariable> cond);
      virtual ~NoSuchJump();

      virtual const char *what() const noexcept;

      inline std::shared_ptr<compile::RtlVariable> cond() const { return m_cond; }

    private:
      std::shared_ptr<compile::RtlVariable> m_cond;
    };
  }

  namespace compile {
    class RtlFunction;
    class RtlRegisterMapper;

    class RtlJump {
    public:
      typedef std::vector< std::shared_ptr<RtlVariable> > args_type;

      RtlJump(const RtlBlockName &to);
      RtlJump(RtlJump &&j) =default;
      RtlJump(const RtlJump &j) =default;

      void push_argument(std::shared_ptr<RtlVariable> arg);

      inline std::size_t arity() const { return m_args.size(); }
      std::shared_ptr<RtlVariable> operator[] (int i) const;

      inline const RtlBlockName &to() const { return m_to; }

      inline const args_type &arguments() const { return m_args; }
      inline args_type &arguments() { return m_args; }
      inline void clear_arguments() { m_args.clear(); }

    private:
      RtlBlockName m_to;
      args_type m_args;
    };

    class RtlBasicBlock;

    class RtlJumpIterator {
    public:
      RtlJumpIterator(const RtlJumpIterator &o) =default;
      RtlJumpIterator(RtlJumpIterator &&o) =default;

      typedef std::pair< const std::shared_ptr<RtlVariable>, RtlJump > value_type;
      typedef int difference_type;
      typedef std::pair< const std::shared_ptr<RtlVariable>, RtlJump > &reference;
      typedef std::pair< const std::shared_ptr<RtlVariable>, RtlJump > *pointer;
      typedef RtlJumpIterator iterator;
      typedef std::input_iterator_tag iterator_category;

      RtlJumpIterator begin();
      RtlJumpIterator end();

      bool operator==(const RtlJumpIterator &i) const;
      inline bool operator!=(const RtlJumpIterator &i) const { return !(*this == i); }

      inline RtlJumpIterator operator++(int i) {
        RtlJumpIterator r(*this);
        ++(*this);
        return r;
      }
      RtlJumpIterator &operator++();

      value_type &operator*();
      value_type *operator->();

      RtlJumpIterator &operator=(const RtlJumpIterator &o) =default;
      RtlJumpIterator &operator=(RtlJumpIterator &&o) =default;

    private:
      typedef std::map< std::shared_ptr<RtlVariable>, RtlJump >::iterator underlying;

      RtlJumpIterator(RtlBasicBlock &blk, const underlying &u);

      RtlBasicBlock &m_block;

      underlying m_underlying, m_unconditional;

      friend class RtlBasicBlock;
    };

    class RtlOpBuilder {
    public:
      virtual ~RtlOpBuilder();
      virtual RtlFunction &function() const =0;

      virtual void clear() =0;
      virtual void op(std::unique_ptr<RtlOp> &&op) =0;

      template<typename It>
      void replaceOps(const It &begin, const It &end) {
        clear();
        addOps(begin, end);
      }

      template<typename It>
      void addOps(const It &begin, const It &end) {
        for ( It i(begin); i != end; ++i ) {
          op(std::move(*i));
        }
      }

      template<typename T, typename... Args>
      T &emplace_op(Args &&... args) {
        auto newOp(std::make_unique<T>(std::forward<Args>(args)...));
        T &ret(*newOp);
        op(std::move(newOp));
        return ret;
      }
    };

    class RtlBasicBlock : public RtlOpBuilder {
    public:
      RtlBasicBlock(RtlFunction &fn, const RtlBlockName &nm);
      virtual ~RtlBasicBlock();

      inline RtlBlockName name() const { return m_name; }
      virtual RtlFunction &function() const;

      virtual void op(std::unique_ptr<RtlOp> &&op);
      virtual void clear();

      inline int endTime() const { return m_ops.size(); }

      // Add input. Marks the given variable as having
      std::shared_ptr<RtlVariable> addInput(const std::string &namePrefix,
                                            std::shared_ptr<RtlType> type);


      void cast(std::shared_ptr<RtlVariable> from, std::shared_ptr<RtlVariable> to);

      typedef std::list< std::unique_ptr<RtlOp> >::iterator iterator;
      iterator begin();
      iterator end();

      typedef std::list< std::unique_ptr<RtlOp> >::const_iterator const_iterator;
      const_iterator begin() const;
      const_iterator end() const;

      RtlJumpIterator jumps();

      typedef std::vector< std::shared_ptr<RtlVariable> > inputs_type;
      inline const inputs_type &inputs() const { return m_inputs; }
      inline std::size_t arity() const { return inputs().size(); }

      inline void replace_jumps(std::map<std::shared_ptr<RtlVariable>, RtlJump> newJumps) {
        m_jumps = std::move(newJumps);
      }

      inline RtlJump &jump(const RtlBlockName &to) { return jump(nullptr, to); }
      RtlJump &jump(std::shared_ptr<RtlVariable> cond, const RtlBlockName &to);

      inline RtlJump &jump() { return jump(nullptr); }
      RtlJump &jump(std::shared_ptr<RtlVariable> cond);

      inline bool has_jump() const { return has_jump(nullptr); }
      bool has_jump(std::shared_ptr<RtlVariable> cond) const;
    private:
      RtlFunction &m_function;
      RtlBlockName m_name;

      inputs_type m_inputs;
      std::list< std::unique_ptr<RtlOp> > m_ops;

      std::map< std::shared_ptr<RtlVariable>, RtlJump > m_jumps;

      friend class RtlFunction;
      friend class RtlJumpIterator;
    };

    class RtlBlockIterator {
    public:
      typedef std::pair< RtlBlockName, std::shared_ptr<RtlBasicBlock> > value_type;
      typedef RtlBlockIterator iterator_type;

      RtlBlockIterator(RtlFunction &fn,
                       int i);
      RtlBlockIterator(const RtlBlockIterator &o);

      bool operator==(const RtlBlockIterator &i) const;
      inline bool operator!=(RtlBlockIterator &i) const {
        return !(*this == i);
      }

      bool operator<(const RtlBlockIterator &i) const;
      bool operator>(const RtlBlockIterator &i) const;

      inline bool operator<=(const RtlBlockIterator &i) const {
        return (*this == i) || (*this < i);
      }

      inline bool operator>=(const RtlBlockIterator &i) const {
        return (*this == i) || (*this > i);
      }

      RtlBlockIterator operator+(int i) const;
      RtlBlockIterator &operator++();
      RtlBlockIterator operator++(int i);

      RtlBlockIterator begin() const;
      RtlBlockIterator end() const;

      value_type operator*() const;

    private:
      void swap(RtlBlockIterator &i);

      RtlFunction &m_function;
      int m_index;
    };

    class RtlFunction {
    public:
      RtlFunction(const std::string &name);
      ~RtlFunction();

      inline const std::string &functionName() const { return m_functionName; }

      std::shared_ptr<RtlBasicBlock> block();
      std::shared_ptr<RtlBasicBlock> block(const RtlBlockName &blockName) const;

      std::shared_ptr<RtlVariable> variable(const std::string &namePrefix,
                                            std::shared_ptr<RtlType> type);

      RtlTypeFactory &types();

      void dump(std::ostream &out);

      RtlBlockIterator blocks();

    private:
      std::string m_functionName;
      std::vector< std::shared_ptr<RtlBasicBlock> > m_blocks;

      std::unordered_map<std::string, std::shared_ptr<RtlVariable> > m_variables;

      std::unique_ptr<RtlTypeFactory> m_types;

      friend class RtlBlockIterator;
    };

    class OperandPrinter : public virtual RtlOperandVisitor {
    public:
      OperandPrinter(RtlFunction &fn, std::ostream &out);
      virtual ~OperandPrinter();

      virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input, bool output) override;
      virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) override;
      virtual void operand(const std::string &name, const RtlBlockName &dest) override;

    protected:
      void next();

      std::ostream &m_out;
      bool m_first;
    };

    void dumpOp(RtlOp &o, RtlFunction &f, std::ostream &out);
  }
}

#endif
