#include <gtest/gtest.h>
#include "Graph.hpp"
#include "algorithms/AStar.hpp"

// Test 1: Empty Graph Edge Cases
TEST(AStarTest, EmptyGraph) {
    Graph<std::pair<int, int>, double> g;
    auto res = astar(g, {0, 0}, {2, 2}, manhattanHeuristic);
    EXPECT_TRUE(res.path.empty());
    EXPECT_EQ(res.nodesVisited, 0);
}

// Test 2: Single Node Edge Cases
TEST(AStarTest, SingleNode) {
    Graph<std::pair<int, int>, double> g;
    g.addNode({0, 0});
    auto res = astar(g, {0, 0}, {0, 0}, manhattanHeuristic);
    ASSERT_EQ(res.path.size(), 1);
    EXPECT_EQ(res.path[0].first, 0);
    EXPECT_EQ(res.path[0].second, 0);
}

// Test 3: Disconnected Graph
TEST(AStarTest, DisconnectedGraph) {
    Graph<std::pair<int, int>, double> g(false);
    g.addEdge({0, 0}, {0, 1}, 1.0);
    g.addEdge({1, 0}, {1, 1}, 1.0);

    auto res = astar(g, {0, 0}, {1, 1}, manhattanHeuristic);
    EXPECT_TRUE(res.path.empty()); // unreachable
}

// Test 4: Known Correct Output Verification (Grid navigation around a wall)
TEST(AStarTest, GridWithWall) {
    Graph<std::pair<int, int>, double> g(false);

    // 3x3 grid with wall at (1, 0) and (1, 1)
    // Node (1, 2) is the only way through.
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            g.addNode({x, y});
        }
    }

    // Connect grid edges, skipping wall blockages:
    // We disconnect edges through (1, 0) and (1, 1)
    auto blocked = [](int x, int y) {
        return (x == 1 && (y == 0 || y == 1));
    };

    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            if (blocked(x, y)) continue;
            std::pair<int, int> curr = {x, y};
            if (x + 1 < 3 && !blocked(x + 1, y)) {
                g.addEdge(curr, {x + 1, y}, 1.0);
            }
            if (y + 1 < 3 && !blocked(x, y + 1)) {
                g.addEdge(curr, {x, y + 1}, 1.0);
            }
        }
    }

    // Path from (0,0) to (2,0)
    auto res = astar(g, {0, 0}, {2, 0}, manhattanHeuristic);

    // Optimal path must go down to y=2 and back up:
    // (0,0) -> (0,1) -> (0,2) -> (1,2) -> (2,2) -> (2,1) -> (2,0)
    ASSERT_EQ(res.path.size(), 7);
    EXPECT_EQ(res.path[0], std::make_pair(0, 0));
    EXPECT_EQ(res.path[2], std::make_pair(0, 2));
    EXPECT_EQ(res.path[3], std::make_pair(1, 2));
    EXPECT_EQ(res.path[6], std::make_pair(2, 0));
}

// Test 5: Performance Test (A* vs Zero Heuristic / Dijkstra)
TEST(AStarTest, PerformanceBench) {
    Graph<std::pair<int, int>, double> g(false);
    int size = 20; // 20x20 grid (400 nodes)
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            g.addNode({x, y});
        }
    }
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            std::pair<int, int> curr = {x, y};
            if (x + 1 < size) g.addEdge(curr, {x + 1, y}, 1.0);
            if (y + 1 < size) g.addEdge(curr, {x, y + 1}, 1.0);
        }
    }

    std::pair<int, int> start = {0, 0};
    std::pair<int, int> goal = {size - 1, size - 1};

    auto resAStar = astar(g, start, goal, manhattanHeuristic);
    auto resDijkstra = astar(g, start, goal, zeroHeuristic); // zero heuristic = Dijkstra

    EXPECT_EQ(resAStar.path.size(), resDijkstra.path.size());
    // A* should visit significantly fewer nodes than uniform Dijkstra
    EXPECT_LT(resAStar.nodesVisited, resDijkstra.nodesVisited);
}
