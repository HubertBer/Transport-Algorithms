#pragma once

#include <cmath>
#include <map>
#include <unordered_set>

#include "algorithm.hpp"
#include "dijkstra.hpp"
#include "visualizations.hpp"

class ArcFlags : public Algorithm {
  void selectGridRegions(int reg_count_x, int reg_count_y) {
    float min_x = std::numeric_limits<float>::max(),
          max_x = std::numeric_limits<float>::min();
    float min_y = std::numeric_limits<float>::max(),
          max_y = std::numeric_limits<float>::min();
    for (auto &p : graph.coordinates) {
      min_x = std::min(min_x, p.x);
      max_x = std::max(max_x, p.x);
      min_y = std::min(min_y, p.y);
      max_y = std::max(max_y, p.y);
    }

    float region_size_x = (max_x - min_x) / reg_count_x;
    float region_size_y = (max_y - min_y) / reg_count_y;

    regions.resize(graph.num_nodes());
    for (int v = 0; v < graph.num_nodes(); ++v) {
      int rx = std::floor((graph.coordinates[v].x - min_x) / region_size_x);
      int ry = std::floor((graph.coordinates[v].y - min_y) / region_size_y);
      regions[v] = ry * reg_count_x + rx;
    }
  }

public:
  ArcFlags(const Graph &g) : graph(g) {}

  void precompute() override {
    const int region_count = 16;

    selectGridRegions(sqrt(region_count), sqrt(region_count));

    auto graph_rev = graph.reversed();
    Dijkstra dijkstra_rev(graph_rev);

    std::unordered_set<int> boundary;
    for (int u = 0; u < graph.num_nodes(); ++u) {
      for (const auto &e : graph.adj[u]) {
        int v = e.to;
        flags[{u, v}].assign(region_count, false);
        flags[{u, v}][regions[v]] = true;
        if (regions[u] != regions[v])
          boundary.insert(v);
      }
    }

    for (auto &b : boundary) {
      auto dist = dijkstra_rev.queryAll(b);
      for (int u = 0; u < graph.num_nodes(); ++u) {
        if (dist[u] == std::numeric_limits<double>::max())
          continue;
        for (const auto &e : graph.adj[u]) {
          int v = e.to;
          if (dist[v] == std::numeric_limits<double>::max())
            continue;
          if (std::abs(dist[u] - dist[v] - e.distance) < 1e-9)
            flags[{u, v}][regions[b]] = true;
        }
      }
    }
  }

  ShortestPathResult query(int source, int target) const override {
    auto edge_predicate = [&](int u, int v) {
      if (regions[u] == regions[target]) {
        return true;
      }
      return flags.at({u, v}).at(regions[target]);
    };

    Dijkstra dijkstra(graph);
    dijkstra.addEdgePredicate(edge_predicate);

    auto result = dijkstra.query(source, target);
    return result;
  }

  std::string name() const override { return "arc-flags"; }

private:
  const Graph &graph;
  std::vector<int> regions;
  std::map<std::pair<int, int>, std::vector<bool>> flags;
};
