#pragma once

#include <random>

#include "algorithm.hpp"
#include "dijkstra.hpp"
#include "visualizations.hpp"

class Alt : public Algorithm {
public:
  Alt(const Graph &g) : graph(g) {}

  void precompute() override {
    // TODO: better landmark selection
    landmarks.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, graph.num_nodes() - 1);
    for (int i = 0; i < 20; ++i) {
      landmarks.push_back(dis(gen));
    }

    auto graph_rev = graph.reversed();
    Dijkstra dijkstra(graph), dijkstra_rev(graph_rev);
    from_landmark_dist.resize(landmarks.size());
    to_landmark_dist.resize(landmarks.size());
    for (int l = 0; l < landmarks.size(); ++l) {
      from_landmark_dist[l] = dijkstra.queryAll(landmarks[l]);
      to_landmark_dist[l] = dijkstra_rev.queryAll(landmarks[l]);
    }
  }

  ShortestPathResult query(int source, int target) const override {
    auto heuristic = [&](const Graph &g, int target, int v) {
      double d = 0;
      for (int l = 0; l < landmarks.size(); ++l) {
        double d_lv = from_landmark_dist[l][v],
               d_lt = from_landmark_dist[l][target],
               d_vl = to_landmark_dist[l][v],
               d_tl = to_landmark_dist[l][target];
        if (d_lv < INF && d_lt < INF)
          d = std::max(d, d_lt - d_lv);
        if (d_vl < INF && d_tl < INF)
          d = std::max(d, d_vl - d_tl);
      }
      return d;
    };

    Dijkstra dijkstra(graph);
    return dijkstra.queryHeuristic(source, target, heuristic);
  }

  std::string name() const override { return "alt"; }

private:
  const Graph &graph;
  std::vector<int> landmarks;
  std::vector<std::vector<double>> from_landmark_dist, to_landmark_dist;
};
