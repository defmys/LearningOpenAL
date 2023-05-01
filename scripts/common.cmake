
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++20 \
    -Wall \
    -Werror \
    -Wno-extra-tokens \
    -I/usr/local/include \
    -I/opt/homebrew/include \
    ")

macro(add_example target)
    target_include_directories(${target} PUBLIC ${source_root})
    target_include_directories(${target} PRIVATE /opt/homebrew/opt/openal-soft/include)
    add_dependencies(${target} copy_${target}_resources)

    add_custom_target(
        copy_${target}_resources
        COMMAND cp -r ${CMAKE_CURRENT_LIST_DIR}/resource/ ${PROJECT_BINARY_DIR}/resource/${target}/ || (exit 0)
    )

    list(APPEND example_targets ${target})
    set(example_targets ${example_targets} PARENT_SCOPE)
endmacro()
