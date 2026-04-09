#pragma once

#include "raylib.h"
#include "raymath.h"

#include <cstdint>
#include <vector>

struct Edge {
  int to;
  double distance;
  uint64_t id;
};

inline float GetDistanceInMeters(Vector2 coord1, Vector2 coord2) {
    const float metersPerDegree = 111132.0f;
    Vector2 v1 = { coord1.x * metersPerDegree, coord1.y * metersPerDegree * 2 };
    Vector2 v2 = { coord2.x * metersPerDegree, coord2.y * metersPerDegree * 2 };
    return Vector2Distance(v1, v2);
}

struct Graph {
  std::vector<std::vector<Edge>> adj;
  std::vector<Vector2> coordinates;
  uint64_t next_edge_id = 0;

  explicit Graph(int n = 0) : adj(n) {}
  Graph(int n, std::vector<std::vector<Edge>> adj, std::vector<Vector2> coordinates) : adj(std::move(adj)), coordinates(std::move(coordinates)) {}
  Graph(const Graph& other) {
    adj.resize(other.adj.size());
    for (int i = 0; i < adj.size(); i += 1) {
      adj[i] = other.adj[i];
    }
    coordinates = other.coordinates;
    next_edge_id = other.next_edge_id;
  }

  int num_nodes() const { return static_cast<int>(adj.size()); }

  int num_edges() const {
    return next_edge_id;
  }

  void add_edge(int from, int to, double distance) {
    adj[from].push_back({to, distance, next_edge_id});
    next_edge_id += 1;
  }
};
