#ifndef __nnc_graph_toposort_HPP__
#define __nnc_graph_toposort_HPP__

#include <iostream>
#include <set>

#include "graph/dfs.hpp"

namespace nnc::graph {
  template<typename Out, typename Gr>
  Out toposort(Gr &graph) {
    Out ret;
    std::set<typename Gr::id_type> parented_nodes, roots;

    graph.nodes([&parented_nodes, &graph](const typename Gr::id_type &i, const typename Gr::node_type &n) {
      graph.successors(i, [&parented_nodes](const typename Gr::id_type &j, const typename Gr::node_type &n) {
        parented_nodes.insert(j);
      });
    });
    graph.nodes([&parented_nodes, &roots](const typename Gr::id_type &i, const typename Gr::node_type &n) {
      if ( parented_nodes.find(i) == parented_nodes.end() )
        roots.insert(i);
    });

    while ( !roots.empty() ) {
      auto next(*roots.begin());
      roots.erase(roots.begin());

      ret.emplace_back(next);

      graph.successors(next, [&next, &graph, &roots](const typename Gr::id_type &child, const typename Gr::node_type &n) {
        graph.removeEdge(next, child);

        bool hasIncoming(false);
        // Check if child has any incoming edges
        graph.predecessors(child, [&hasIncoming](const typename Gr::id_type &n, const typename Gr::node_type &d) {
          hasIncoming = true;
        });

        if ( !hasIncoming ) {
          roots.insert(child);
        }
      });
    }

    return ret;
  }
}

#endif
