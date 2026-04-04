enum class VisualizationEventType {
    ADD_START_VERTEX,
    ADD_END_VERTEX,
    START_VISITING_VERTEX,
    END_VISITING_VERTEX,
    START_VISITING_EDGE,
    END_VISITING_EDGE,
};

struct VisualizationEvent {
    VisualizationEventType type;
    uint64_t id;
};
