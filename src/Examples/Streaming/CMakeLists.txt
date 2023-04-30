cmake_minimum_required(VERSION 3.19.0)

set(target Streaming)

add_library(${target} STATIC Streaming.cpp)
add_example(${target})
