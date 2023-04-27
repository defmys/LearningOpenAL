cmake_minimum_required(VERSION 3.19.0)

set(target Api)

add_library(${target} STATIC ApiExample.cpp)
target_include_directories(${target} PUBLIC ${source_root})
target_include_directories(${target} PRIVATE /opt/homebrew/opt/openal-soft/include)
add_dependencies(${target} copy_${target}_resources)

add_custom_target(
    copy_${target}_resources
    COMMAND cp -r ${CMAKE_CURRENT_LIST_DIR}/resource/ ${PROJECT_BINARY_DIR}/resource/${target}/
)
