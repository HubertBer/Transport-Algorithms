#pragma once

#include <cstdint>
#include <vector>

enum class VisualizationEventType {
  ADD_START_VERTEX,
  ADD_END_VERTEX,
  START_VISITING_VERTEX,
  END_VISITING_VERTEX,
  START_VISITING_EDGE,
  END_VISITING_EDGE,
  LANDMARK,
};

struct VisualizationEvent {
  VisualizationEventType type;
  uint64_t id;
};

struct VisualisationQueue {
  std::vector<VisualizationEvent> events;

  void push(VisualizationEventType type, uint64_t id) {
    events.push_back(VisualizationEvent{.type = type, .id = id});
  }

  void add_start_vertex(uint64_t id) {
    push(VisualizationEventType::ADD_START_VERTEX, id);
  }

  void add_end_vertex(uint64_t id) {
    push(VisualizationEventType::ADD_END_VERTEX, id);
  }

  void start_visiting_vertex(uint64_t id) {
    push(VisualizationEventType::START_VISITING_VERTEX, id);
  }

  void end_visiting_vertex(uint64_t id) {
    push(VisualizationEventType::END_VISITING_VERTEX, id);
  }

  void start_visiting_edge(uint64_t id) {
    push(VisualizationEventType::START_VISITING_EDGE, id);
  }

  void end_visiting_edge(uint64_t id) {
    push(VisualizationEventType::END_VISITING_EDGE, id);
  }
};
