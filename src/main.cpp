#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <tuple>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/BellmanFord.hpp"
#include "algorithms/Kruskal.hpp"
#include "algorithms/TarjanSCC.hpp"
#include "algorithms/TopologicalSort.hpp"

int main() {
    std::cout << "==================================================\n";
    std::cout << "  Graph Engine - Connectivity & Sorting Demo      \n";
    std::cout << "==================================================\n\n";

    // ============================================================================
    // 1. Kruskal Minimum Spanning Tree
    // ============================================================================
    std::cout << "[1] Running Kruskal's Minimum Spanning Tree (Undirected):\n";
    Graph<int, int> undirGraph(false); // Undirected

    undirGraph.addEdge(1, 2, 4);
    undirGraph.addEdge(1, 3, 2);
    undirGraph.addEdge(2, 3, 1);
    undirGraph.addEdge(3, 4, 5);
    undirGraph.addEdge(2, 4, 3);

    std::cout << "  Graph edges:\n";
    std::cout << "    1 - 2 (wt 4), 1 - 3 (wt 2), 2 - 3 (wt 1), 3 - 4 (wt 5), 2 - 4 (wt 3)\n";

    auto mst = kruskal(undirGraph);
    int totalMSTWeight = 0;
    std::cout << "  Selected MST edges:\n";
    for (const auto& edge : mst) {
        auto [wt, u, v] = edge;
        std::cout << "    Node " << u << " -- " << v << " (weight: " << wt << ")\n";
        totalMSTWeight += wt;
    }
    std::cout << "  Total MST Weight: " << totalMSTWeight << " (Expected: 6)\n\n";

    // ============================================================================
    // 2. Tarjan's Strongly Connected Components
    // ============================================================================
    std::cout << "[2] Running Tarjan's SCC Algorithm on the Sample Directed Graph:\n";
    Graph<int, int> directedSample(true); // Directed

    directedSample.addEdge(0, 1, 1);
    directedSample.addEdge(1, 2, 1);
    directedSample.addEdge(2, 0, 1);
    directedSample.addEdge(2, 3, 3);
    directedSample.addEdge(3, 4, 2);
    directedSample.addEdge(4, 5, 2);
    directedSample.addEdge(5, 3, 2);
    directedSample.addEdge(4, 6, 1);
    directedSample.addEdge(6, 7, 4);
    directedSample.addEdge(7, 8, -2);
    directedSample.addEdge(8, 9, 3);
    directedSample.addEdge(9, 10, 2);
    directedSample.addEdge(10, 11, 1);
    directedSample.addEdge(7, 9, 5);
    directedSample.addEdge(10, 6, 2);

    TarjanSCC<int, int> tarjan;
    auto sccs = tarjan.findSCC(directedSample);

    std::cout << "  Found strongly connected components (grouped by curly brackets):\n";
    for (size_t i = 0; i < sccs.size(); ++i) {
        std::cout << "    Component " << i + 1 << ": { ";
        for (const auto& node : sccs[i]) {
            std::cout << node << " ";
        }
        std::cout << "}\n";
    }
    std::cout << "  Total Components: " << sccs.size() << " (Expected: 4)\n\n";

    // ============================================================================
    // 3. Topological Sort (DAG dependency)
    // ============================================================================
    std::cout << "[3] Running Topological Sort on a DAG:\n";
    
    Graph<std::string, int> dag(true);
    dag.addEdge("CS101", "CS201", 1);
    dag.addEdge("CS101", "CS202", 1);
    dag.addEdge("CS201", "CS301", 1);
    dag.addEdge("CS202", "CS301", 1);

    std::cout << "  DAG Dependencies: CS101 -> CS201/CS202 -> CS301\n";

    try {
        auto kahnOrder = topologicalSort(dag);
        std::cout << "    Kahn's BFS Order: ";
        for (size_t i = 0; i < kahnOrder.size(); ++i) {
            std::cout << kahnOrder[i] << (i + 1 < kahnOrder.size() ? " -> " : "");
        }
        std::cout << "\n";

        auto dfsOrder = topologicalSortDFS(dag);
        std::cout << "    DFS-Based Order:  ";
        for (size_t i = 0; i < dfsOrder.size(); ++i) {
            std::cout << dfsOrder[i] << (i + 1 < dfsOrder.size() ? " -> " : "");
        }
        std::cout << "\n";
    } catch (const std::exception& e) {
        std::cerr << "    Error: " << e.what() << "\n";
    }
    std::cout << "\n";

    // ============================================================================
    // 4. Topological Sort Cycle Detection
    // ============================================================================
    std::cout << "[4] Testing Topological Sort Cycle Detection:\n";
    
    Graph<std::string, int> cyclicDag(true);
    cyclicDag.addEdge("A", "B", 1);
    cyclicDag.addEdge("B", "C", 1);
    cyclicDag.addEdge("C", "A", 1); // Creates loop: A -> B -> C -> A

    std::cout << "  Running Topological Sort on cyclic graph A -> B -> C -> A...\n";
    try {
        topologicalSort(cyclicDag);
        std::cout << "    Topological sort succeeded (Unexpected: Cycle should have been caught!)\n";
    } catch (const std::runtime_error& e) {
        std::cout << "    Caught expected exception: \"" << e.what() << "\" (Success! Cycle detected)\n";
    }

    std::cout << "\n==================================================\n";
    std::cout << "All Connectivity & Sorting algorithms demonstrated successfully!\n";
    std::cout << "==================================================\n";

    return 0;
}
