#include "compile/insnsched.hpp"
#include "compile/arbitrary.hpp"
#include "compile/dump.hpp"

#include <sstream>
#include <functional>

namespace nnc::compile {
  class UndemandVisitor : public RtlOperandVisitor {
  public:
    UndemandVisitor(RtlFunction &fn, std::map<RtlVariablePtr, int> &demanded)
      : RtlOperandVisitor(fn), m_demanded(demanded) {
    }
    virtual ~UndemandVisitor() {
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      if ( input ) {
        std::cerr << "Undemand " << var->name() << std::endl;
        auto it(m_demanded.find(var->repr()));
        if ( it != m_demanded.end() ) {
          it->second--;
          if ( it->second == 0) {
            std::cerr << "Variable " << var->name() << " no longer needed" << std::endl;
            m_demanded.erase(it);
          }
        }
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

  private:
    std::map<RtlVariablePtr, int> &m_demanded;
  };

  class OpRequiredCheck : public RtlOperandVisitor {
  public:
    OpRequiredCheck(const InsnScheduler::BlockScheduler &bs)
      : RtlOperandVisitor(bs.source().function()), m_sched(bs), m_required(false) {
    }
    virtual ~OpRequiredCheck() {
    }

    inline bool required() const { return m_required; }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      if ( m_required ) return; // Short-circuit
      if ( output ) {
        if ( m_sched.isNeeded(var) )
          m_required = true;
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

  private:
    const InsnScheduler::BlockScheduler &m_sched;
    bool m_required;
  };

  void htmlize(std::string &s) {
    std::stringstream outstr;

    char buf[2] = {0};
    std::transform(s.begin(), s.end(), std::ostream_iterator<const char *>(outstr),
                   [&buf](const char c) {
                     if ( c == '<' ) return "&lt;";
                     else if ( c == '>' ) return "&gt;";
                     else {
                       buf[0] = c;
                       buf[1] = 0;
                       return (const char *)buf;
                     }
                   });
    s = outstr.str();
  }

  class DotOutputProducer : public RtlOpSelector {
  public:
    DotOutputProducer(RtlFunction &fn, std::ostream &out)
      : m_function(fn), m_out(out), m_ix(0) {
    }
    virtual ~DotOutputProducer() {
    }

    virtual void alias(const RtlVariablePtr &left, const RtlVariablePtr &right) override {
      m_out << "alias " << left->name() << " to " << right->name() << "<br/>" << std::endl;
    }

    virtual void arbitrary(const RtlVariablePtr &p, const RegClass &rc) override {
      m_out << p->name() << " is an arbitrary in " << rc.name() << "<br/>" << std::endl;
    }

    virtual void clear() {
    }

    virtual void op(std::unique_ptr<RtlOp> &&op) {
      std::stringstream s;
      RtlBasicBlockDumper out(s);
      out.dumpOp(m_function, nullptr, *op, m_ix++);
      std::string html(s.str());
      htmlize(html);
      m_out << html << "<br/>" << std::endl;
    }

    virtual RtlFunction &function() const override { return m_function; }

  private:
    RtlFunction &m_function;
    std::ostream &m_out;
    int m_ix;
  };

  class DotInputProducer : public RtlOpMarker {
  public:
    DotInputProducer(RtlFunction &fn, std::ostream &out)
      : m_ix(0), m_function(fn), m_out(out) {
    }
    virtual ~DotInputProducer() {}

    virtual void matchOp(RtlOp *op, bool consume = true) override {
      std::stringstream outstr;
      RtlBasicBlockDumper out(outstr);
      out.dumpOp(m_function, nullptr, *op, m_ix++);

      std::string s(outstr.str());
      htmlize(s);

      m_out << s;
      m_out << "<br/>" << std::endl;
    }

  private:
    int m_ix;
    RtlFunction &m_function;
    std::ostream &m_out;
  };

  class RuleBuilder : public RtlOpSelector, public RtlOperandVisitor {
  public:
    RuleBuilder(InsnScheduler::BlockScheduler &s,
                std::deque<InsnScheduler::BlockScheduler::MatchPtr> & ready,
                std::map<RtlVariablePtr, RtlVariablePtr> &produced,
                std::multimap<RtlVariablePtr, InsnScheduler::BlockScheduler::MatchPtr> &suspended)
      : RtlOperandVisitor(s.source().function()),
        m_sched(s), m_ready(ready), m_produced(produced), m_suspended(suspended) {
    }

    virtual ~RuleBuilder() {
    }

    virtual RtlFunction &function() const {
      return RtlOperandVisitor::function();
    }

    virtual void alias(const RtlVariablePtr &left,
                       const RtlVariablePtr &right) override {
      // Alias
      auto newVar(m_sched.getAlias(left, right->type()));
      if ( newVar ) {
        produce(right)->alias(newVar);
      } else {
        auto newAlias(produce(right));
        m_sched.destination().emplace_op<RtlAliasOp>(map(left), newAlias);
        m_sched.alias(left, right->type(), newAlias);
      }
    }

    virtual void arbitrary(const RtlVariablePtr &p, const RegClass &rc) override {
      m_sched.destination().emplace_op<RtlArbitraryOp>(produce(p), rc);
    }

    virtual void clear() {
      for ( const auto &local: m_locals )
        m_produced.erase(m_produced.find(local));
      m_locals.clear();
    }

    virtual void op(std::unique_ptr<RtlOp> &&op) {
      m_rewrites.clear();

      op->operands(*this);

      // Rewrite
      for ( const auto &rewrite: m_rewrites )
        op->operand(rewrite.first, rewrite.second);

      m_sched.destination().op(std::move(op));
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      RtlVariablePtr newVar;
      if ( output )
        newVar = produce(var);
      else {
        newVar = map(var);
      }

      m_rewrites.emplace(name, newVar);
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

  private:
    RtlVariablePtr map(const RtlVariablePtr &p) {
      auto it(m_produced.find(p));
      if ( it == m_produced.end() ) {
        std::cerr << "Could not find var " << p->name() << std::endl;
        throw std::runtime_error("Variable not yet produced, but rule being built");
      }

      return it->second;
    }

    RtlVariablePtr produce(const RtlVariablePtr &p) {
      auto it(m_produced.find(p->repr()));
      // TODO copy 
      auto newVar(m_sched.destination().function().variable(p->namePrefix(), p->type()));

      if ( it == m_produced.end() ) {
        m_produced.emplace(p->repr(), newVar);
        m_locals.emplace(p->repr());
      } else {
        it->second = newVar;
      }

      std::cerr << "Producing " << p->name() << std::endl;

      auto [ begin, end ] = m_suspended.equal_range(p->repr());
      for (auto it(begin); it != end; it = m_suspended.erase(it) ) {
        it->second->resolveOneDep(p->repr());
        if ( it->second->ready() ) {
          m_ready.push_back(it->second);
        }
      };

      return newVar;
    }

    InsnScheduler::BlockScheduler &m_sched;
    std::deque<InsnScheduler::BlockScheduler::MatchPtr> &m_ready;
    std::map<RtlVariablePtr, RtlVariablePtr> &m_produced;
    std::multimap<RtlVariablePtr, InsnScheduler::BlockScheduler::MatchPtr> &m_suspended;

    std::set<RtlVariablePtr> m_locals;
    std::map<std::string, RtlVariablePtr> m_rewrites;
  };

  class SuspensionRemover : public RtlOpMarker, public RtlOperandVisitor {
  public:
    SuspensionRemover(InsnScheduler::BlockScheduler &s, InsnScheduler::BlockScheduler::MatchPtr rule)
      : RtlOperandVisitor(s.source().function()), m_sched(s), m_rule(rule) {
    }
    virtual ~SuspensionRemover() {
    }

    virtual void matchOp(RtlOp *op, bool consume = true) override {
      op->operands(*this);
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      if ( input && !m_sched.isProduced(var) ) {
        // This would cause a suspension
        auto [ begin, end ] = m_sched.m_suspended.equal_range(var->repr());
        auto it(std::find_if(begin, end, [this](const auto &c) { return c.second == m_rule; }));
        if ( it != end) {
          m_sched.m_suspended.erase(it);
        }
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

  private:
    InsnScheduler::BlockScheduler &m_sched;
    InsnScheduler::BlockScheduler::MatchPtr m_rule;
  };

  class CoverageCollector : public RtlOpMarker {
  public:
    CoverageCollector(std::set<RtlOp *> &coverage, InsnScheduler::BlockScheduler &s)
      : m_coverage(coverage), m_block(s) {
    }
    virtual ~CoverageCollector() {
    }

    virtual void matchOp(RtlOp *op, bool consume = true) override {
      if ( !m_block.isCovered(*op) )
        m_coverage.insert(op);
    }

  private:
    std::set<RtlOp *> &m_coverage;
    InsnScheduler::BlockScheduler &m_block;
  };

  class ProductionCollector : public RtlOpSelector, public RtlOperandVisitor {
  public:
    ProductionCollector(RtlFunction &fn, std::set<RtlVariablePtr> &productions)
      : RtlOperandVisitor(fn), m_productions(productions) {
    }
    virtual ~ProductionCollector() {
    }

    virtual void alias(const RtlVariablePtr &left, const RtlVariablePtr &right) {
      produces(left);
      produces(right);
    }

    virtual void arbitrary(const RtlVariablePtr &out, const RegClass &rc) override {
      produces(out);
    }

    virtual void clear() {
      m_productions.clear();
    }

    virtual void op(std::unique_ptr<RtlOp> &&op) {
      op->operands(*this);
    }

    virtual RtlFunction &function() const {
      return RtlOperandVisitor::function();
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      if ( output )
        produces(var);
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

  private:
    void produces(const RtlVariablePtr &p) {
      m_productions.insert(p->repr());
    }

    std::set<RtlVariablePtr> &m_productions;
  };

  class DemandCounter : public RtlOperandVisitor {
  public:
    DemandCounter(RtlFunction &f, std::map<RtlVariablePtr, int> &demanded)
      : RtlOperandVisitor(f),
        m_demanded(demanded) {
    }
    virtual ~DemandCounter() {
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      if ( input ) {
        auto it(m_demanded.find(var->repr()));
        if ( m_demanded.end() == it ) {
          m_demanded.emplace(var->repr(), 1);
        } else
          ++it->second;
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

  private:
    std::map<RtlVariablePtr, int> &m_demanded;
  };

  class RuleApplier : public InterestRecorder {
  public:
    RuleApplier(InsnSelRule &rule, InsnScheduler::BlockScheduler &sel)
      : m_rule(rule), m_selector(sel) {
    }

    virtual ~RuleApplier() { }

    virtual void allInstructions() override {
      for ( const auto &op: m_selector.source() )
        m_rule.apply(m_selector, *op);
    };

    virtual void instruction(const char *nm) override {
      for ( auto it(m_selector.m_ops_by_mnemonic.lower_bound(std::make_pair(nm, nullptr)));
            it != m_selector.m_ops_by_mnemonic.end() &&
              it->first == nm;
            ++it )
        m_rule.apply(m_selector, *it->second);
    }

  private:
    InsnSelRule &m_rule;
    InsnScheduler::BlockScheduler &m_selector;
  };

  class RuleReadinessCheck : public virtual RtlOpMarker, public virtual RtlOperandVisitor {
  public:
    RuleReadinessCheck(InsnScheduler::BlockScheduler &s)
      : RtlOperandVisitor(s.source().function()), m_block(s),
        m_ready(false) {
    }
    virtual ~RuleReadinessCheck() {
    }

    virtual void matchOp(RtlOp *op, bool consume = true) override {
      // Check that all inputs to this operation are ready
      op->operands(*this);
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlVariable> var, bool input = true, bool output = false) {
      if ( input &&
           m_local.find(var->repr()) == m_local.end() &&
           m_block.m_produced.find(var->repr()) == m_block.m_produced.end() ) {
        m_needed.insert(var->repr());
      } else if ( output ) {
        m_local.insert(var->repr());
      }
    }

    virtual void operand(const std::string &name, std::shared_ptr<RtlType> ty, const void *l, std::size_t lsz) {
    }

    virtual void operand(const std::string &name, const RtlBlockName &dest) {
    }

    inline const std::set<RtlVariablePtr> &deps() const { return m_needed; }
    inline bool ready() const { return m_needed.empty(); }

  private:
    InsnScheduler::BlockScheduler &m_block;
    bool m_ready;
    std::set<RtlVariablePtr> m_needed, m_local;
  };

  InsnScheduler::InsnScheduler(RtlFunction &src, RtlFunction &dest)
    : m_source(src), m_destination(dest) {
  }

  InsnScheduler::~InsnScheduler() {
  }

  InsnScheduler::BlockScheduler &InsnScheduler::block(const RtlBlockName &srcBlock) {
    auto it(m_blockMapping.find(srcBlock));
    if ( it != m_blockMapping.end() ) return it->second;
    else {
      return m_blockMapping.emplace(std::piecewise_construct,
                                     std::forward_as_tuple(srcBlock),
                                     std::forward_as_tuple(*this, m_source.block(srcBlock), m_destination.block())).first->second;
    }
  }

  InsnScheduler::BlockScheduler::BlockScheduler(InsnScheduler &s, RtlBasicBlock &src, RtlBasicBlock &dest)
    : m_function(s), m_src(src), m_dest(dest) {
    DemandCounter dc(m_src.function(), m_demanded);

    for ( auto &op: m_src) {
      m_ops_by_mnemonic.emplace(op->mnemonic(), op.get());
      op->operands(dc);
    }

    std::transform(m_src.inputs().begin(), m_src.inputs().end(),
                   std::inserter(m_produced, m_produced.begin()),
                   [this](const auto &oldInput) {
                     auto newInput(m_dest.addInput(oldInput->namePrefix(), oldInput->type()));
                     return std::make_pair(oldInput, newInput);
                   });
  }

  InsnScheduler::BlockScheduler::~BlockScheduler() {
  }

  void InsnScheduler::BlockScheduler::alias(const RtlVariablePtr &v, const std::shared_ptr<RtlType> &ty,
                                            const RtlVariablePtr &newp) {
    auto it(m_aliases.find(std::make_pair(v, ty)));
    if ( it == m_aliases.end() ) {
      m_aliases.emplace(std::make_pair(v, ty), newp);
    } else {
      it->second = newp;
    }
  }

  RtlVariablePtr InsnScheduler::BlockScheduler::getAlias(const RtlVariablePtr &v, const std::shared_ptr<RtlType> &ty) const {
    auto it(m_aliases.find(std::make_pair(v, ty)));
    if ( it == m_aliases.end() ) return nullptr;
    else return it->second;
  }

  void InsnScheduler::BlockScheduler::produceDot(std::ostream &out) {
    out << "graph {";
    for ( auto it(m_selected.begin()); it != m_selected.end(); ++it ) {
      SchedulableInsn &sel(*it);

      out << "node_" << (void*) &sel << "[" << std::endl;
      out << " label = <<TABLE><TR><TD>" << std::endl;
      DotInputProducer ip(source().function(), out);
      sel.consumedOperations(ip);
      // Now dump each op
      out << "</TD></TR><TR><TD>" << std::endl;
      out << "<FONT FACE=\"Courier New\">" << std::endl;
      DotOutputProducer op(source().function(), out);
      sel.build(op);
      out << "</FONT></TD></TR></TABLE>>" << std::endl;

      auto isReady(std::find(std::begin(m_ready), std::end(m_ready), it));
      if ( isReady != m_ready.end() ) {
        out << ", pencolor=green, penwidth=2";
      }

      out << "];" << std::endl;
    }

    for ( auto it(m_coverage.begin());
          it != m_coverage.end(); ) {
      auto begin(it);
      for ( ; it->first == begin->first && it != m_coverage.end(); ++it );

      for ( auto x(begin); x != it; ) {
        auto xx(x);
        for ( auto y(++x); y != it; ++y )
          out << "node_" << (void*) &(*xx->second) << " -- node_" << (void *) (&*y->second) << std::endl;
      }
    }

    out << "}" << std::endl;
  }

  std::vector<InsnScheduler::BlockScheduler::MatchPtr> InsnScheduler::BlockScheduler::interferes(const MatchPtr &p) {
    std::map<SchedulableInsn *, MatchPtr> r;
    for ( const auto &covered: p->coverage() ) {
      auto [begin, end] = m_coverage.equal_range(covered);
      for ( auto it(begin); it != end; ++it ) {
        r.emplace(&(*it->second), it->second);
      }
    }

    auto e(r.find(&(*p)));
    if ( e != r.end() )
      r.erase(e);

    std::vector<MatchPtr> ret;
    std::transform(r.begin(), r.end(), std::back_inserter(ret),
                   [](const auto &r) {
                     return r.second;
                   });

    return ret;
  }

  RtlFunction &InsnScheduler::BlockScheduler::function() const {
    return m_function.source();
  }

  void InsnScheduler::BlockScheduler::addRule(std::unique_ptr<InsnSelRule> &&rule) {
    RuleApplier apply(*rule, *this);
    rule->registerInterest(apply);
  }

  void InsnScheduler::BlockScheduler::markInsnSchedulable(std::unique_ptr<SelectedInsn> &&rule) {
    // This instruction can be scheduled
    SchedulableInsn &insn(m_selected.emplace_back(*this, std::move(rule)));

    auto it(m_selected.end());
    --it;

    // it points to the newly created rule

    for ( const auto &covered: insn.coverage() ) {
      m_coverage.emplace(covered, it);
    }

    // Now determine if this one is ready already. It's ready if all
    // inputs to all the ops being matched are ready
    RuleReadinessCheck check(*this);
    insn.rule().consumedOperations(check);
    if ( check.ready() ) {
      m_ready.push_back(it);
    } else {
      // Note that this insn is waiting for these dependencies
      insn.wait(check.deps());

      for ( const auto &dep: check.deps() )
        m_suspended.emplace(dep, it);
    }
  }

  bool InsnScheduler::BlockScheduler::isCovered(RtlOp &op) const {
    auto it(m_ops_by_mnemonic.find(std::make_pair(op.mnemonic(), &op)));
    return it == m_ops_by_mnemonic.end();
  }

  bool InsnScheduler::BlockScheduler::isProduced(const RtlVariablePtr &p) const {
    auto it(m_produced.find(p->repr()));
    return it != m_produced.end();
  }

  bool InsnScheduler::BlockScheduler::isNeeded(const RtlVariablePtr &p) const {
    if ( isProduced(p) ) return false;

    auto it(m_demanded.find(p->repr()));
    // TODO these may be needed by the jump arguments
    std::cerr << p->name() << " is needed because it is demanded by " <<
      (it != m_demanded.end() ? it->second : 0) << "  uses" << std::endl;
    return it != m_demanded.end() && it->second > 0;
  }

  bool InsnScheduler::BlockScheduler::isComplete() const {
    std::cerr << "These mnemonics remain: " << std::endl;
    for ( const auto &o: m_ops_by_mnemonic ) {
      RtlBasicBlockDumper dump(std::cerr);
      dump.dumpOp(source().function(), &source(), *o.second, 0);
      std::cerr << std::endl;
    }

    return m_ops_by_mnemonic.empty();
  }

  bool InsnScheduler::BlockScheduler::isOpNecessary(const RtlOp &op) const {
    OpRequiredCheck c(*this);
    op.operands(c);
    return c.required();
  }

  void InsnScheduler::BlockScheduler::finish() {
    // Copy over jumps
    for ( const auto &jump: source().jumps() ) {
      std::cerr << "Copy jump " << jump.first << std::endl;
      auto &newJump(destination().jump(mapVar(jump.first), m_function.block(jump.second.to()).destination().name()));

      for ( const auto &arg: jump.second.arguments() ) {
        if ( !mapVar(arg) )
          std::cerr << "Push argument " << arg->name() << std::endl;
        newJump.push_argument(mapVar(arg));
      }
    }
  }

  RtlVariablePtr InsnScheduler::BlockScheduler::mapVar(const RtlVariablePtr &v) const {
    if ( !v ) return nullptr;

    auto it (m_produced.find(v->repr()));
    if ( it == m_produced.end() ) return nullptr;
    else return it->second;
  }

  void InsnScheduler::BlockScheduler::ignore(MatchPtr i) {
    removeReady(i);

    // Eliminate from m_suspended queue
    removeSuspended(i);

    // Eliminate from m_coverage queue
    removeCoverage(i);

    // Remove from m_selected
    m_selected.erase(i);
  }

  void InsnScheduler::BlockScheduler::select(MatchPtr i) {
    // Now for all covered ops who may not still be needed, eliminate the op
    for ( const auto &covered: i->coverage() )
      removeOp(*covered);

    // Now build the rule, any variables that are produced by the
    // resulting operations are marked off.
    RuleBuilder builder(*this, m_ready, m_produced, m_suspended);
    i->build(builder);

    // However, some of the removed ops may still be needed, so add those back
    for ( const auto &covered: i->coverage() )
      if ( covered->isPure() ) {
        // Check that this operation's outputs are no longer needed
        if ( isOpNecessary(*covered) )
          addOp(*covered);
      }

    i->eliminateRedundant();

    // Remove all coverage
    removeCoverage(i);

    removeReady(i);
    m_selected.erase(i);
  }

  void InsnScheduler::BlockScheduler::removeReady(MatchPtr i) {
    // Eliminate from m_ready queue
    std::deque<MatchPtr> ready(std::move(m_ready));
    std::copy_if(ready.begin(), ready.end(),
                 std::back_inserter(m_ready),
                 [i](const auto &o) { return i != o; });
  }

  void InsnScheduler::BlockScheduler::removeCoverage(MatchPtr i) {
    for ( const auto &coverage: i->coverage() ) {
      auto [begin, end] = m_coverage.equal_range(coverage);
      auto it(std::find_if(begin, end, [i] (const auto &c) { return c.second == i; }));
      if ( it != end )
        m_coverage.erase(it);
    }
  }

  void InsnScheduler::BlockScheduler::removeSuspended(MatchPtr i) {
    SuspensionRemover s(*this, i);
    i->consumedOperations(s);
  }

  void InsnScheduler::BlockScheduler::addOp(RtlOp &op) {
    DemandCounter c(m_src.function(), m_demanded);
    op.operands(c);

    m_ops_by_mnemonic.emplace(op.mnemonic(), &op);
  }

  void InsnScheduler::BlockScheduler::removeOp(RtlOp &op) {
    // Remove the op from the demanded list
    UndemandVisitor undemand(m_src.function(), m_demanded);
    op.operands(undemand);

    auto it(m_ops_by_mnemonic.find(std::make_pair(op.mnemonic(), &op)));
    RtlBasicBlockDumper dump(std::cerr);
    if ( it != m_ops_by_mnemonic.end() ) {
      std::cerr << "Erasing operation ";
      dump.dumpOp(source().function(), &source(), op, 0);
      std::cerr << std::endl;
      m_ops_by_mnemonic.erase(it);
    } else {
      std::cerr << "Not erasing op ";
      dump.dumpOp(source().function(), &source(), op, 0);
      std::cerr << std::endl;
    }
  }

  InsnScheduler::SchedulableInsn::SchedulableInsn(BlockScheduler &s, std::unique_ptr<SelectedInsn> &&rule)
    : m_sched(s), m_rule(std::move(rule)) {
    ProductionCollector productions(s.source().function(), m_productions);
    m_rule->build(productions);

    CoverageCollector coverage(m_coverage, s);
    m_rule->consumedOperations(coverage);

    // erase any productions that already exist
    for ( const auto &mapping: s.varMappings() ) {
      auto it(m_productions.find(mapping.first));
      if ( it != m_productions.end() )
        m_productions.erase(it);
    }
  }

  void InsnScheduler::SchedulableInsn::resolveOneDep(const RtlVariablePtr &p) {
    auto it(m_unresolvedDeps.find(p->repr()));
    if ( it == m_unresolvedDeps.end() ) return;
    m_unresolvedDeps.erase(it);
    std::cerr << "Resolving dep " << p->name() << ": " << m_unresolvedDeps.size() << " remain" << std::endl;
  }

  void InsnScheduler::SchedulableInsn::eliminateRedundant() const {
    // This operation is going to be built, which means it will cover
    // some operations. That means it may make some other selections
    // redundant. A selection is redundant if (1) this op covers a
    // side-effectful op that another selection covers as well, or (2) if
    // a selection has no uncovered ops and produces no needed
    // variables
    //
    // We then eliminate any selections whose ops are all
    // covered. However, if this selection covers an op whose result
    // may still be needed by a future op, then this op is not
    // considered covered.
    std::deque<BlockScheduler::MatchPtr> redundant;
    for ( const auto &covered: m_coverage ) {
      bool necessary(m_sched.isOpNecessary(*covered));
      std::cerr << "Eliminating ";
      RtlBasicBlockDumper dump(std::cerr);
      dump.dumpOp(m_sched.source().function(), &m_sched.source(), *covered, 0);
      std::cerr << std::endl;

      // Okay, so now we look up each operation covered by us
      auto [begin, end] = m_sched.m_coverage.equal_range(covered);
      std::cerr << "Eliminating a redundant op... checking " << std::distance(begin, end) << " rule(s)" << std::endl;
      for ( auto it(begin); it != end; ++it ) {
        SchedulableInsn &other(*it->second);
        if ( &other == this ) continue;

        std::cerr << "Checking purity, then necessity" << std::endl;
        if ( !covered->isPure() ) {
          std::cerr << "Redundant because it is side effectful" << std::endl;
          // By case (1), this selection is redundant
          redundant.push_back(it->second);
        } else {
          // Only erase this operation in the coverage table if none
          // of its outputs are still being demanded.
          if ( !necessary ) {
            std::cerr << "Removing op from coverage" << std::endl;
            auto theirit(it->second->m_coverage.find(covered));
            if ( theirit != it->second->m_coverage.end() )
              it->second->m_coverage.erase(theirit);
          } else
            std::cerr << "This op is still necessary" << std::endl;
        }
      }
    }

    // Now check for case (2). We go over all selected insns, and
    // check if the coverage is empty. If so, we check if it produces
    // any unproduced variables.
    for ( auto it(m_sched.m_selected.begin()); it != m_sched.m_selected.end(); ++it ) {
      SchedulableInsn &other(*it);
      if ( &other == this ) continue;

      if ( it->redundant() )
        redundant.push_back(it);
    }

    std::cerr << "Selecting this op made " << redundant.size() << " rules redundant" << std::endl;
    // Now eliminate all redundant operations
    std::for_each(redundant.begin(), redundant.end(),
                  std::bind(&InsnScheduler::BlockScheduler::ignore, &m_sched, std::placeholders::_1));
  }

  bool InsnScheduler::SchedulableInsn::redundant() const {
    // If we cover uncovered ops, then we're not redundant
    if ( !m_coverage.empty() ) return false;

    // if we produce any variable that is not already produced we're not redundant
    if ( std::any_of(m_productions.begin(), m_productions.end(),
                     [this](const auto &p) {
                       auto needed(!m_sched.isProduced(p) && m_sched.isNeeded(p));
                       if ( needed ) std::cerr << "We are needed because " << p->name() << std::endl;
                       return needed;
                     }) )
      return false;

    return true;
  }
}
