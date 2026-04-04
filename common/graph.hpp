#pragma once

#include <vector>

struct Edge {
  int to;
  double distance;
};

struct Graph {
  std::vector<std::vector<Edge>> adj;

  explicit Graph(int n = 0) : adj(n) {}
  Graph(int n, std::vector<std::vector<Edge>> adj) : adj(std::move(adj)) {}

  int num_nodes() const { return static_cast<int>(adj.size()); }

  int num_edges() const {
    int total = 0;
    for (const auto &neighbours : adj)
      total += static_cast<int>(neighbours.size());
    return total;
  }

  void add_edge(int from, int to, double distance) {
    adj[from].push_back({to, distance});
  }
};
