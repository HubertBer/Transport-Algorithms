#pragma once

#include "algorithm.hpp"

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

    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    dist[source] = 0.0;
    pq.push({0.0, source});

    while (!pq.empty()) {
      auto [d, u] = pq.top();
      pq.pop();

      if (u == target)
        break;
      if (d > dist[u])
        continue;

      for (auto &e : g.adj[u]) {
        double nd = dist[u] + e.distance;
        if (nd < dist[e.to]) {
          dist[e.to] = nd;
          prev[e.to] = u;
          pq.push({nd, e.to});
        }
      }
    }

    std::vector<int> path;
    if (dist[target] < INF) {
      for (int v = target; v != -1; v = prev[v])
        path.push_back(v);
      std::reverse(path.begin(), path.end());
    }

    int visited = std::count_if(dist.begin(), dist.end(),
                                [](double d) { return d < INF; });
    return {path, dist[target], visited};
  }

  std::string name() const override { return "dijkstra"; }
};
