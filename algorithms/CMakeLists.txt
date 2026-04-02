add_library(algorithms INTERFACE)
target_include_directories(algorithms INTERFACE .)
target_link_libraries(algorithms INTERFACE common)
