#pragma once

#include "algorithm.hpp"
#include "dijkstra.hpp"
#include "raymath.h"
#include "visualizations.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

class AStar : public Algorithm {
public:
  AStar(const Graph &g) : graph(g) {}

  void precompute() override {}

  ShortestPathResult query(int source, int target) const override {
    Graph g(graph);
    const auto &coord = g.coordinates;

    for (int i = 0; i < g.adj.size(); i += 1) {
      for (int j = 0; j < g.adj[i].size(); j += 1) {
        auto edge = g.adj[i][j];
        auto p0 = GetDistanceInMeters(coord[i], coord[target]);
        auto p1 = GetDistanceInMeters(coord[edge.to], coord[target]);
        g.adj[i][j].distance += p1 - p0;
      }
    }

    Dijkstra dijkstra(g);
    return dijkstra.query(source, target);
  }

  std::string name() const override { return "A*"; }

private:
  const Graph &graph;
};
