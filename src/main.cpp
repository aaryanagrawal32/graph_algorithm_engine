#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/BellmanFord.hpp"

int main() {
    std::cout << "==================================================\n";
    std::cout << "  Graph Engine - A* & Bellman-Ford Demonstration  \n";
    std::cout << "==================================================\n\n";

    // ============================================================================
    // 1. A* vs Dijkstra grid comparison
    // ============================================================================
    std::cout << "[1] Benchmarking A* vs Dijkstra on a 5x5 Grid with a Wall Obstacle:\n";
    
    // We create a grid graph with coordinates as std::pair<int, int>
    Graph<std::pair<int, int>, double> grid(false); // Undirected

    // Obstacle Wall: (2, 0), (2, 1), (2, 2), (2, 3).
    // Node (2, 4) is open as a passage.
    auto isWall = [](int x, int y) {
        return (x == 2 && y >= 0 && y <= 3);
    };

    // Add nodes
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            if (!isWall(x, y)) {
                grid.addNode({x, y});
            }
        }
    }

    // Add edges to adjacent neighbors (up, down, left, right)
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            if (isWall(x, y)) continue;

            std::pair<int, int> curr = {x, y};

            // Connect Right neighbor
            if (x + 1 < 5 && !isWall(x + 1, y)) {
                grid.addEdge(curr, {x + 1, y}, 1.0);
            }
            // Connect Down neighbor
            if (y + 1 < 5 && !isWall(x, y + 1)) {
                grid.addEdge(curr, {x, y + 1}, 1.0);
            }
        }
    }

    std::pair<int, int> start = {0, 0};
    std::pair<int, int> goal = {4, 4};

    // Run Dijkstra with Stats
    auto dijkstraResult = dijkstraWithStats(grid, start, goal);
    
    // Run A* with Manhattan Heuristic
    auto astarResult = astar(grid, start, goal, manhattanHeuristic);

    // Print Dijkstra results
    std::cout << "  Dijkstra Path (from (0,0) to (4,4)):\n";
    std::cout << "    Path: ";
    for (size_t i = 0; i < dijkstraResult.path.size(); ++i) {
        std::cout << "(" << dijkstraResult.path[i].first << "," << dijkstraResult.path[i].second << ")"
                  << (i + 1 < dijkstraResult.path.size() ? " -> " : "");
    }
    std::cout << "\n    Nodes Visited: " << dijkstraResult.nodesVisited << "\n";
    std::cout << "    Path Cost: " << dijkstraResult.path.size() - 1 << " units\n\n";

    // Print A* results
    std::cout << "  A* Path (using Manhattan Heuristic):\n";
    std::cout << "    Path: ";
    for (size_t i = 0; i < astarResult.path.size(); ++i) {
        std::cout << "(" << astarResult.path[i].first << "," << astarResult.path[i].second << ")"
                  << (i + 1 < astarResult.path.size() ? " -> " : "");
    }
    std::cout << "\n    Nodes Visited: " << astarResult.nodesVisited << "\n";
    std::cout << "    Path Cost: " << astarResult.path.size() - 1 << " units\n\n";

    std::cout << "  *Comparison: Both find the optimal cost path around the wall, but\n";
    std::cout << "   A* visited fewer nodes (" << astarResult.nodesVisited << ") compared to Dijkstra (" 
              << dijkstraResult.nodesVisited << ") because A* uses the heuristic to focus\n";
    std::cout << "   exploration directly toward the goal (4, 4).\n\n";

    // ============================================================================
    // 2. Bellman-Ford demo on sample graph (containing negative edge weight)
    // ============================================================================
    std::cout << "[2] Running Bellman-Ford on the Sample Graph (containing a negative weight edge):\n";
    
    Graph<int, int> g(true); // Directed
    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(2, 0, 1);
    g.addEdge(2, 3, 3);
    g.addEdge(3, 4, 2);
    g.addEdge(4, 5, 2);
    g.addEdge(5, 3, 2);
    g.addEdge(4, 6, 1);
    g.addEdge(6, 7, 4);
    g.addEdge(7, 8, -2); // Negative edge
    g.addEdge(8, 9, 3);
    g.addEdge(9, 10, 2);
    g.addEdge(10, 11, 1);
    g.addEdge(7, 9, 5);
    g.addEdge(10, 6, 2);

    try {
        auto bfDist = bellmanFord(g, 0);
        std::cout << "  Shortest distances from node 0 using Bellman-Ford:\n";
        std::cout << "  " << std::left << std::setw(10) << "Node" << "Distance\n";
        std::cout << "  " << std::left << std::setw(10) << "----" << "--------\n";
        for (int i = 0; i < 12; ++i) {
            std::cout << "  " << std::left << std::setw(10) << i << bfDist[i] << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "  Error: " << e.what() << "\n";
    }
    std::cout << "\n";

    // ============================================================================
    // 3. Bellman-Ford negative cycle detection
    // ============================================================================
    std::cout << "[3] Testing Bellman-Ford Negative Cycle Detection:\n";
    
    Graph<int, int> cycleGraph(true);
    cycleGraph.addEdge(0, 1, 2);
    cycleGraph.addEdge(1, 2, -5); // Creates a negative loop: 0 -> 1 -> 2 -> 0 of cost 2 + (-5) + 1 = -2
    cycleGraph.addEdge(2, 0, 1);

    std::cout << "  Graph contains negative cycle: 0 -(2)-> 1 -(-5)-> 2 -(1)-> 0\n";
    try {
        std::cout << "  Running Bellman-Ford from node 0...\n";
        bellmanFord(cycleGraph, 0);
        std::cout << "  Bellman-Ford succeeded (Unexpected: Negative cycle should have been caught!)\n";
    } catch (const std::runtime_error& e) {
        std::cout << "  Caught expected exception: \"" << e.what() << "\" (Success! Cycle detected)\n";
    }

    std::cout << "\n==================================================\n";
    std::cout << "All A* and Bellman-Ford operations demonstrated successfully!\n";
    std::cout << "==================================================\n";

    return 0;
}
