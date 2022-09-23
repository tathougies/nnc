#ifndef __nnc_compile_rtl_HPP__
#define __nnc_compile_rtl_HPP__

#include <memory>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <iostream>

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

    template<typename Jumps, typename Underlying>
    class RtlJumpIterator {
    private:
      RtlJumpIterator(Jumps &jumps)
        : m_jumps(jumps) {
      }

      Jumps &m_jumps;

      friend class RtlBasicBlock;

    public:
      Underlying begin() { return m_jumps.rbegin(); }
      Underlying end() { return m_jumps.rend(); }
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
      bool isEntryPoint() const;

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

      bool has_jump_to(const RtlBlockName &to) const;
      RtlJump &get_jump_to(const RtlBlockName &to);
      const RtlJump &get_jump_to(const RtlBlockName &to) const;

    private:
      RtlFunction &m_function;
      RtlBlockName m_name;

      inputs_type m_inputs;
      std::list< std::unique_ptr<RtlOp> > m_ops;

      std::map< std::shared_ptr<RtlVariable>, RtlJump > m_jumps;

      friend class RtlFunction;

    public:
      typedef RtlJumpIterator<decltype(m_jumps), decltype(m_jumps)::reverse_iterator> jumps_iterator;
      typedef RtlJumpIterator<const decltype(m_jumps), decltype(m_jumps)::const_reverse_iterator> const_jumps_iterator;
      jumps_iterator jumps();
      const_jumps_iterator jumps() const;
    };

    class RtlFunction;

    template<typename Q>
    class Top {
    public:
      static typename Q::value_type &get(Q &q) {
        return q.top();
      }
    };

    template<typename T>
    class Top<std::queue<T>> {
    public:
      static T &get(std::queue<T> &q) {
        return q.front();
      }
    };

    template<typename Q>
    class RtlBasicBlockTraversal {
    public:
      typedef RtlBasicBlock value_type;

      RtlBasicBlockTraversal<Q> &begin() {
        return *this;
      }

      RtlBasicBlockTraversal<Q> end() {
        return RtlBasicBlockTraversal<Q>(m_function);
      }

      RtlBasicBlockTraversal<Q> &operator++() {
        next();
        return *this;
      }

      bool operator==(const RtlBasicBlockTraversal &o) const {
        return &o.m_function == &m_function && o.current == current;
      }

      bool operator!=(const RtlBasicBlockTraversal &o) const {
        return !(*this == o);
      }

      value_type &operator*() {
        return *current;
      }

    private:
      friend class RtlFunction;

      RtlBasicBlockTraversal(RtlFunction &func, typename Q::value_type &entry)
        : m_function(func), current(&*entry) {
      }

      RtlBasicBlockTraversal(RtlFunction &func)
        : m_function(func), current(nullptr) { }

      void add(const typename Q::value_type &o) {
        RtlBasicBlock *blk(&(*o));
        if ( visited.find(blk) == visited.end() ) {
          upcoming.push(o);
          visited.insert(blk);
        }
      }

      void next();

      RtlFunction &m_function;
      RtlBasicBlock *current;
      Q upcoming;
      std::unordered_set<RtlBasicBlock *> visited;
    };

    class RtlFunction {
    public:
      RtlFunction(const std::string &name);
      ~RtlFunction();

      inline const std::string &functionName() const { return m_functionName; }

      RtlBasicBlock &block(bool entry = false);
      RtlBasicBlock &block(const RtlBlockName &blockName);
      const RtlBasicBlock &block(const RtlBlockName &blockName) const;
      inline RtlBasicBlock &entry() { return *m_entry; }
      inline const RtlBasicBlock &entry() const { return *m_entry; }

      std::shared_ptr<RtlVariable> variable(const std::string &namePrefix,
                                            std::shared_ptr<RtlType> type);

      RtlTypeFactory &types();

      void dump(std::ostream &out);

      inline const std::list<RtlBasicBlock> &blocks() const { return m_blocks; };
      inline std::list<RtlBasicBlock> &blocks() { return m_blocks; }

    private:
      std::string m_functionName;
      std::list< RtlBasicBlock > m_blocks;
      decltype(m_blocks)::iterator m_entry;

      std::unordered_map<std::string, std::shared_ptr<RtlVariable> > m_variables;

      std::unique_ptr<RtlTypeFactory> m_types;

      friend class RtlBlockIterator;

    public:
      inline RtlBasicBlockTraversal<std::queue<decltype(m_blocks)::iterator>> bfs() {
        return RtlBasicBlockTraversal<std::queue<decltype(m_blocks)::iterator>>(*this, m_entry);
      }

      inline RtlBasicBlockTraversal<std::stack<decltype(m_blocks)::iterator>> dfs() {
        return RtlBasicBlockTraversal<std::stack<decltype(m_blocks)::iterator>>(*this, m_entry);
      }

      decltype(m_blocks)::iterator findBlock(const RtlBlockName &nm);
    };

    template<typename Q>
    void RtlBasicBlockTraversal<Q>::next() {
      value_type &block(**this);
      for ( const auto &jump: block.jumps() ) {
        auto it(m_function.findBlock(jump.second.to()));
        add(it);
      }

      if ( !upcoming.empty() ) {
        current = &(*Top<Q>::get(upcoming));
        upcoming.pop();
      } else
        current = nullptr;
    }
  }
}

#endif
