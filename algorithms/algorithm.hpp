#pragma once

#include "graph.hpp"

#include <string>
#include <vector>

struct ShortestPathResult {
  std::vector<int> path;
  double distance;
  int visited;
};

class Algorithm {
public:
  virtual ~Algorithm() = default;
  virtual ShortestPathResult compute(const Graph &g, int source,
                                     int target) const = 0;
  virtual std::string name() const = 0;
};
