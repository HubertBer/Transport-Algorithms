#pragma once

#include <random>

#include "algorithm.hpp"
#include "dijkstra.hpp"
#include "visualizations.hpp"

class Alt : public Algorithm {
  void selectRandomLandmarks(int landmark_count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, graph.num_nodes() - 1);

    auto graph_rev = graph.reversed();
    Dijkstra dijkstra(graph), dijkstra_rev(graph_rev);

    for (int i = 0; i < landmark_count; ++i) {
      int l = dis(gen);
      landmarks.push_back(l);
      from_landmark_dist.push_back(dijkstra.queryAll(l));
      to_landmark_dist.push_back(dijkstra_rev.queryAll(l));
    }
  }

  void selectFarthestLandmarks(int landmark_count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, graph.num_nodes() - 1);

    auto graph_rev = graph.reversed();
    Dijkstra dijkstra(graph), dijkstra_rev(graph_rev);

    for (int i = 0; i < landmark_count; ++i) {
      int nl;
      if (i == 0) {
        nl = dis(gen);
      } else {
        double max_d = 0;
        for (int v = 0; v < graph.num_nodes(); ++v) {
          double min_dv = INF;
          for (int l = 0; l < landmarks.size(); ++l) {
            if (min_dv > from_landmark_dist[l][v] ||
                min_dv > to_landmark_dist[l][v]) {
              min_dv =
                  std::min(from_landmark_dist[l][v], to_landmark_dist[l][v]);
            }
          }
          if (min_dv > max_d) {
            max_d = min_dv;
            nl = v;
          }
        }
      }
      landmarks.push_back(nl);
      from_landmark_dist.push_back(dijkstra.queryAll(nl));
      to_landmark_dist.push_back(dijkstra_rev.queryAll(nl));
    }
  }

public:
  Alt(const Graph &g) : graph(g) {}

  void precompute() override {
    landmarks.clear();
    from_landmark_dist.clear();
    to_landmark_dist.clear();

    selectFarthestLandmarks(20);
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
    auto result = dijkstra.queryHeuristic(source, target, heuristic);

    std::vector<VisualizationEvent> events(landmarks.size() +
                                           result.visualization_events.size());
    for (int i = 0; i < landmarks.size(); ++i) {
      events[i] = {VisualizationEventType::LANDMARK, landmarks[i]};
    }
    for (int i = 0; i < result.visualization_events.size(); ++i) {
      events[landmarks.size() + i] = result.visualization_events[i];
    }
    result.visualization_events = events;

    return result;
  }

  std::string name() const override { return "alt"; }

private:
  const Graph &graph;
  std::vector<int> landmarks;
  std::vector<std::vector<double>> from_landmark_dist, to_landmark_dist;
};
