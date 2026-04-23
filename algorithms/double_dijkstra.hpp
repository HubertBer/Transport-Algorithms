#pragma once

#include "algorithm.hpp"
#include "visualizations.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <queue>
#include <vector>

class DoubleDijkstra : public Algorithm {
  constexpr static double INF = std::numeric_limits<double>::infinity();

public:
  DoubleDijkstra(const Graph &g) : graph(g) {}

  void precompute() override {}

  ShortestPathResult query(int source, int target) const override {
    VisualisationQueue visualisation_queue;

    std::array<std::vector<double>, 2> dist{
        {std::vector<double>(graph.num_nodes(), INF),
         std::vector<double>(graph.num_nodes(), INF)}};
    std::array<std::vector<std::pair<uint32_t, uint32_t>>, 2> prev{
        std::vector<std::pair<uint32_t, uint32_t>>(graph.num_nodes(), {-1, -1}),
        std::vector<std::pair<uint32_t, uint32_t>>(graph.num_nodes(),
                                                   {-1, -1})};
    using T = std::tuple<double, int, uint64_t>;
    std::priority_queue<T, std::vector<T>, std::greater<T>> pq[2];

    dist[0][source] = dist[1][target] = 0.0;
    pq[0].push({0.0, source, graph.num_edges()});
    pq[1].push({0.0, target, graph.num_edges()});
    visualisation_queue.add_start_vertex(source);
    visualisation_queue.add_end_vertex(target);

    double mu = INF;
    int meetup_node = UINT32_MAX;
    while (!pq[0].empty() && !pq[1].empty()) {
      double df = pq[0].empty() ? INF : std::get<0>(pq[0].top());
      double db = pq[1].empty() ? INF : std::get<0>(pq[1].top());
      if (df + db >= mu) {
        break;
      }

      int dir = df < db ? 0 : 1;

      auto [d, u, edge_id] = pq[dir].top();
      pq[dir].pop();
      if (edge_id < graph.num_edges()) {
        visualisation_queue.end_visiting_edge(edge_id);
      }
      visualisation_queue.start_visiting_vertex(u);

      for (auto &e : graph.adj[u]) {
        double nd = dist[dir][u] + e.distance;
        if (nd < dist[dir][e.to]) {
          dist[dir][e.to] = nd;
          prev[dir][e.to] = {u, e.id};
          pq[dir].push({nd, e.to, e.id});
          visualisation_queue.start_visiting_edge(e.id);
        }
        if (nd + dist[1 - dir][e.to] < mu) {
          mu = nd + dist[1 - dir][e.to];
          meetup_node = e.to;
        }
      }
      visualisation_queue.end_visiting_vertex(u);
    }

    std::vector<std::pair<uint32_t, uint32_t>> path;
    if (mu < INF) {
      for (uint32_t v = meetup_node; prev[0][v].first != UINT32_MAX;
           v = prev[0][v].first) {
        path.emplace_back(v, prev[0][v].second);
      }
      std::reverse(path.begin(), path.end());
      for (uint32_t v = meetup_node; prev[1][v].first != UINT32_MAX;
           v = prev[1][v].first) {
        path.emplace_back(prev[1][v].first, prev[1][v].second);
      }
      std::reverse(path.begin(), path.end());
    }

    int visited = 0;
    for (int i = 0; i < graph.num_nodes(); ++i) {
      if (dist[0][i] < INF || dist[1][i] < INF) {
        ++visited;
      }
    }
    return {path, mu, visited, visualisation_queue.events};
  }

  std::string name() const override { return "double_dijkstra"; }

private:
  const Graph &graph;
};
