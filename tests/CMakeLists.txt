include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Windows: force shared CRT linking to match gtest's defaults
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# One add_executable per test file, each linked against gtest_main
foreach(test_file
    test_graph
    test_bfs_dfs
    test_dijkstra
    test_astar
    test_mst
    test_scc
)
    add_executable(${test_file} ${test_file}.cpp)
    target_include_directories(${test_file} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_file} GTest::gtest_main)
    add_test(NAME ${test_file} COMMAND ${test_file})
endforeach()
