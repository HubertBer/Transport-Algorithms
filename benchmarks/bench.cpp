#include "algorithm.hpp"
#include "graph.hpp"
#include "graph_io.hpp"

#include "dijkstra.hpp"
#include "double_dijkstra.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

std::unique_ptr<Algorithm> make_algorithm(const std::string &name, const Graph &g) {
  if (name == "dijkstra")
    return std::make_unique<Dijkstra>(g);
  if (name == "double_dijkstra")
    return std::make_unique<DoubleDijkstra>(g);
  throw std::invalid_argument("Unknown algorithm: " + name);
}

int main(int argc, char *argv[]) {
  std::string data, algo;
  int source, target;

  if (argc == 5) {
    data = argv[1];
    algo = argv[2];
    source = std::stoi(argv[3]);
    target = std::stoi(argv[4]);
  } else {
    std::cerr << "Usage: " << argv[0] << "\n"
              << "       " << argv[0] << " data algo source target\n";
    return 1;
  }

  Graph g = load_graph_from_csv(data);

  auto algorithm = make_algorithm(algo, g);

  std::cout << "Algorithm : " << algorithm->name() << '\n'
            << "Nodes     : " << g.num_nodes() << '\n'
            << "Edges     : " << g.num_edges() << '\n'
            << "Source    : " << source << '\n'
            << "Target    : " << target << '\n';

  auto t0 = std::chrono::steady_clock::now();
  algorithm->precompute();
  ShortestPathResult result = algorithm->query(source, target);
  auto t1 = std::chrono::steady_clock::now();

  double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

  if (result.path.empty()) {
    std::cout << "No path found.\n";
  } else {
    std::cout << "Distance  : " << result.distance / 1000.0 << " km\n"
              << "Hops      : " << result.path.size() - 1 << '\n'
              << "Visited   : " << result.visited << '\n'
              << "Time      : " << ms << " ms\n";
  }

  return 0;
}
