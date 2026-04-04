package Transport_algorithms
/**
* AI use: None
*/

import rl "vendor:raylib"
import "core:math"
import h "core:slice/heap"
import "core:fmt"

raw_edge :: struct {
    from: u32, 
    to: u32,
    value: f32
}

edge :: struct {
    other_node: u32,
    value: f32
}

node :: rl.Vector2
F_INF :: math.F32_MAX / 2


graph_static_adjacency_list :: struct {
    num_edges: u32,
    adj: [][]edge,
    nodes: []node,
}

dijsktra_heap_element :: struct {
    node: u32,
    dist: f32
}

dijkstra_data :: struct {
    graph: graph_static_adjacency_list,
    dist: [dynamic]f32,
    heap: [dynamic]dijsktra_heap_element
}

dijkstra_diff :: struct {
    visited_node: u32,
    added_to_queue: [dynamic]u32
}

make_graph_static :: proc (nodes: []node, edges: []raw_edge, alloc := context.allocator) -> graph_static_adjacency_list {
    num_edges := len(edges)
    num_nodes := len(nodes)
    edges_arr := make_dynamic_array_len_cap([dynamic]edge, num_edges, num_edges, alloc)
    adj := make_dynamic_array_len_cap([dynamic][]edge, num_nodes, num_nodes, alloc)
    new_nodes := make_dynamic_array_len_cap([dynamic]node, num_nodes, num_nodes, alloc)
    node_adjacent_edges := make_dynamic_array_len_cap([dynamic]u32, num_nodes + 1, num_nodes + 1, context.temp_allocator)

    copy_slice(new_nodes[:], nodes)

    for e in edges {
        node_adjacent_edges[e.from + 1] += 1
    }
    math.cumsum_inplace(node_adjacent_edges[:])
    
    for i in 1..<len(node_adjacent_edges) {
        start := node_adjacent_edges[i - 1]
        end := node_adjacent_edges[i]
        adj[i - 1] = edges_arr[start:end]
    }

    for e in edges {
        edges_arr[node_adjacent_edges[e.from]] = edge {
            other_node = e.to,
            value = e.value,
        }
        node_adjacent_edges[e.from] += 1
    }

    return graph_static_adjacency_list {
        u32(num_edges),
        adj[:],
        new_nodes[:],
    }
}

// delete_graph_static :: proc

dijkstra_cmp :: proc (e0, e1: dijsktra_heap_element) -> bool {
    if e0.dist == e1.dist { return e0.node > e1.node }
    return e0.dist > e1.dist
}

dijkstra_is_finished :: proc(data: dijkstra_data) -> bool {
    return len(data.heap) == 0
}

dijkstra_step :: proc (data: ^dijkstra_data) -> (diff: dijkstra_diff, already_visited: bool) {
    data := data
    h.pop(data.heap[:], dijkstra_cmp)
    top := data.heap[len(data.heap) - 1]
    unordered_remove(&data.heap, len(data.heap) - 1)

    if data.dist[top.node] < F_INF { 
        already_visited = true
        return
    }
    
    data.dist[top.node] = top.dist
    diff.visited_node = top.node
    for e in data.graph.adj[top.node] {
        if data.dist[e.other_node] < F_INF { continue }
        append(&diff.added_to_queue, e.other_node)
        append(&data.heap, dijsktra_heap_element{
            node = e.other_node,
            dist = e.value + top.dist
        })
        h.push(data.heap[:], dijkstra_cmp)
    }
    
    return
}

dijkstra_start :: proc (graph: graph_static_adjacency_list, start: u32 = 0, alloc:= context.allocator) -> dijkstra_data {
    num_nodes := len(graph.nodes)   
    dist := make_dynamic_array_len_cap([dynamic]f32, num_nodes, num_nodes, alloc)
    for &v in dist {v = F_INF}
    heap := make_dynamic_array([dynamic]dijsktra_heap_element, alloc)
    append(&heap, dijsktra_heap_element{
        node = start,
        dist = 0, 
    })
    
    return dijkstra_data {
        graph,
        dist,
        heap
    }
}
origin: rl.Vector2 = {20, 20}
scale: f32 = 15.0

draw_graph :: proc(graph: graph_static_adjacency_list) {
    for edges, node_id in graph.adj {
        for edge in edges {
            start := graph.nodes[node_id] * scale + origin
            end := graph.nodes[edge.other_node] * scale + origin
            rl.DrawLineEx(start, end, 3, rl.GRAY)
            triangle_pos := start * 1 / 3 + end * 2 / 3
            rl.DrawPoly(triangle_pos, 3, 8, -rl.Vector2Angle(end - start, rl.Vector2{1, 0}) * rl.RAD2DEG, rl.GRAY)

        }
    }
    for node in graph.nodes {
        rl.DrawCircleV(node * scale + origin, 10, rl.BLACK)
    }
}

draw_dijkstra_diff :: proc (diff: dijkstra_diff, data: dijkstra_data, graph: graph_static_adjacency_list) {
    for node, node_id in graph.nodes {
        color := rl.BLACK
        if data.dist[node_id] < F_INF {
            color = rl.DARKBLUE
        }
        rl.DrawCircleV(node * scale + origin, 10, color)
    }

    rl.DrawCircleV(graph.nodes[diff.visited_node] * scale + origin, 14, rl.GREEN)
    
    for node0 in diff.added_to_queue {
        rl.DrawCircleV(graph.nodes[node0] * scale + origin, 12, rl.YELLOW)
    }
} 

main :: proc () {
    nodes := nodes_20
    edges := edges_20
    graph := make_graph_static(nodes[:], edges[:])
    
    rl.InitWindow(800, 600, "Transport algorithms")
    
    dijkstra_timer : f32 = 2
    
    dijkstra_data := dijkstra_start(graph, 0)
    diff: dijkstra_diff
    for !rl.WindowShouldClose() {
        dt := rl.GetFrameTime()
        
        dijkstra_timer -= dt
        if dijkstra_timer < 0 && !dijkstra_is_finished(dijkstra_data){
            already_visited := true
            for already_visited && !dijkstra_is_finished(dijkstra_data) {
                diff, already_visited = dijkstra_step(&dijkstra_data)
            }
            dijkstra_timer += 1
            
        }
        
        rl.BeginDrawing()
        rl.ClearBackground(rl.WHITE)
        draw_graph(graph)
        draw_dijkstra_diff(diff, dijkstra_data, graph)
        rl.EndDrawing()
    }
}