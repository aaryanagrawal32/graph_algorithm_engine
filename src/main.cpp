#include <iostream>
#include <string>
#include <iomanip>
#include "Graph.hpp"

int main() {
    std::cout << "==================================================\n";
    std::cout << "    Graph Algorithm Engine - Session 02 Demo      \n";
    std::cout << "==================================================\n\n";

    // 1. Creating a directed graph with string keys and double weights
    std::cout << "[1] Creating Directed Graph (std::string, double weights)...\n";
    Graph<std::string, double> dirGraph(true);

    dirGraph.addEdge("A", "B", 4.5);
    dirGraph.addEdge("A", "C", 2.1);
    dirGraph.addEdge("B", "C", 1.0);
    dirGraph.addEdge("C", "D", 5.2);
    
    // Test duplicate addition handling (updating the weight)
    dirGraph.addEdge("A", "B", 4.2);

    std::cout << "Nodes count: " << dirGraph.nodeCount() << " (Expected: 4)\n";
    std::cout << "Edges count: " << dirGraph.edgeCount() << " (Expected: 4)\n";

    // Print all neighbors of "A"
    std::cout << "Neighbors of 'A':\n";
    try {
        for (const auto& edge : dirGraph.neighbors("A")) {
            std::cout << "  -> " << edge.to << " (weight: " << edge.weight << ")\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    std::cout << "\n";

    // 2. Creating an undirected graph with int keys and int weights
    std::cout << "[2] Creating Undirected Graph (int, int weights)...\n";
    Graph<int, int> undirGraph(false);

    undirGraph.addEdge(1, 2, 10);
    undirGraph.addEdge(1, 3, 20);
    undirGraph.addEdge(2, 3, 30);

    std::cout << "Nodes count: " << undirGraph.nodeCount() << " (Expected: 3)\n";
    std::cout << "Edges count: " << undirGraph.edgeCount() << " (Expected: 3)\n";

    // Print all neighbors of 1
    std::cout << "Neighbors of node 1:\n";
    try {
        for (const auto& edge : undirGraph.neighbors(1)) {
            std::cout << "  <-> " << edge.to << " (weight: " << edge.weight << ")\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    std::cout << "\n";

    // 3. Testing range-based for loops over nodes (keys)
    std::cout << "[3] Iterating over all nodes in the undirected graph using Range-based for loop:\n";
    for (const auto& node : undirGraph) {
        std::cout << "  Node ID: " << node << "\n";
    }

    std::cout << "\n==================================================\n";
    std::cout << "All Graph operations demonstrated successfully!\n";
    std::cout << "==================================================\n";
    return 0;
}
