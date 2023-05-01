cmake_minimum_required(VERSION 3.19.0)

add_subdirectory(ApiExample)
add_subdirectory(Streaming)

set(example_targets ${example_targets} PARENT_SCOPE)
