cmake_minimum_required(VERSION 3.19.0)

set( target learning_openal )

file(GLOB_RECURSE source ${source_root}/Framework/*.cpp)

add_executable(
    ${target} 
    ${source}
    ${CMAKE_BINARY_DIR}/includes.inl
    ${CMAKE_BINARY_DIR}/register_examples.inl
)
set_target_properties(${target} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(${target} PRIVATE ${source_root})
target_include_directories(${target} PRIVATE /opt/homebrew/opt/openal-soft/include)
target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(${target} ${openal} SndFile::sndfile)

foreach(example_target ${example_targets})
    target_link_libraries(${target} ${example_target})
endforeach()
