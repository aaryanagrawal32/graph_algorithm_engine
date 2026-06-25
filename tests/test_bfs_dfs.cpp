#include <gtest/gtest.h>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"

// Test 1: Empty Graph Edge Cases
TEST(BFS_DFSTest, EmptyGraph) {
    Graph<int> g(true);
    auto bfsOrder = bfs(g, 1);
    auto dfsOrder = dfs(g, 1);
    EXPECT_TRUE(bfsOrder.empty());
    EXPECT_TRUE(dfsOrder.empty());
    EXPECT_TRUE(bfsShortestPath(g, 1, 2).empty());
    EXPECT_FALSE(hasCycle(g));
    EXPECT_TRUE(isConnected(g)); // Empty graph is connected
}

// Test 2: Single Node Edge Cases
TEST(BFS_DFSTest, SingleNode) {
    Graph<int> g(true);
    g.addNode(1);
    auto bfsOrder = bfs(g, 1);
    auto dfsOrder = dfs(g, 1);
    ASSERT_EQ(bfsOrder.size(), 1);
    EXPECT_EQ(bfsOrder[0], 1);
    ASSERT_EQ(dfsOrder.size(), 1);
    EXPECT_EQ(dfsOrder[0], 1);

    auto path = bfsShortestPath(g, 1, 1);
    ASSERT_EQ(path.size(), 1);
    EXPECT_EQ(path[0], 1);

    EXPECT_FALSE(hasCycle(g));
    EXPECT_TRUE(isConnected(g));
}

// Test 3: Disconnected Graph
TEST(BFS_DFSTest, DisconnectedGraph) {
    Graph<int> g(false); // undirected
    g.addEdge(1, 2, 1);
    g.addEdge(3, 4, 1);

    auto bfsOrder = bfs(g, 1);
    EXPECT_EQ(bfsOrder.size(), 2); // only visits 1 and 2
    
    auto path = bfsShortestPath(g, 1, 3);
    EXPECT_TRUE(path.empty()); // unreachable

    EXPECT_FALSE(isConnected(g));
}

// Test 4: Known Correct Output Verification
TEST(BFS_DFSTest, KnownOutput) {
    Graph<int> g(true); // directed
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(2, 4, 1);
    g.addEdge(3, 4, 1);

    auto bfsOrder = bfs(g, 1);
    // BFS level-order: 1 -> (2, 3) -> 4
    ASSERT_EQ(bfsOrder.size(), 4);
    EXPECT_EQ(bfsOrder[0], 1);
    EXPECT_TRUE((bfsOrder[1] == 2 && bfsOrder[2] == 3) || (bfsOrder[1] == 3 && bfsOrder[2] == 2));
    EXPECT_EQ(bfsOrder[3], 4);

    // BFS shortest path
    auto path = bfsShortestPath(g, 1, 4);
    EXPECT_EQ(path.size(), 3); // 1 -> 2 -> 4 or 1 -> 3 -> 4
    EXPECT_EQ(path[0], 1);
    EXPECT_EQ(path[2], 4);

    // Cycle detection
    EXPECT_FALSE(hasCycle(g));
    
    g.addEdge(4, 1, 1); // creates cycle
    EXPECT_TRUE(hasCycle(g));
}

// Test 5: Performance Test (Large Graph)
TEST(BFS_DFSTest, PerformanceLargeGraph) {
    Graph<int> g(false);
    int N = 1000;
    for (int i = 0; i < N - 1; ++i) {
        g.addEdge(i, i + 1, 1);
    }
    auto bfsOrder = bfs(g, 0);
    EXPECT_EQ(bfsOrder.size(), 1000);
}
