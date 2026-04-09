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
      from_landmark_dist[l] = dijkstra.query(landmarks[l]);
      to_landmark_dist[l] = dijkstra_rev.query(landmarks[l]);
    }
  }

  ShortestPathResult query(int source, int target) const override {
    std::vector<double> potentials(graph.num_nodes(), 0);
    for (int l = 0; l < landmarks.size(); ++l) {
      for (int v = 0; v < graph.num_nodes(); ++v) {
        double d_lv = from_landmark_dist[l][v],
               d_lt = from_landmark_dist[l][target],
               d_vl = to_landmark_dist[l][v],
               d_tl = to_landmark_dist[l][target];
        if (d_lv < INF && d_lt < INF)
          potentials[v] = std::max(potentials[v], d_lt - d_lv);
        if (d_vl < INF && d_tl < INF)
          potentials[v] = std::max(potentials[v], d_vl - d_tl);
      }
    }

    Graph g(graph);
    for (int v = 0; v < g.adj.size(); ++v) {
      for (auto &e : g.adj[v]) {
        e.distance =
            std::max(0.0, e.distance + potentials[e.to] - potentials[v]);
      }
    }

    Dijkstra dijkstra(g);
    return dijkstra.query(source, target);
  }

  std::string name() const override { return "alt"; }

private:
  const Graph &graph;
  std::vector<int> landmarks;
  std::vector<std::vector<double>> from_landmark_dist, to_landmark_dist;
};
