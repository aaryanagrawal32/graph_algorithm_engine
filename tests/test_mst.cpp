#include <gtest/gtest.h>
#include "Graph.hpp"
#include "algorithms/Kruskal.hpp"
#include <tuple>
#include <string>

// Test 1: DSU Functionality (makeSet, find, unite)
TEST(KruskalTest, DSUBasic) {
    DSU<int> dsu;
    dsu.makeSet(1);
    dsu.makeSet(2);
    dsu.makeSet(3);

    // Initial roots should be themselves
    EXPECT_EQ(dsu.find(1), 1);
    EXPECT_EQ(dsu.find(2), 2);
    EXPECT_EQ(dsu.find(3), 3);

    // Unite 1 and 2
    EXPECT_TRUE(dsu.unite(1, 2));
    EXPECT_EQ(dsu.find(1), dsu.find(2));

    // Trying to unite them again should return false (cycle/same component)
    EXPECT_FALSE(dsu.unite(1, 2));

    // 3 is still separate
    EXPECT_NE(dsu.find(1), dsu.find(3));

    // Unite 2 and 3
    EXPECT_TRUE(dsu.unite(2, 3));
    EXPECT_EQ(dsu.find(1), dsu.find(3));
}

// Test 2: Empty Graph Edge Case
TEST(KruskalTest, EmptyGraph) {
    Graph<int, int> g(false); // undirected
    auto mst = kruskal(g);
    EXPECT_TRUE(mst.empty());
}

// Test 3: Single Node Edge Case
TEST(KruskalTest, SingleNode) {
    Graph<int, int> g(false); // undirected
    g.addNode(1);
    auto mst = kruskal(g);
    EXPECT_TRUE(mst.empty());
}

// Test 4: Directed Graph Exception
TEST(KruskalTest, DirectedGraphThrows) {
    Graph<int, int> g(true); // directed
    g.addEdge(1, 2, 5);
    EXPECT_THROW(kruskal(g), std::invalid_argument);
}

// Test 5: Disconnected Graph (Spanning Forest)
TEST(KruskalTest, DisconnectedForest) {
    Graph<int, int> g(false); // undirected
    g.addEdge(1, 2, 3);
    g.addEdge(3, 4, 5);

    auto mst = kruskal(g);
    // Should contain exactly 2 edges: 1-2 (wt 3) and 3-4 (wt 5)
    EXPECT_EQ(mst.size(), 2);
    
    int weightSum = 0;
    for (const auto& [w, u, v] : mst) {
        weightSum += w;
    }
    EXPECT_EQ(weightSum, 8);
}

// Test 6: Known Correct Output Verification
TEST(KruskalTest, KnownMST) {
    Graph<std::string, int> g(false); // undirected
    g.addEdge("A", "B", 4);
    g.addEdge("A", "H", 8);
    g.addEdge("B", "C", 8);
    g.addEdge("B", "H", 11);
    g.addEdge("C", "D", 7);
    g.addEdge("C", "F", 4);
    g.addEdge("C", "I", 2);
    g.addEdge("D", "E", 9);
    g.addEdge("D", "F", 14);
    g.addEdge("E", "F", 10);
    g.addEdge("F", "G", 2);
    g.addEdge("G", "H", 1);
    g.addEdge("G", "I", 6);
    g.addEdge("H", "I", 7);

    auto mst = kruskal(g);

    // Number of vertices = 9, so MST should have exactly 8 edges
    EXPECT_EQ(mst.size(), 8);

    int totalWeight = 0;
    for (const auto& [w, u, v] : mst) {
        totalWeight += w;
    }
    // Correct total MST weight for standard example is 37
    EXPECT_EQ(totalWeight, 37);
}
