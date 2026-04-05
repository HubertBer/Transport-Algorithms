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
    std::vector<double> dist(g.num_nodes(), INF);
    std::vector<int> prev(g.num_nodes(), -1);

    using P = std::tuple<double, int, uint64_t>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    std::vector<VisualizationEvent> visualization_events;
    visualization_events.emplace_back(VisualizationEventType::ADD_START_VERTEX, source);
    visualization_events.emplace_back(VisualizationEventType::ADD_END_VERTEX, target);

    dist[source] = 0.0;
    pq.push({0.0, source, g.num_edges()});

    while (!pq.empty()) {
      auto [d, u, edge_id] = pq.top();
      pq.pop();

      if (edge_id < g.num_edges()) {
        visualization_events.emplace_back(VisualizationEventType::END_VISITING_EDGE, edge_id);
      }

      if (u == target) { break; }
      if (d > dist[u]) { continue; }

      visualization_events.emplace_back(VisualizationEventType::START_VISITING_VERTEX, u);

      for (auto &e : g.adj[u]) {
        double nd = dist[u] + e.distance;
        if (nd < dist[e.to]) {
          dist[e.to] = nd;
          prev[e.to] = u;
          pq.push({nd, e.to, e.id});
          visualization_events.emplace_back(VisualizationEventType::START_VISITING_EDGE, e.id);
        }
      }

      visualization_events.emplace_back(VisualizationEventType::END_VISITING_VERTEX, u);
    }

    std::vector<int> path;
    if (dist[target] < INF) {
      for (int v = target; v != -1; v = prev[v])
        path.push_back(v);
      std::reverse(path.begin(), path.end());
    }

    int visited = std::count_if(dist.begin(), dist.end(),
                                [](double d) { return d < INF; });
    return {path, dist[target], visited, visualization_events};
  }

  std::string name() const override { return "dijkstra"; }
};
