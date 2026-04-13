#pragma once

#include "algorithm.hpp"
#include "visualizations.hpp"

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

constexpr double INF = std::numeric_limits<double>::infinity();

class Dijkstra : public Algorithm {
public:
    ShortestPathResult compute(const Graph &g, int source,
                             int target) const override {
    VisualisationQueue visualisation_queue;

    std::vector<double> dist(g.num_nodes(), INF);
    std::vector<std::pair<uint32_t, uint32_t>> prev(g.num_nodes(), {-1, -1});

    using P = std::tuple<double, int, uint64_t>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    visualisation_queue.add_start_vertex(source);
    visualisation_queue.add_end_vertex(target);

    dist[source] = 0.0;
    pq.push({0.0, source, g.num_edges()});

    while (!pq.empty()) {
      auto [d, u, edge_id] = pq.top();
      pq.pop();

      if (edge_id < g.num_edges()) {
        visualisation_queue.end_visiting_edge(edge_id);
      }

      if (u == target) {
        break;
      }
      if (d > dist[u]) {
        continue;
      }

      visualisation_queue.start_visiting_vertex(u);

      for (auto &e : g.adj[u]) {
        double nd = dist[u] + e.distance;
        if (nd < dist[e.to]) {
          dist[e.to] = nd;
          prev[e.to] = {u, e.id};
          pq.push({nd, e.to, e.id});
          visualisation_queue.start_visiting_edge(e.id);
        }
      }

      visualisation_queue.end_visiting_vertex(u);
    }

    std::vector<std::pair<uint32_t, uint32_t>> path;
    if (dist[target] < INF) {
      for (auto v = target; prev[v].first != -1; v = prev[v].first) {
        path.emplace_back(v, prev[v].second);
      }
      std::reverse(path.begin(), path.end());
    }

    int visited = std::count_if(dist.begin(), dist.end(),
                                [](double d) { return d < INF; });
    return {path, dist[target], visited, visualisation_queue.events};
  }

  std::string name() const override { return "dijkstra"; }
};
