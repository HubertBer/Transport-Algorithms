#include "raylib.h"
#include "visualizations.hpp"
#include "algorithm.hpp"
#include "graph_io.hpp"
#include "raymath.h"
#include "dijkstra.hpp"
#include "a_star.hpp"
#include "double_dijkstra.hpp"
#include "graph.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <iostream>
#include <memory>
#include <vector>

using std::vector;
struct Button {
    Rectangle bounds;
    std::string label;
    int fontSize;
    Color bgColor;
    Color textColor;
    int font_size;
    Color text_color;
};

struct Slider {
    Rectangle bounds;
    std::string middle_format;
    float* value;
    float min_value;
    float max_value;
    int font_size;
    Color text_color;
};

inline void DrawTextStretched(const char* text, int posX, int posY, int fontSize, Color color) {
    float fx = posX;
    float fy = posY;
    float size = fontSize;
    Font f = GetFontDefault();
    float spacing = 4.0f;
    Vector2 ts = MeasureTextEx(f, text, size, spacing);
    DrawTextEx(f, text, { fx - ts.x * 0.5f, fy - size * 0.5f }, size, spacing, color);
}

inline Slider make_slider(float x, float y, float width, float height, const std::string& middle_format, float *value, float min_value, float max_value, int font_size = 20, Color text_color = BLACK) {
    return Slider{
        Rectangle{x, y, width, height},
        middle_format,
        value,
        min_value,
        max_value,
        font_size,
        text_color
    };
}

inline Button createButton(float x, float y, float width, float height, const std::string& label, int fontSize = 30, Color bg = BLACK, Color fg = WHITE) {
    return Button{{x, y, width, height}, label, fontSize, bg, fg};
}

inline void draw_slider(Slider slider) {
    GuiSliderBar(slider.bounds, "", "", slider.value, slider.min_value, slider.max_value);
    DrawTextStretched(
        TextFormat(slider.middle_format.c_str(), *slider.value),
        slider.bounds.x + slider.bounds.width / 2,
        slider.bounds.y + slider.bounds.height / 2,
        slider.font_size,
        slider.text_color
    );
}

inline void drawButton(const Button& button) {
    Rectangle scaledBounds = button.bounds;
    DrawRectangleRec(scaledBounds, button.bgColor);

    DrawTextStretched(
        button.label.c_str(),
        button.bounds.x + button.bounds.width / 2,
        button.bounds.y + button.bounds.height / 2,
        button.fontSize,
        button.textColor
    );
}

inline bool isButtonClicked(const Button& button) {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, button.bounds) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

std::unique_ptr<Algorithm> make_algorithm(std::string &name) {
    if (name == "dijkstra")
        return std::make_unique<Dijkstra>();
    if (name == "A*")
        return std::make_unique<AStar>();
    if (name == "double_dijkstra")
        return std::make_unique<DoubleDijkstra>();
    throw std::invalid_argument("Unknown algorithm: " + name);
}

vector<Vector2> compute_positions(vector<Vector2> coordinates) {
    for (int i = 0; i < coordinates.size(); ++i) {
        // Flip y coordinates as the screen positions have them flipped
        coordinates[i].y *= -1;
    }

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

struct AlgoSimulationState {
    Vector2 origin;
    Vector2 scale;

    Color unvisited_color;
    Color being_visited_color;
    Color visited_color;

    vector<bool> vertex_visited;
    vector<bool> is_path_edge;
    vector<Color> node_color;
    vector<float> node_radius;
    vector<Color> edge_color;
    vector<float> edge_width;
    
    std::string algo_name;
    ShortestPathResult result;

    int event_now;
    float next_event_timer;

    Button dijkstra_button;
    Button a_star_button;
    Button double_dijkstra_button;
    
};

AlgoSimulationState make_algo_simulation_state(int n, int m, std::string algo, ShortestPathResult result, Vector2 origin, Vector2 scale) {
    return {
        origin,
        scale,

        GRAY,
        GREEN,
        BLACK,
    
        vector<bool>(n, false),
        vector<bool>(m, false),
        vector<Color>(n),
        vector<float>(n),
        vector<Color>(m),
        vector<float>(m),
        algo,
        result,

        0,
        0,

        createButton(0, 0.465, 0.21, 0.03, "dijkstra", 15),
        createButton(0, 0.50, 0.21, 0.03, "A*", 15),
        createButton(0, 0.535, 0.21, 0.03, "double_dijkstra", 15)
    };
} 

void raylib_visualization(ShortestPathResult result, const Graph& graph, std::string algo) {
    vector<Vector2> positions = compute_positions(graph.coordinates);
    InitWindow(1920, 1200, "transport algorithm visualization");

    static constexpr int max_simulations = 4;
    vector<vector<Vector2>> origins{
        {},
        {{50, 50}},
        {{50, 50}, {750, 50}},
        {{50, 50}, {650, 50}, {50, 500}},
        {{50, 50}, {650, 50}, {50, 500}, {650, 500}},
    };
    vector<Vector2> scales{
        {},
        {1400, 1400},
        {700, 700},
        {600, 600},
        {600, 600},
    };

    vector<AlgoSimulationState> sim_states{
        make_algo_simulation_state(graph.num_nodes(), graph.num_edges(), algo, result, origins[2][0], scales[2]),
        make_algo_simulation_state(graph.num_nodes(), graph.num_edges(), algo, result, origins[2][1], scales[2]),    
    };

    uint64_t start_node = 0;
    uint64_t end_node = 0;
    float frequency = 7;
    float path_width = 10;
    int64_t source_picked = -1;
    int64_t target_picked = -1;

    for (auto& sim_state: sim_states) {
        for (int i = 0; i < graph.num_nodes(); i += 1) {
            sim_state.node_color[i] = sim_state.unvisited_color;
            sim_state.node_radius[i] = 1;
        }
        for (int i = 0; i < graph.num_edges(); i += 1) {
            sim_state.edge_color[i] = sim_state.unvisited_color;
            sim_state.edge_width[i] = 0.6;
        }
        for (auto [v, eid]: sim_state.result.path){
            sim_state.is_path_edge[eid] = true;
        }
    }

    auto reset_visualization = [&](uint64_t source, uint64_t target){
        int idx = 0;
        for (auto& sim_state: sim_states) {
            auto algorithm = make_algorithm(sim_state.algo_name);
            auto result = algorithm->compute(graph, source, target);
            sim_state.result = result;
    
            for (int i = 0; i < graph.num_nodes(); ++i) {
                sim_state.node_color[i] = sim_state.unvisited_color;
                sim_state.node_radius[i] = 1;
                sim_state.vertex_visited[i] = false;
            }
            for (int i = 0; i < graph.num_edges(); ++i) {
                sim_state.edge_color[i] = sim_state.unvisited_color;
                sim_state.edge_width[i]   = 0.6;
                sim_state.is_path_edge[i] = false;
            }
            for (auto [v, eid]: sim_state.result.path){
                sim_state.is_path_edge[eid] = true;
            }
            sim_state.event_now = 0;
            sim_state.next_event_timer = 0;
            sim_state.origin = origins[sim_states.size()][idx];
            sim_state.scale = scales[sim_states.size()];
            idx += 1;
        }

        start_node = source;
        end_node = target;
        source_picked = -1;
        target_picked = -1;
    };

    // We may not need to do it incrementally don't now yet though.
    auto process_events_tick = [&](AlgoSimulationState& sim_state) {
        while(sim_state.event_now < sim_state.result.visualization_events.size()) {
            auto event = sim_state.result.visualization_events[sim_state.event_now];
            sim_state.event_now += 1;

            switch(event.type) {
                case VisualizationEventType::ADD_START_VERTEX:
                    start_node = event.id;
                    break;
                case VisualizationEventType::ADD_END_VERTEX:
                    end_node = event.id;
                    break;
                case VisualizationEventType::START_VISITING_EDGE:    
                    sim_state.edge_color[event.id] = sim_state.being_visited_color;
                    sim_state.edge_width[event.id] = 2;
                    break;
                case VisualizationEventType::END_VISITING_EDGE:
                    sim_state.edge_color[event.id] = sim_state.visited_color;
                    sim_state.edge_width[event.id] = 1.5;
                    break;
                case VisualizationEventType::START_VISITING_VERTEX:
                    sim_state.vertex_visited[event.id] = true;
                    sim_state.node_color[event.id] = sim_state.being_visited_color;
                    sim_state.node_radius[event.id] = 2;
                    break;
                case VisualizationEventType::END_VISITING_VERTEX:
                    sim_state.node_color[event.id] = sim_state.visited_color;
                    sim_state.node_radius[event.id] = 1;
                    // The end of the tick
                    return;
            }
        }
    
    };

    auto closest_point_idx = [&](Vector2 screen_pos) {
        float min_dist = Vector2DistanceSqr(screen_pos, sim_states[0].origin + positions[0] * sim_states[0].scale);
        uint64_t min_idx = 0;
        for (auto& sim_state: sim_states) {
            for (int i = 1; i < positions.size(); ++i) {
                auto pos = sim_state.origin + positions[i] * sim_state.scale;
                auto dist = Vector2DistanceSqr(screen_pos, pos);
                if (dist < min_dist) {
                    min_dist = dist;
                    min_idx = i;
                }
            }
        }
        return min_idx;
    };

    Slider frequency_slider         = make_slider(1200, 650, 250, 40, "SIM SPEED: %.1f", &frequency, 0, 16);
    Slider path_width_slider        = make_slider(1200, 700, 250, 40, "PATH WIDTH: %.1f", &path_width, 0, 30);
    Button reset_button             = createButton(1200, 750, 250, 40, "RESET");
    Button add_vis                  = createButton(1200, 800, 80, 40, "NEW");
    Button rm_vis                   = createButton(1300, 800, 80, 40, "RM");

    auto screen_space_button = [](const AlgoSimulationState& sim_state, Button button) {
        auto new_button = button;
        new_button.bounds.x = new_button.bounds.x * sim_state.scale.x + sim_state.origin.x;        
        new_button.bounds.y = new_button.bounds.y * sim_state.scale.y + sim_state.origin.y;        
        new_button.bounds.width = new_button.bounds.width * sim_state.scale.x;        
        new_button.bounds.height = new_button.bounds.height * sim_state.scale.y;        
        return new_button;
    };

    auto draw_ui = [&]() {
        // GLOBAL UI
        draw_slider(frequency_slider);
        draw_slider(path_width_slider);
        drawButton(reset_button);
        drawButton(add_vis);
        drawButton(rm_vis);
        DrawFPS(5, 5);

        // PER SIM UI
        for (auto& sim_state: sim_states) {
            drawButton(screen_space_button(sim_state, sim_state.dijkstra_button));
            drawButton(screen_space_button(sim_state, sim_state.a_star_button));
            drawButton(screen_space_button(sim_state, sim_state.double_dijkstra_button));
            auto [x, y] = Vector2{0.07, 0.45} * sim_state.scale + sim_state.origin;
            DrawTextStretched(sim_state.algo_name.c_str(), x, y, 10, BLACK);
        }
    };

    auto handle_ui_logic = [&]() {
        for (auto& sim_state: sim_states) {
            if (isButtonClicked(screen_space_button(sim_state, sim_state.dijkstra_button))) {
                sim_state.algo_name = "dijkstra";
            }
            if (isButtonClicked(screen_space_button(sim_state, sim_state.a_star_button))) {
                sim_state.algo_name = "A*";
            }
            if (isButtonClicked(screen_space_button(sim_state, sim_state.double_dijkstra_button))) {
                sim_state.algo_name = "double_dijkstra";
            }
        }
        if (isButtonClicked(reset_button)) {
            reset_visualization(start_node, end_node);
        }
        if (isButtonClicked(add_vis)) {
            if (sim_states.size() < max_simulations) {
                sim_states.push_back(make_algo_simulation_state(graph.num_nodes(), graph.num_edges(), algo, result, {}, {}));
            } 
            reset_visualization(start_node, end_node);
        }
        if (isButtonClicked(rm_vis)) {
            if (sim_states.size() > 0) {
                sim_states.pop_back();
            } 
            reset_visualization(start_node, end_node);
        }
    };

    while (!WindowShouldClose()) {
        handle_ui_logic();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = GetMousePosition();
            source_picked = closest_point_idx(mouse_pos);
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = GetMousePosition();
            target_picked = closest_point_idx(mouse_pos);
        }
        if (source_picked >= 0 && target_picked >= 0) {
            reset_visualization(source_picked, target_picked);
        }

        BeginDrawing();
        ClearBackground(WHITE);

        float dt = GetFrameTime();
        for (auto& sim_state: sim_states) {
            sim_state.next_event_timer -= dt;
            while (sim_state.next_event_timer < 0) {
                sim_state.next_event_timer += powf(2, -frequency);
                process_events_tick(sim_state);
            }
        }

        // Draw edges
        for (auto& sim_state: sim_states) {
            for (int i = 0; i < graph.num_nodes(); ++i) {
                for (int j = 0; j < graph.adj[i].size(); ++j) {
                    auto edge = graph.adj[i][j];
                    auto pos0 = positions[i] * sim_state.scale + sim_state.origin;
                    auto pos1 = positions[edge.to] * sim_state.scale + sim_state.origin;
                    if (sim_state.is_path_edge[edge.id] && (sim_state.vertex_visited[i] || sim_state.vertex_visited[edge.to])) {
                        DrawLineEx(pos0, pos1, path_width, MAROON);
                        DrawCircleV(pos0, path_width / 2, MAROON);
                    } else {
                        DrawLineEx(pos0, pos1, sim_state.edge_width[edge.id], sim_state.edge_color[edge.id]);
                    }
                }
            }

            // Draw nodes
            for (int i = 0; i < graph.num_nodes(); ++i) {
                auto pos = positions[i] * sim_state.scale + sim_state.origin;
                DrawCircleV(pos, sim_state.node_radius[i], sim_state.node_color[i]);
            }
            
            // Draw start and end node
            DrawCircleV(positions[start_node] * sim_state.scale + sim_state.origin, 4, BLUE);
            DrawCircleV(positions[end_node] * sim_state.scale + sim_state.origin, 4, RED);
            // Draw source and target nodes
            if (source_picked >= 0) { DrawCircleV(positions[source_picked] * sim_state.scale + sim_state.origin, 8, DARKBLUE); }
        if (target_picked >= 0) { DrawCircleV(positions[target_picked] * sim_state.scale + sim_state.origin, 8, MAROON); }
        }

        draw_ui();
        EndDrawing();
    }
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

    raylib_visualization(result, g, algo);
    return 0;
}
