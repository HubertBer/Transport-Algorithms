#include "raylib.h"
#include "visualizations.hpp"
#include "algorithm.hpp"
#include "graph_io.hpp"
#include "raymath.h"
#include "dijkstra.hpp"
#include "graph.hpp"

#include <iostream>
#include <vector>
using std::vector;

vector<Vector2> compute_positions(const vector<Vector2> coordinates) {
    Vector2 min_coords = coordinates[0];
    Vector2 max_coords = coordinates[0];
    for (auto [x, y]: coordinates) {
        min_coords.x = std::min(min_coords.x, x);
        min_coords.y = std::min(min_coords.y, y);
        max_coords.x = std::max(max_coords.x, x);
        max_coords.y = std::max(max_coords.y, y);
    }
    Vector2 coord_diff = max_coords - min_coords;
    // Let's translate and rescale the coordinates so that they are in [0, 1] interval.
    float scaling_factor = 1.0 / std::max(coord_diff.x, coord_diff.y);

    vector<Vector2> positions(coordinates);
    for (int i = 0; i < positions.size(); ++i) {
        positions[i] -= min_coords;
        positions[i] *= scaling_factor;
    }
    return positions;
}

void raylib_visualization(vector<VisualizationEvent> events, const Graph& graph) {
    vector<Vector2> positions = compute_positions(graph.coordinates);
    InitWindow(1920, 1200, "transport algorithm visualization");
    
    Vector2 origin{50, 50};
    float scale_y = 1000;
    float scale_x = 1800;
    Vector2 scale = {scale_x, scale_y};


    const Color unvisited_color = GRAY;
    const Color being_visited_color = GREEN;
    const Color visited_color = BLACK;

    vector<Color> node_color(graph.num_nodes());
    vector<float> node_radius(graph.num_nodes());
    vector<Color> edge_color(graph.num_edges());
    vector<float> edge_width(graph.num_edges());
    std::cout<< "BRUH" << '\n';
    for (int i = 0; i < graph.num_nodes(); ++i) {
        node_color[i] = unvisited_color;
        node_radius[i] = 8;
    }
    for (int i = 0; i < graph.num_edges(); ++i) {
        edge_color[i] = unvisited_color;
        edge_width[i] = 4;
    }

    int event_now = 0;
    const float time_between_events = 0.5;
    float next_event_timer = 0;
    std::cout<< "BRUH" << '\n';

    // We may not need to do it incrementally don't now yet though.
    auto process_events_tick = [&]() {
        std::cout<< "PROCESSING_TICK" << '\n';
        std::cout<< "PROCESSING_TICK " << events.size() << '\n';
        while(event_now < events.size()) {
            auto event = events[event_now];
            event_now += 1;
            
            std::cout<< "ANOTHER_EVENT "<< event.id << '\n';
            switch(event.type) {
                case VisualizationEventType::ADD_START_VERTEX: break;
                case VisualizationEventType::ADD_END_VERTEX: break;
                case VisualizationEventType::START_VISITING_EDGE:
                    edge_color[event.id] = being_visited_color;
                    edge_width[event.id] = 7;
                    break;
                case VisualizationEventType::END_VISITING_EDGE:
                    edge_color[event.id] = visited_color;
                    edge_width[event.id] = 4;
                    break;
                case VisualizationEventType::START_VISITING_VERTEX:
                    node_color[event.id] = being_visited_color;
                    node_radius[event.id] = 12;
                    break;
                case VisualizationEventType::END_VISITING_VERTEX:
                    node_color[event.id] = visited_color;
                    node_radius[event.id] = 8;
                    // The end of the tick
                    return;
            }
        }
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        next_event_timer -= GetFrameTime();
        if (next_event_timer < 0) {
            next_event_timer += time_between_events;
            process_events_tick();
        }

        for (int i = 0; i < graph.num_nodes(); ++i) {
            for (int j = 0; j < graph.adj[i].size(); ++j) {
                auto edge = graph.adj[i][j];
                auto pos0 = positions[i] * scale + origin;
                auto pos1 = positions[edge.to] * scale + origin;
                DrawLineEx(pos0, pos1, edge_width[edge.id], edge_color[edge.id]);
            }
        }

        for (int i = 0; i < graph.num_nodes(); ++i) {
            auto pos = positions[i];
            pos *= scale;
            pos += origin;
            DrawCircleV(pos, node_radius[i], node_color[i]);
        }

        EndDrawing();
    }
}

std::unique_ptr<Algorithm> make_algorithm(const std::string &name) {
    if (name == "dijkstra")
        return std::make_unique<Dijkstra>();
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

    auto algorithm = make_algorithm(algo);

    std::cout << "Algorithm : " << algorithm->name() << '\n'
                << "Nodes     : " << g.num_nodes() << '\n'
                << "Edges     : " << g.num_edges() << '\n'
                << "Source    : " << source << '\n'
                << "Target    : " << target << '\n';

    ShortestPathResult result = algorithm->compute(g, source, target);

    raylib_visualization(result.visualization_events, g);
    return 0;
}