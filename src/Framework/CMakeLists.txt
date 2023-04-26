cmake_minimum_required(VERSION 3.19.0)

set( target learning_open_al )

file(GLOB_RECURSE source ${source_root}/*.cpp)

add_executable(${target} ${source})
set_target_properties(${target} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(${target} copy_resources)
target_include_directories(${target} PRIVATE /opt/homebrew/opt/openal-soft/include)
target_link_libraries(${target} ${openal} SndFile::sndfile)

