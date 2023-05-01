cmake_minimum_required(VERSION 3.19.0)

set(target PosAndVelocity)

add_library(${target} STATIC PosAndVelocity.cpp)
add_example(${target})
