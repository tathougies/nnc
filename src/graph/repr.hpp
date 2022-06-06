#ifndef __nnc_graph_repr_HPP__
#define __nnc_graph_repr_HPP__

namespace nnc::graph {
  template<typename Map, typename Set = std::set<typename Map::value_type::first_type> >
  class AdjList {
  public:
    typedef typename Map::value_type::first_type node_type;
    typedef node_type id_type;

    AdjList(Map &m) : m_data(m) {
      for ( const auto &p: m ) {
        m_nodes.emplace(p.first);
        m_nodes.emplace(p.second);
      }
    }
    AdjList(Map &m, const Set &s)
      : m_nodes(s), m_data(m) {
    }

    static inline id_type name(const node_type &t) { return t; }

    template<typename Fn>
    void nodes(Fn f) const {
      for( const auto &n: m_nodes )
        f(n, n);
    }

    template<typename Fn>
    void successors(const id_type &n, Fn f) const {
      auto it(m_data.begin()), next(it);
      if ( it != m_data.end() ) next++;

      while ( it != m_data.end() ){
        if ( it->first == n )
          f(it->second, it->second);

        it = next;
        if ( next != m_data.end() ) {
          next++;
        }
      }
    }

    template<typename Fn>
    void predecessors(const id_type &n, Fn f) const {
      auto it(m_data.begin()), next(it);
      if ( it != m_data.end() ) next++;

      while ( it != m_data.end() ){
        if ( it->second == n )
          f(it->first, it->first);

        it = next;
        if ( next != m_data.end() ) {
          next++;
        }
      }
    }

    void removeEdge(const id_type &n, const id_type &p) {
      auto it(m_data.find(std::make_pair(n, p)));
      if ( it != m_data.end() )
        m_data.erase(it);
    }

  private:
    Set m_nodes;
    Map &m_data;
  };

  template<typename Map>
  AdjList<Map> adjList(Map &m) { return AdjList(m); }
}

#endif
