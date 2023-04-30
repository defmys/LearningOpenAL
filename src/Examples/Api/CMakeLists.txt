cmake_minimum_required(VERSION 3.19.0)

set(target Api)

add_library(${target} STATIC ApiExample.cpp)
add_example(${target})
