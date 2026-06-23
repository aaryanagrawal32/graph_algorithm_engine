#include <iostream>
#include <string>
#include <vector>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"

int main() {
    std::cout << "==================================================\n";
    std::cout << "    Graph Algorithm Engine - Session 03 Demo      \n";
    std::cout << "==================================================\n\n";

    // 1. Constructing the sample graph specified in data/sample_graph.csv
    std::cout << "[1] Constructing Sample Graph from spec...\n";
    Graph<int, int> g(true); // directed graph

    // Edges from CSV: source,destination,weight
    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(2, 0, 1);
    g.addEdge(2, 3, 3);
    g.addEdge(3, 4, 2);
    g.addEdge(4, 5, 2);
    g.addEdge(5, 3, 2);
    g.addEdge(4, 6, 1);
    g.addEdge(6, 7, 4);
    g.addEdge(7, 8, -2);
    g.addEdge(8, 9, 3);
    g.addEdge(9, 10, 2);
    g.addEdge(10, 11, 1);
    g.addEdge(7, 9, 5);
    g.addEdge(10, 6, 2);

    std::cout << "Sample Graph created successfully!\n";
    std::cout << "Nodes count: " << g.nodeCount() << " (Expected: 12)\n";
    std::cout << "Edges count: " << g.edgeCount() << " (Expected: 15)\n\n";

    // 2. BFS Traversal starting from node 0
    std::cout << "[2] Running BFS Traversal starting from node 0:\n";
    std::vector<int> bfsOrder = bfs(g, 0);
    std::cout << "  BFS traversal order: ";
    for (size_t i = 0; i < bfsOrder.size(); ++i) {
        std::cout << bfsOrder[i] << (i + 1 < bfsOrder.size() ? " -> " : "");
    }
    std::cout << "\n\n";

    // Helper lambda to print paths
    auto printPath = [](const std::string& name, const std::vector<int>& path) {
        std::cout << "  Shortest path " << name << ": ";
        if (path.empty()) {
            std::cout << "UNREACHABLE\n";
        } else {
            for (size_t i = 0; i < path.size(); ++i) {
                std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
            }
            std::cout << "\n";
        }
    };

    // 3. BFS Shortest Path
    std::cout << "[3] Computing Shortest Paths (using BFS unweighted shortest path):\n";
    
    // Path 0 -> 5 (Unweighted shortest path)
    std::vector<int> path0_5 = bfsShortestPath(g, 0, 5);
    printPath("0 to 5", path0_5);

    // Path 6 -> 11 (Unweighted shortest path: 6 -> 7 -> 9 -> 10 -> 11)
    // BFS finds the 4-edge path rather than the 5-edge path through node 8.
    std::vector<int> path6_11 = bfsShortestPath(g, 6, 11);
    printPath("6 to 11", path6_11);

    // Path 7 -> 9 (Unweighted shortest path: 7 -> 9)
    // Note: The edge weights are 7->9 = 5, and 7->8->9 = -2 + 3 = 1.
    // In a weighted graph, 7 -> 8 -> 9 is cheaper (cost 1) than 7 -> 9 (cost 5).
    // But since BFS is unweighted, it will output 7 -> 9. This demonstrates that
    // BFS is hop-count optimal, not weight optimal!
    std::vector<int> path7_9 = bfsShortestPath(g, 7, 9);
    printPath("7 to 9 (Weighted cost = 5, but hop-optimal)", path7_9);

    // Path 5 -> 0 (Unreachable case)
    std::vector<int> path5_0 = bfsShortestPath(g, 5, 0);
    printPath("5 to 0 (unreachable)", path5_0);

    std::cout << "\n==================================================\n";
    std::cout << "All BFS operations demonstrated successfully!\n";
    std::cout << "==================================================\n";

    return 0;
}
