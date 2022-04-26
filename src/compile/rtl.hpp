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

    class RtlJump {
    public:
      typedef std::vector< std::shared_ptr<RtlVariable> >  args_type;

      RtlJump(const RtlBlockName &to);

      void push_argument(std::shared_ptr<RtlVariable> arg);

      inline std::size_t arity() const { return m_args.size(); }
      std::shared_ptr<RtlVariable> operator[] (int i) const;

      inline const RtlBlockName &to() const { return m_to; }

      inline const args_type &arguments() const { return m_args; }

    private:
      RtlBlockName m_to;
      args_type m_args;
    };

    class RtlBasicBlock;

    class RtlJumpIterator {
    public:
      typedef std::map< std::shared_ptr<RtlVariable>, RtlJump >::iterator iterator;

      iterator begin();
      iterator end();

    private:
      RtlJumpIterator(RtlBasicBlock &blk);

      RtlBasicBlock &m_block;
      friend class RtlBasicBlock;
    };

    class RtlBasicBlock {
    public:
      RtlBasicBlock(RtlFunction &fn, const RtlBlockName &nm);
      ~RtlBasicBlock();

      inline RtlBlockName name() const { return m_name; }
      inline RtlFunction &function() const { return m_function; }

      // Add input. Marks the given variable as having
      std::shared_ptr<RtlVariable> addInput(const std::string &namePrefix,
                                            std::shared_ptr<RtlType> type);

      template<typename It>
      void replaceOps(const It &begin, const It &end) {
        m_ops.clear();
        addOps(begin, end);
      }

      template<typename It>
      void addOps(const It &begin, const It &end) {
        for ( It i(begin); i != end; ++i ) {
          op(std::move(*i));
        }
      }

      void op(std::unique_ptr<RtlOp> &&op);

      // Add tail call

      template<typename T, typename... Args>
      T &emplace_op(Args &&... args) {
        auto newOp(std::make_unique<T>(std::forward<Args>(args)...));
        T &ret(*newOp);
        op(std::move(newOp));
        return ret;
      }

      void cast(std::shared_ptr<RtlVariable> from, std::shared_ptr<RtlVariable> to);

      typedef std::list< std::unique_ptr<RtlOp> >::iterator iterator;
      iterator begin();
      iterator end();

      RtlJumpIterator jumps();

      typedef std::vector< std::shared_ptr<RtlVariable> > inputs_type;
      inline const inputs_type &inputs() const { return m_inputs; }

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
  }
}

#endif
