cmake_minimum_required(VERSION 3.19.0)

set(project_root ${CMAKE_CURRENT_SOURCE_DIR})
set(source_root ${project_root}/src)

include(${project_root}/scripts/common.cmake)

project( learning_openal )

find_library(
    openal
    openal
    REQUIRED 
    PATHS /opt/homebrew/opt/openal-soft/lib
)

find_package(SndFile REQUIRED)

add_subdirectory(src/Examples)
add_subdirectory(src/Framework)

message("Registered Examples: ${example_targets}")

#Include headers
foreach(example_target ${example_targets})
    list(APPEND include_example_headers_list "#include \"Examples/${example_target}/${example_target}.h\"")
endforeach()
list(JOIN include_example_headers_list ";\n" include_example_headers)
string(APPEND include_example_headers ";\n")

configure_file(${source_root}/Framework/includes.inl.in includes.inl)

#Register example instances.
foreach(example_target ${example_targets})
    list(APPEND register_example_list "examples.emplace(\"${example_target}\", std::make_unique<${example_target}>())")
endforeach()
list(JOIN register_example_list ";\n" register_examples)
string(APPEND register_examples ";\n")
configure_file(${source_root}/Framework/register_examples.inl.in register_examples.inl)
