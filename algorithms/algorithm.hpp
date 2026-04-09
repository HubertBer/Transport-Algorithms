#pragma once

#include "graph.hpp"
#include "visualizations.hpp"

#include <string>
#include <vector>

struct ShortestPathResult {
  std::vector<int> path;
  double distance;
  int visited;
  std::vector<VisualizationEvent> visualization_events;
};

struct AllShortestPathsResult {
  std::vector<double> distance;
};

class Algorithm {
public:
  virtual ~Algorithm() = default;
  virtual void precompute() = 0;
  virtual ShortestPathResult query(int source, int target) const = 0;
  virtual std::string name() const = 0;
};
