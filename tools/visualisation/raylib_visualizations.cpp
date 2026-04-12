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

void raylib_visualization(vector<VisualizationEvent> events, const Graph& graph, std::string algo) {
    vector<Vector2> positions = compute_positions(graph.coordinates);
    InitWindow(1920, 1200, "transport algorithm visualization");

    Vector2 origin{50, 50};
    Vector2 scale = {1400, 1400};

    const Color unvisited_color = GRAY;
    const Color being_visited_color = GREEN;
    const Color visited_color = BLACK;

    vector<Color> node_color(graph.num_nodes());
    vector<float> node_radius(graph.num_nodes());
    vector<Color> edge_color(graph.num_edges());
    vector<float> edge_width(graph.num_edges());

    for (int i = 0; i < graph.num_nodes(); ++i) {
        node_color[i] = unvisited_color;
        node_radius[i] = 1;
    }
    for (int i = 0; i < graph.num_edges(); ++i) {
        edge_color[i] = unvisited_color;
        edge_width[i] = 0.6;
    }

    int event_now = 0;
    float frequency = 7;
    float next_event_timer = 0;
    uint64_t start_node = 0;
    uint64_t end_node = 0;
    int64_t source_picked = -1;
    int64_t target_picked = -1;
    
    auto reset_visualization = [&](uint64_t source, uint64_t target){
        auto algorithm = make_algorithm(algo);
        auto result = algorithm->compute(graph, source, target);
        events = result.visualization_events;

        for (int i = 0; i < graph.num_nodes(); ++i) {
            node_color[i] = unvisited_color;
            node_radius[i] = 1;
        }
        for (int i = 0; i < graph.num_edges(); ++i) {
            edge_color[i] = unvisited_color;
            edge_width[i] = 0.6;
        }

        event_now = 0;
        float next_event_timer = 0;
        start_node = source;
        end_node = target;
        source_picked = -1;
        target_picked = -1;
    };

    // We may not need to do it incrementally don't now yet though.
    auto process_events_tick = [&]() {
        std::cout<< "PROCESSING_TICK" << '\n';
        std::cout<< "PROCESSING_TICK " << events.size() << '\n';
        while(event_now < events.size()) {
            auto event = events[event_now];
            event_now += 1;

            std::cout<< "ANOTHER_EVENT "<< event.id << '\n';
            switch(event.type) {
                case VisualizationEventType::ADD_START_VERTEX:
                    start_node = event.id;
                    break;
                case VisualizationEventType::ADD_END_VERTEX:
                    end_node = event.id;
                    break;
                case VisualizationEventType::START_VISITING_EDGE:
                    edge_color[event.id] = being_visited_color;
                    edge_width[event.id] = 2;
                    break;
                case VisualizationEventType::END_VISITING_EDGE:
                    edge_color[event.id] = visited_color;
                    edge_width[event.id] = 1.5;
                    break;
                case VisualizationEventType::START_VISITING_VERTEX:
                    node_color[event.id] = being_visited_color;
                    node_radius[event.id] = 2;
                    break;
                case VisualizationEventType::END_VISITING_VERTEX:
                    node_color[event.id] = visited_color;
                    node_radius[event.id] = 1;
                    // The end of the tick
                    return;
            }
        }
    };

    auto closest_point_idx = [&](Vector2 screen_pos) {
        float min_dist = Vector2DistanceSqr(screen_pos, origin + positions[0] * scale);
        uint64_t min_idx = 0;
        for (int i = 1; i < positions.size(); ++i) {
            auto pos = origin + positions[i] * scale;
            auto dist = Vector2DistanceSqr(screen_pos, pos);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = i;
            }
        }
        return min_idx;
    };

    Button dijkstra_button = createButton(50, 700, 300, 40, "dijkstra");
    Button a_star_button = createButton(50, 750, 300, 40, "A*");
    Button double_dijkstra_button = createButton(50, 800, 300, 40, "double_dijkstra");
    Button reset_button = createButton(400, 800, 250, 40, "RESET");
    Slider frequency_slider = make_slider(400, 700, 250, 40, "SIM SPEED: %.1f", &frequency, 0, 16);
    auto draw_ui = [&]() {
        drawButton(dijkstra_button);
        drawButton(a_star_button);
        drawButton(double_dijkstra_button);
        draw_slider(frequency_slider);
        drawButton(reset_button);
        DrawTextStretched(algo.c_str(), 150, 680, 30, BLACK);
        DrawFPS(5, 5);
    };

    auto handle_ui_logic = [&]() {
        if (isButtonClicked(dijkstra_button)) {
            algo = "dijkstra";
        }
        if (isButtonClicked(a_star_button)) {
            algo = "A*";
        }
        if (isButtonClicked(double_dijkstra_button)) {
            algo = "double_dijkstra";
        }
        if (isButtonClicked(reset_button)) {
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

        next_event_timer -= GetFrameTime();
        while (next_event_timer < 0) {
            next_event_timer += powf(2, -frequency);
            process_events_tick();
        }

        // Draw edges
        for (int i = 0; i < graph.num_nodes(); ++i) {
            for (int j = 0; j < graph.adj[i].size(); ++j) {
                auto edge = graph.adj[i][j];
                auto pos0 = positions[i] * scale + origin;
                auto pos1 = positions[edge.to] * scale + origin;
                DrawLineEx(pos0, pos1, edge_width[edge.id], edge_color[edge.id]);
            }
        }

        // Draw nodes
        for (int i = 0; i < graph.num_nodes(); ++i) {
            auto pos = positions[i];
            pos *= scale;
            pos += origin;
            DrawCircleV(pos, node_radius[i], node_color[i]);
        }

        // Draw start and end node
        DrawCircleV(positions[start_node] * scale + origin, 4, BLUE);
        DrawCircleV(positions[end_node] * scale + origin, 4, RED);
        // Draw source and target nodes
        std :: cout << "SOURCE PICKED" << source_picked << std::endl;
        std :: cout << "TARGET PICKED" << target_picked << std::endl;
        if (source_picked >= 0) { DrawCircleV(positions[source_picked] * scale + origin, 8, DARKBLUE); }
        if (target_picked >= 0) { DrawCircleV(positions[target_picked] * scale + origin, 8, MAROON); }

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

    raylib_visualization(result.visualization_events, g, algo);
    return 0;
}
