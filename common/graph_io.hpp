#pragma once
#include "graph.hpp"
#include "raylib.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

inline Graph load_graph_from_csv(const std::string &dir) {
  const std::string nodes_csv = dir + "/nodes.csv";
  const std::string edges_csv = dir + "/edges.csv";

  int num_nodes = 0;
  std::vector<Vector2> coords;

  {
    std::ifstream f(nodes_csv);
    if (!f)
      throw std::runtime_error("Cannot open nodes file: " + nodes_csv);

    std::string line;
    std::getline(f, line);

    while (std::getline(f, line)) {
      if (line.empty())
        continue;

      std::istringstream ss(line);
      std::string tok;

      int id;
      float lat, lon;

      std::getline(ss, tok, ',');
      id = std::stoi(tok);
      std::getline(ss, tok, ',');
      std::getline(ss, tok, ',');
      lat = std::stof(tok);
      std::getline(ss, tok, ',');
      lon = std::stof(tok);

      coords.emplace_back(lon, lat);
      ++num_nodes;
    }
  }

  Graph g(num_nodes);
  g.coordinates = coords;

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
      // TODO remove later. For now make sure that graph is not actually directed
      g.add_edge(to, from, dist);
    }
  }

  return g;
}
