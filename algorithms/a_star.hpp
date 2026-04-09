#pragma once

#include "algorithm.hpp"
#include "visualizations.hpp"
#include "dijkstra.hpp"
#include "raymath.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>
#include <iostream>

class AStar : public Algorithm {
public:
  ShortestPathResult compute(const Graph &og_g, int source,
                             int target) const override {
    Dijkstra dijkstra;
    Graph g(og_g);
    const auto& coord = g.coordinates;

    std ::cerr << "BRUH" << '\n';

    for (int i = 0; i < g.adj.size(); i += 1) {
        for (int j = 0; j < g.adj[i].size(); j += 1) {
            auto edge = g.adj[i][j];
            auto p0 = GetDistanceInMeters(coord[i], coord[target]);
            auto p1 = GetDistanceInMeters(coord[edge.to], coord[target]);
            g.adj[i][j].distance += p1 - p0;
        }
    }
    
    return dijkstra.compute(g, source, target);
  }

  std::string name() const override { return "A*"; }
};
