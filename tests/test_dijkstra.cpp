#include <gtest/gtest.h>
#include "Graph.hpp"
#include "algorithms/Dijkstra.hpp"

// Test 1: Empty Graph Edge Cases
TEST(DijkstraTest, EmptyGraph) {
    Graph<int, double> g;
    auto res = dijkstra(g, 1);
    EXPECT_TRUE(res.dist.empty());
    EXPECT_TRUE(res.prev.empty());
    EXPECT_TRUE(reconstructPath(res, 1, 2).empty());
}

// Test 2: Single Node Edge Cases
TEST(DijkstraTest, SingleNode) {
    Graph<int, double> g;
    g.addNode(1);
    auto res = dijkstra(g, 1);
    EXPECT_EQ(res.dist.size(), 1);
    EXPECT_EQ(res.dist[1], 0.0);
    EXPECT_TRUE(res.prev.empty());

    auto path = reconstructPath(res, 1, 1);
    ASSERT_EQ(path.size(), 1);
    EXPECT_EQ(path[0], 1);
}

// Test 3: Disconnected Graph
TEST(DijkstraTest, DisconnectedGraph) {
    Graph<int, double> g(false);
    g.addEdge(1, 2, 5.0);
    g.addEdge(3, 4, 2.0);

    auto res = dijkstra(g, 1);
    EXPECT_EQ(res.dist[1], 0.0);
    EXPECT_EQ(res.dist[2], 5.0);
    
    // Node 3 is unreachable from 1
    double INF = std::numeric_limits<double>::infinity();
    EXPECT_EQ(res.dist[3], INF);
    
    auto path = reconstructPath(res, 1, 3);
    EXPECT_TRUE(path.empty());
}

// Test 4: Known Correct Output Verification
TEST(DijkstraTest, KnownOutput) {
    Graph<std::string, int> g(true); // directed
    g.addEdge("A", "B", 4);
    g.addEdge("A", "C", 2);
    g.addEdge("C", "B", 1);
    g.addEdge("B", "D", 5);
    g.addEdge("C", "D", 8);

    auto res = dijkstra(g, std::string("A"));
    EXPECT_EQ(res.dist["A"], 0);
    EXPECT_EQ(res.dist["C"], 2);
    EXPECT_EQ(res.dist["B"], 3); // A -> C -> B is cost 3, cheaper than A -> B (cost 4)
    EXPECT_EQ(res.dist["D"], 8); // A -> C -> B -> D is cost 8

    auto path = reconstructPath(res, std::string("A"), std::string("D"));
    ASSERT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], "A");
    EXPECT_EQ(path[1], "C");
    EXPECT_EQ(path[2], "B");
    EXPECT_EQ(path[3], "D");
}

// Test 5: Performance Test (Large Graph)
TEST(DijkstraTest, PerformanceLargeGraph) {
    Graph<int, int> g(false);
    int N = 1000;
    for (int i = 0; i < N - 1; ++i) {
        g.addEdge(i, i + 1, 2);
    }
    auto res = dijkstra(g, 0);
    EXPECT_EQ(res.dist[N - 1], (N - 1) * 2);
}
