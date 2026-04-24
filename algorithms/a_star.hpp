#pragma once

#include "algorithm.hpp"
#include "dijkstra.hpp"
#include "raymath.h"
#include "visualizations.hpp"

class AStar : public Algorithm {
public:
  AStar(const Graph &g) : graph(g) {}

  void precompute() override {}

  ShortestPathResult query(int source, int target) const override {
    auto heuristic = [&](const Graph &g, int t, int v) {
      return GetDistanceInMeters(graph.coordinates[t], graph.coordinates[v]);
    };

    Dijkstra dijkstra(graph);
    dijkstra.addHeuristic(heuristic);

    auto result = dijkstra.query(source, target);
    return result;
  }

  std::string name() const override { return "A*"; }

private:
  const Graph &graph;
};
