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

inline double to_radians(double degree) { return degree * (PI / 180.0); }

inline double GetDistanceInMeters(Vector2 coord1, Vector2 coord2) {
  // https://en.wikipedia.org/wiki/Haversine_formula#Formulation
  const double R = 6371.0 * 1000.0;

  double p1 = to_radians(coord1.y), p2 = to_radians(coord2.y);
  double dp = to_radians(coord2.y - coord1.y),
         dl = to_radians(coord2.x - coord1.x);

  double h = std::pow(std::sin(dp / 2.0), 2) +
             std::cos(p1) * std::cos(p2) * std::pow(std::sin(dl / 2.0), 2);

  double theta = 2.0 * std::atan2(std::sqrt(h), std::sqrt(1.0 - h));

  return R * theta;
}

struct Graph {
  std::vector<std::vector<Edge>> adj;
  std::vector<Vector2> coordinates;
  uint64_t next_edge_id = 0;

  explicit Graph(int n = 0) : adj(n) {}
  Graph(int n, std::vector<std::vector<Edge>> adj,
        std::vector<Vector2> coordinates)
      : adj(std::move(adj)), coordinates(std::move(coordinates)) {}
  Graph(const Graph &other) {
    adj.resize(other.adj.size());
    for (int i = 0; i < adj.size(); i += 1) {
      adj[i] = other.adj[i];
    }
    coordinates = other.coordinates;
    next_edge_id = other.next_edge_id;
  }

  int num_nodes() const { return static_cast<int>(adj.size()); }

  int num_edges() const { return next_edge_id; }

  void add_edge(int from, int to, double distance) {
    adj[from].push_back({to, distance, next_edge_id});
    next_edge_id += 1;
  }

  Graph reversed() const {
    int n = num_nodes();
    Graph rev(n);
    rev.adj.assign(n, std::vector<Edge>());
    rev.coordinates = coordinates;
    rev.next_edge_id = next_edge_id;
    for (int v = 0; v < n; ++v) {
      for (const Edge e : adj[v]) {
        rev.adj[v].push_back({e.to, e.distance, e.id});
      }
    }
    return rev;
  }
};
