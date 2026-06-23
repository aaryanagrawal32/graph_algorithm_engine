#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"

int main() {
    std::cout << "==================================================\n";
    std::cout << "  Graph Algorithm Engine - Dijkstra Demonstration \n";
    std::cout << "==================================================\n\n";

    // 1. Constructing a positive-weighted road map graph
    std::cout << "[1] Constructing Travel Map (Undirected, positive weights)...\n";
    Graph<std::string, double> travelMap(false); // Undirected

    travelMap.addEdge("Delhi", "Mumbai", 1400.0);
    travelMap.addEdge("Delhi", "Kolkata", 1500.0);
    travelMap.addEdge("Mumbai", "Kolkata", 2000.0);
    travelMap.addEdge("Mumbai", "Bengaluru", 1000.0);
    travelMap.addEdge("Kolkata", "Bengaluru", 1900.0);
    travelMap.addEdge("Delhi", "Bengaluru", 2100.0);
    travelMap.addEdge("Delhi", "Hyderabad", 1200.0);
    travelMap.addEdge("Hyderabad", "Bengaluru", 500.0);

    std::cout << "Travel Map created successfully!\n";
    std::cout << "Cities count: " << travelMap.nodeCount() << "\n";
    std::cout << "Routes count: " << travelMap.edgeCount() << "\n\n";

    // 2. Run Dijkstra's shortest path
    std::cout << "[2] Running Dijkstra's Algorithm from 'Delhi'...\n";
    auto dijkstraRes = dijkstra(travelMap, std::string("Delhi"));

    std::cout << "  Shortest distances from Delhi:\n";
    std::cout << "  -----------------------------\n";
    std::cout << "  " << std::left << std::setw(15) << "City" << "Distance (km)\n";
    std::cout << "  " << std::left << std::setw(15) << "----" << "-------------\n";
    for (const auto& [city, dist] : dijkstraRes.dist) {
        std::cout << "  " << std::left << std::setw(15) << city << dist << "\n";
    }
    std::cout << "\n";

    // 3. Path reconstruction & Comparison with BFS
    std::cout << "[3] Comparing Shortest Paths from 'Delhi' to 'Bengaluru':\n";

    // Dijkstra Path (Weight-optimal)
    std::vector<std::string> dijkstraPath = reconstructPath(dijkstraRes, std::string("Delhi"), std::string("Bengaluru"));
    std::cout << "  Dijkstra Path (Weight-Optimal): ";
    for (size_t i = 0; i < dijkstraPath.size(); ++i) {
        std::cout << dijkstraPath[i] << (i + 1 < dijkstraPath.size() ? " -> " : "");
    }
    std::cout << "\n  Total Cost: " << dijkstraRes.dist["Bengaluru"] << " km\n\n";

    // BFS Path (Hop-Optimal)
    std::vector<std::string> bfsPath = bfsShortestPath(travelMap, std::string("Delhi"), std::string("Bengaluru"));
    std::cout << "  BFS Path (Hop-Optimal / Minimum Edges): ";
    for (size_t i = 0; i < bfsPath.size(); ++i) {
        std::cout << bfsPath[i] << (i + 1 < bfsPath.size() ? " -> " : "");
    }
    // Calculate cost of BFS path
    double bfsCost = 0.0;
    for (size_t i = 0; i + 1 < bfsPath.size(); ++i) {
        for (const auto& edge : travelMap.neighbors(bfsPath[i])) {
            if (edge.to == bfsPath[i+1]) {
                bfsCost += edge.weight;
                break;
            }
        }
    }
    std::cout << "\n  Total Cost: " << bfsCost << " km\n\n";

    std::cout << "  *Analysis: BFS finds the direct route (Delhi -> Bengaluru) containing 1 edge\n";
    std::cout << "   but costing 2100 km. Dijkstra finds the routing through Hyderabad\n";
    std::cout << "   containing 2 edges but costing only 1700 km, showing why Dijkstra is\n";
    std::cout << "   necessary for weighted networks.\n\n";

    std::cout << "==================================================\n";
    std::cout << "Dijkstra demonstrated successfully!\n";
    std::cout << "==================================================\n";

    return 0;
}
