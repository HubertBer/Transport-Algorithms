#pragma once

#include "graph.hpp"
#include "visualizations.hpp"

#include <string>
#include <vector>

struct ShortestPathResult {
  std::vector<std::pair<uint32_t, uint32_t>> path;
  double distance;
  int visited;
  std::vector<VisualizationEvent> visualization_events;
};

class Algorithm {
public:
  virtual ~Algorithm() = default;
  virtual ShortestPathResult compute(const Graph &g, int source,
                                     int target) const = 0;
  virtual std::string name() const = 0;
};
