#include <gtest/gtest.h>
#include "Graph.hpp"

// Test 1: Empty Graph Edge Case
TEST(GraphTest, EmptyGraph) {
    Graph<int> g;
    EXPECT_EQ(g.nodeCount(), 0);
    EXPECT_EQ(g.edgeCount(), 0);
    EXPECT_FALSE(g.hasNode(1));
}

// Test 2: Single Node Edge Case
TEST(GraphTest, SingleNode) {
    Graph<int> g;
    g.addNode(1);
    EXPECT_EQ(g.nodeCount(), 1);
    EXPECT_TRUE(g.hasNode(1));
    EXPECT_EQ(g.neighbors(1).size(), 0);
}

// Test 3: Disconnected Graph
TEST(GraphTest, DisconnectedGraph) {
    Graph<int> g;
    g.addNode(1);
    g.addNode(2);
    EXPECT_EQ(g.nodeCount(), 2);
    EXPECT_EQ(g.edgeCount(), 0);
    EXPECT_FALSE(g.hasEdge(1, 2));
}

// Test 4: Known Correct Output Verification (Adding/Removing Edges)
TEST(GraphTest, AddRemoveEdges) {
    Graph<std::string, double> g(true); // directed
    g.addEdge("A", "B", 1.5);
    g.addEdge("A", "C", 2.5);
    g.addEdge("B", "C", 0.5);

    EXPECT_EQ(g.nodeCount(), 3);
    EXPECT_EQ(g.edgeCount(), 3);
    EXPECT_TRUE(g.hasEdge("A", "B"));
    EXPECT_TRUE(g.hasEdge("B", "C"));
    EXPECT_FALSE(g.hasEdge("C", "A"));

    g.removeEdge("A", "B");
    EXPECT_FALSE(g.hasEdge("A", "B"));
    EXPECT_EQ(g.edgeCount(), 2);
}

// Test 5: Performance Test (Large Graph)
TEST(GraphTest, PerformanceLargeGraph) {
    Graph<int> g(false); // undirected
    int N = 1000;
    for (int i = 0; i < N; ++i) {
        g.addNode(i);
    }
    for (int i = 0; i < N - 1; ++i) {
        g.addEdge(i, i + 1, 1);
    }
    EXPECT_EQ(g.nodeCount(), 1000);
    EXPECT_EQ(g.edgeCount(), 999);
}
