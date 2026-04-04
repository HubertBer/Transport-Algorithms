#pragma once
#include "graph.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

inline Graph load_graph_from_csv(const std::string &dir) {
  const std::string nodes_csv = dir + "/nodes.csv";
  const std::string edges_csv = dir + "/edges.csv";

  int num_nodes = 0;
  {
    std::ifstream f(nodes_csv);
    if (!f)
      throw std::runtime_error("Cannot open nodes file: " + nodes_csv);

    std::string line;
    std::getline(f, line);
    while (std::getline(f, line))
      if (!line.empty())
        ++num_nodes;
  }

  Graph g(num_nodes);

  {
    std::ifstream f(edges_csv);
    if (!f)
      throw std::runtime_error("Cannot open edges file: " + edges_csv);

    std::string line;
    std::getline(f, line);

    while (std::getline(f, line)) {
      if (line.empty())
        continue;

      std::istringstream ss(line);
      std::string tok;

      int from, to;
      double dist;

      std::getline(ss, tok, ',');
      from = std::stoi(tok);
      std::getline(ss, tok, ',');
      to = std::stoi(tok);
      std::getline(ss, tok, ',');
      dist = std::stod(tok);

      g.add_edge(from, to, dist);
    }
  }

  return g;
}
