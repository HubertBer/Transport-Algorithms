#pragma once

#include "raylib.h"

#include <cstdint>
#include <vector>

struct Edge {
  int to;
  double distance;
  uint64_t id;
};

struct Graph {
  std::vector<std::vector<Edge>> adj;
  std::vector<Vector2> coordinates;
  uint64_t next_edge_id = 0;

  explicit Graph(int n = 0) : adj(n) {}
  Graph(int n, std::vector<std::vector<Edge>> adj, std::vector<Vector2> coordinates) : adj(std::move(adj)), coordinates(std::move(coordinates)) {}

  int num_nodes() const { return static_cast<int>(adj.size()); }

  int num_edges() const {
    int total = 0;
    for (const auto &neighbours : adj)
      total += static_cast<int>(neighbours.size());
    return total;
  }

  void add_edge(int from, int to, double distance) {
    adj[from].push_back({to, distance, next_edge_id});
    next_edge_id += 1;
  }
};
