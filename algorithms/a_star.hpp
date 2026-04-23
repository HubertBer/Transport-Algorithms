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
    Dijkstra dijkstra(graph);
    auto heuristic = [&](const Graph &g, int t, int v) {
      return GetDistanceInMeters(graph.coordinates[t], graph.coordinates[v]);
    };
    return dijkstra.queryHeuristic(source, target, heuristic);
  }

  std::string name() const override { return "A*"; }

private:
  const Graph &graph;
};
