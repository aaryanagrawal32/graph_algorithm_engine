#include <gtest/gtest.h>
#include "Graph.hpp"
#include "algorithms/TarjanSCC.hpp"
#include <vector>
#include <string>
#include <algorithm>

// Helper to check if a component contains a node
template<typename T>
bool contains(const std::vector<T>& vec, const T& val) {
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

// Helper to check if two components are equal regardless of element order
template<typename T>
bool componentEquals(std::vector<T> a, std::vector<T> b) {
    if (a.size() != b.size()) return false;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

// Helper to check if the list of SCCs contains a specific component
template<typename T>
bool hasSCC(const std::vector<std::vector<T>>& sccs, const std::vector<T>& target) {
    for (const auto& scc : sccs) {
        if (componentEquals(scc, target)) {
            return true;
        }
    }
    return false;
}

// Test 1: Empty Graph Edge Cases
TEST(TarjanSCCTest, EmptyGraph) {
    Graph<int, int> g(true); // directed
    TarjanSCC<int, int> solver;
    auto sccs = solver.findSCC(g);
    EXPECT_TRUE(sccs.empty());
}

// Test 2: Single Node Edge Cases
TEST(TarjanSCCTest, SingleNode) {
    Graph<int, int> g(true); // directed
    g.addNode(1);
    TarjanSCC<int, int> solver;
    auto sccs = solver.findSCC(g);
    ASSERT_EQ(sccs.size(), 1);
    EXPECT_EQ(sccs[0].size(), 1);
    EXPECT_EQ(sccs[0][0], 1);
}

// Test 3: Undirected Graph Throws Exception
TEST(TarjanSCCTest, UndirectedThrows) {
    Graph<int, int> g(false); // undirected
    g.addEdge(1, 2, 5);
    TarjanSCC<int, int> solver;
    EXPECT_THROW(solver.findSCC(g), std::invalid_argument);
}

// Test 4: Disconnected Components
TEST(TarjanSCCTest, Disconnected) {
    Graph<int, int> g(true);
    g.addNode(1);
    g.addNode(2);
    g.addNode(3);

    TarjanSCC<int, int> solver;
    auto sccs = solver.findSCC(g);
    EXPECT_EQ(sccs.size(), 3);
    EXPECT_TRUE(hasSCC(sccs, {1}));
    EXPECT_TRUE(hasSCC(sccs, {2}));
    EXPECT_TRUE(hasSCC(sccs, {3}));
}

// Test 5: Known Directed Cyclic Components
TEST(TarjanSCCTest, CyclicComponents) {
    // Standard Tarjan's SCC test case:
    // 0 -> 1, 1 -> 2, 2 -> 0
    // 1 -> 3, 3 -> 4, 4 -> 5, 5 -> 3
    // 4 -> 6, 6 -> 7, 7 -> 6
    Graph<int, int> g(true);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    g.addEdge(1, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 3);

    g.addEdge(4, 6);
    g.addEdge(6, 7);
    g.addEdge(7, 6);

    TarjanSCC<int, int> solver;
    auto sccs = solver.findSCC(g);

    // Expected SCCs:
    // SCC 1: {0, 1, 2}
    // SCC 2: {3, 4, 5}
    // SCC 3: {6, 7}
    EXPECT_EQ(sccs.size(), 3);
    EXPECT_TRUE(hasSCC(sccs, {0, 1, 2}));
    EXPECT_TRUE(hasSCC(sccs, {3, 4, 5}));
    EXPECT_TRUE(hasSCC(sccs, {6, 7}));
}
