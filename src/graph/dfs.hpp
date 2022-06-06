#ifndef __nnc_graph_dfs_HPP__
#define __nnc_graph_dfs_HPP__

namespace nnc::graph {
  template<typename Node>
  class NoContextTracker {
  public:
    typedef Node value_type;
    typedef Node node_type;

    NoContextTracker(Node n) : m_value(n) {
    }
    NoContextTracker(const NoContextTracker &n) =default;

    Node &get() { return m_value; }
    const Node &get() const { return m_value; }

    node_type &operator*() { return m_value; }
    const node_type &operator*() const { return m_value; }

    NoContextTracker operator+(const Node &n) const {
      return n;
    }

  private:
    Node m_value;
  };

  template<typename Node>
  class ParentTracker {
  public:
    typedef ParentTracker<Node> value_type;
    typedef Node node_type;

    ParentTracker(Node n) : m_me(n) {
    }
    ParentTracker(Node n, Node p) : m_me(n), m_parent(p) {
    }

    ParentTracker<Node> &get() { return *this; }
    const ParentTracker<Node> &get() const { return *this; }

    bool has_parent() const { return m_parent.has_value(); }

    ParentTracker operator+(const Node &n) const {
      return ParentTracker(n, m_me);
    }

    node_type &operator*() { return m_me; }
    const node_type &operator*() const { return m_me; }

    node_type &child() { return m_me; }
    const node_type &child() const { return m_me; }

    node_type &parent() { return *m_parent; }
    const node_type &parent() const { return *m_parent; }

  private:
    Node m_me;
    std::optional<Node> m_parent;
  };

  template< typename Gr, typename Tracker = NoContextTracker<typename Gr::node_type> >
  class dfs {
  public:
    dfs(const Gr &gr)
      : m_graph(gr) {
    }

    void enqueue(const typename Tracker::node_type &v) {
      auto it(m_visited.find(m_graph.name(v)));
      if ( it == m_visited.end() ) {
        m_visited.emplace(m_graph.name(v));
        m_queue.emplace_back(v);
      }
    }

    void follow(const Tracker &t) {
      // Get all successors
      m_graph.successors(*t, [this, &t](const typename Gr::id_type &nm, const typename Gr::node_type &v) {
        auto it(m_visited.find(nm));
        if ( it != m_visited.end() ) return;

        m_visited.emplace(nm);
        m_queue.emplace_front(t + v);
      });
    }

    bool empty() const { return m_queue.empty(); }

    // Todo use SFINAE
    Tracker next() {
      Tracker t(std::move(peek()));
      drop();
      follow(t);
      return t;
    }

    const Tracker &peek() const {
      return m_queue.front();
    }
    Tracker &peek() {
      return m_queue.front();
    }

    void drop() { m_queue.pop_front(); }

  private:
    const Gr &m_graph;
    std::set<typename Gr::id_type> m_visited;
    std::deque<Tracker> m_queue;
  };
}

#endif
