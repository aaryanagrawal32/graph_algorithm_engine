cmake_minimum_required(VERSION 3.15)
project(GraphEngine VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
add_compile_options(-Wall -Wextra -O2)

# Main executable
add_executable(graph_engine src/main.cpp)
target_include_directories(graph_engine PRIVATE include)

# NOTE: The following directories/targets are commented out for Session 01
# as they will be created in later phases of the project.
# Uncomment them as those files/directories are added.

# Tests (Google Test)
enable_testing()
add_subdirectory(tests)

# Benchmarks
add_executable(benchmark benchmarks/benchmark.cpp)
target_include_directories(benchmark PRIVATE include)

