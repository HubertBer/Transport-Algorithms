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
  Dijkstra(const Graph &g) : graph(g) {}

  void precompute() const override {}

  ShortestPathResult query(int source, int target) const override {
    VisualisationQueue visualisation_queue;

    std::vector<double> dist(graph.num_nodes(), INF);
    std::vector<int> prev(graph.num_nodes(), -1);

    using P = std::tuple<double, int, uint64_t>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    visualisation_queue.add_start_vertex(source);
    visualisation_queue.add_end_vertex(target);

    dist[source] = 0.0;
    pq.push({0.0, source, graph.num_edges()});

    while (!pq.empty()) {
      auto [d, u, edge_id] = pq.top();
      pq.pop();

      if (edge_id < graph.num_edges()) {
        visualisation_queue.end_visiting_edge(edge_id);
      }

      if (u == target) {
        break;
      }
      if (d > dist[u]) {
        continue;
      }

      visualisation_queue.start_visiting_vertex(u);

      for (auto &e : graph.adj[u]) {
        double nd = dist[u] + e.distance;
        if (nd < dist[e.to]) {
          dist[e.to] = nd;
          prev[e.to] = u;
          pq.push({nd, e.to, e.id});
          visualisation_queue.start_visiting_edge(e.id);
        }
      }

      visualisation_queue.end_visiting_vertex(u);
    }

    std::vector<int> path;
    if (dist[target] < INF) {
      for (int v = target; v != -1; v = prev[v])
        path.push_back(v);
      std::reverse(path.begin(), path.end());
    }

    int visited = std::count_if(dist.begin(), dist.end(),
                                [](double d) { return d < INF; });
    return {path, dist[target], visited, visualisation_queue.events};
  }

  std::vector<double> query(int source) const {
    // TODO: don't duplicate code

    std::vector<double> dist(graph.num_nodes(), INF);
    std::vector<int> prev(graph.num_nodes(), -1);

    using P = std::tuple<double, int, uint64_t>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    dist[source] = 0.0;
    pq.push({0.0, source, graph.num_edges()});

    while (!pq.empty()) {
      auto [d, u, edge_id] = pq.top();
      pq.pop();

      if (d > dist[u]) {
        continue;
      }

      for (auto &e : graph.adj[u]) {
        double nd = dist[u] + e.distance;
        if (nd < dist[e.to]) {
          dist[e.to] = nd;
          prev[e.to] = u;
          pq.push({nd, e.to, e.id});
        }
      }
    }

    return dist;
  }

  std::string name() const override { return "dijkstra"; }

private:
  const Graph &graph;
};
