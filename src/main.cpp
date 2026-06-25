#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cmath>

#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/BellmanFord.hpp"
#include "algorithms/Kruskal.hpp"
#include "algorithms/TarjanSCC.hpp"
#include "algorithms/TopologicalSort.hpp"
#include "utils/GraphLoader.hpp"
#include "utils/GraphExporter.hpp"
#include "utils/Timer.hpp"

int main() {
    std::cout << "======================================================================\n";
    std::cout << "               GRAPH ALGORITHM ENGINE - DEMONSTRATION SUITE           \n";
    std::cout << "======================================================================\n\n";

    // Create visualizer directory if it doesn't exist
    try {
        std::filesystem::create_directories("visualizer");
    } catch (const std::exception& e) {
        std::cerr << "[Warning] Failed to create visualizer directory: " << e.what() << "\n";
    }

    // ============================================================================
    // 1. Loading Graphs
    // ============================================================================
    std::cout << "[1] Loading Graph from CSV edge list...\n";
    Graph<int, int> directedGraph(true);      // Directed
    Graph<int, int> undirectedGraph(false);   // Undirected for Kruskal

    try {
        loadFromCSV("data/sample_graph.csv", directedGraph);
        loadFromCSV("data/sample_graph.csv", undirectedGraph);
        std::cout << "  Successfully loaded directed graph: " << directedGraph.nodeCount() 
                  << " nodes, " << directedGraph.edgeCount() << " edges.\n";
        std::cout << "  Successfully loaded undirected graph: " << undirectedGraph.nodeCount() 
                  << " nodes, " << undirectedGraph.edgeCount() << " edges.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "  [Error] Failed to load graph: " << e.what() << "\n";
        return 1;
    }

    // ============================================================================
    // 2. Define Node Coordinates for Visualizer & A* Heuristic
    // ============================================================================
    // Lays out nodes 0-11 in a readable flow from left to right
    std::unordered_map<int, std::pair<double, double>> coordMap;
    coordMap[0] = {50.0, 300.0};
    coordMap[1] = {150.0, 180.0};
    coordMap[2] = {250.0, 300.0};
    coordMap[3] = {400.0, 300.0};
    coordMap[4] = {500.0, 180.0};
    coordMap[5] = {600.0, 300.0};
    coordMap[6] = {750.0, 300.0};
    coordMap[7] = {900.0, 300.0};
    coordMap[8] = {1000.0, 180.0};
    coordMap[9] = {1100.0, 420.0};
    coordMap[10] = {1250.0, 300.0};
    coordMap[11] = {1400.0, 300.0};

    // Export graph.json for the visualizer
    exportToJSON(directedGraph, "visualizer/graph.json", coordMap);

    // ============================================================================
    // 3. Breadth-First Search (BFS)
    // ============================================================================
    std::cout << "[2] Running Breadth-First Search (BFS) Traversal...\n";
    {
        Timer t("BFS");
        auto bfsOrder = bfs(directedGraph, 0);
        double elapsed = t.stop();

        std::cout << "  BFS Visit Order: ";
        for (size_t i = 0; i < bfsOrder.size(); ++i) {
            std::cout << bfsOrder[i] << (i + 1 < bfsOrder.size() ? " -> " : "");
        }
        std::cout << "\n  Execution Time: " << elapsed << " ms\n";
        exportAlgorithmSteps(bfsOrder, "visualizer/bfs_steps.json");
        std::cout << "  Exported BFS steps to 'visualizer/bfs_steps.json'.\n\n";
    }

    // ============================================================================
    // 4. Depth-First Search (DFS)
    // ============================================================================
    std::cout << "[3] Running Depth-First Search (DFS) Traversal...\n";
    {
        Timer t("DFS");
        auto dfsOrder = dfs(directedGraph, 0);
        double elapsed = t.stop();

        std::cout << "  DFS Visit Order: ";
        for (size_t i = 0; i < dfsOrder.size(); ++i) {
            std::cout << dfsOrder[i] << (i + 1 < dfsOrder.size() ? " -> " : "");
        }
        std::cout << "\n  Execution Time: " << elapsed << " ms\n";
        exportAlgorithmSteps(dfsOrder, "visualizer/dfs_steps.json");
        std::cout << "  Exported DFS steps to 'visualizer/dfs_steps.json'.\n\n";
    }

    // ============================================================================
    // 5. Dijkstra's Shortest Path
    // ============================================================================
    std::cout << "[4] Running Dijkstra's Shortest Path (Source: 0, Destination: 11)...\n";
    {
        Timer t("Dijkstra");
        auto dijRes = dijkstra(directedGraph, 0);
        auto path = reconstructPath(dijRes, 0, 11);
        double elapsed = t.stop();

        std::cout << "  Dijkstra Shortest Path: ";
        if (path.empty()) {
            std::cout << "Unreachable";
        } else {
            for (size_t i = 0; i < path.size(); ++i) {
                std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
            }
            std::cout << "\n  Path Cost: " << dijRes.dist[11];
        }
        std::cout << "\n  Execution Time: " << elapsed << " ms\n";
        exportAlgorithmSteps(path, "visualizer/dijkstra_path.json");
        std::cout << "  Exported Dijkstra path to 'visualizer/dijkstra_path.json'.\n\n";
    }

    // ============================================================================
    // 6. A* shortest path Search
    // ============================================================================
    std::cout << "[5] Running A* shortest path Search (Source: 0, Destination: 11)...\n";
    {
        // Custom Euclidean distance heuristic based on the visualizer coordinates
        auto customHeuristic = [&](int u, int v) {
            auto itU = coordMap.find(u);
            auto itV = coordMap.find(v);
            if (itU != coordMap.end() && itV != coordMap.end()) {
                double dx = itU->second.first - itV->second.first;
                double dy = itU->second.second - itV->second.second;
                return std::sqrt(dx * dx + dy * dy) * 0.01; // Scaled heuristic
            }
            return 0.0;
        };

        Timer t("AStar");
        auto astarRes = astar(directedGraph, 0, 11, customHeuristic);
        double elapsed = t.stop();

        std::cout << "  A* Shortest Path: ";
        if (astarRes.path.empty()) {
            std::cout << "Unreachable";
        } else {
            for (size_t i = 0; i < astarRes.path.size(); ++i) {
                std::cout << astarRes.path[i] << (i + 1 < astarRes.path.size() ? " -> " : "");
            }
            std::cout << "\n  Nodes Visited (Closed): " << astarRes.nodesVisited;
        }
        std::cout << "\n  Execution Time: " << elapsed << " ms\n";
        exportAlgorithmSteps(astarRes.path, "visualizer/astar_path.json");
        std::cout << "  Exported A* path to 'visualizer/astar_path.json'.\n\n";
    }

    // ============================================================================
    // 7. Bellman-Ford (Handles Negative Weights)
    // ============================================================================
    std::cout << "[6] Running Bellman-Ford Shortest Path (Source: 0)...\n";
    {
        Timer t("BellmanFord");
        try {
            auto dist = bellmanFord(directedGraph, 0);
            double elapsed = t.stop();

            std::cout << "  Shortest distances from Node 0:\n";
            std::cout << "    Node: ";
            for (int i = 0; i < 12; ++i) std::cout << std::setw(4) << i;
            std::cout << "\n    Cost: ";
            for (int i = 0; i < 12; ++i) std::cout << std::setw(4) << dist[i];
            std::cout << "\n  Execution Time: " << elapsed << " ms\n\n";
        } catch (const std::exception& e) {
            std::cerr << "  [Error] Bellman-Ford threw exception: " << e.what() << "\n\n";
        }
    }

    // ============================================================================
    // 8. Kruskal's Minimum Spanning Tree (Undirected)
    // ============================================================================
    std::cout << "[7] Running Kruskal's Minimum Spanning Tree...\n";
    {
        Timer t("Kruskal");
        auto mst = kruskal(undirectedGraph);
        double elapsed = t.stop();

        int totalWeight = 0;
        std::cout << "  Selected MST edges:\n";
        for (const auto& edge : mst) {
            auto [wt, u, v] = edge;
            std::cout << "    " << u << " -- " << v << " (weight: " << wt << ")\n";
            totalWeight += wt;
        }
        std::cout << "  Total MST Weight: " << totalWeight << "\n";
        std::cout << "  Execution Time: " << elapsed << " ms\n";

        // Export Kruskal MST to JSON
        std::ofstream mstFile("visualizer/kruskal_mst.json");
        if (mstFile.is_open()) {
            mstFile << "{\n  \"mst\": [\n";
            for (size_t i = 0; i < mst.size(); ++i) {
                auto [w, u, v] = mst[i];
                mstFile << "    { \"source\": \"" << u << "\", \"target\": \"" << v << "\", \"weight\": " << w << " }";
                if (i + 1 < mst.size()) mstFile << ",";
                mstFile << "\n";
            }
            mstFile << "  ]\n}\n";
            mstFile.close();
            std::cout << "  Exported Kruskal MST to 'visualizer/kruskal_mst.json'.\n\n";
        }
    }

    // ============================================================================
    // 9. Tarjan's Strongly Connected Components (SCC)
    // ============================================================================
    std::cout << "[8] Running Tarjan's Strongly Connected Components (SCC)...\n";
    {
        TarjanSCC<int, int> tarjan;
        Timer t("Tarjan");
        auto sccs = tarjan.findSCC(directedGraph);
        double elapsed = t.stop();

        std::cout << "  Found Strongly Connected Components:\n";
        for (size_t i = 0; i < sccs.size(); ++i) {
            std::cout << "    Component " << i + 1 << ": { ";
            for (const auto& node : sccs[i]) {
                std::cout << node << " ";
            }
            std::cout << "}\n";
        }
        std::cout << "  Execution Time: " << elapsed << " ms\n";

        // Export Tarjan SCCs to JSON
        std::ofstream sccFile("visualizer/tarjan_sccs.json");
        if (sccFile.is_open()) {
            sccFile << "{\n  \"sccs\": [\n";
            for (size_t i = 0; i < sccs.size(); ++i) {
                sccFile << "    [";
                for (size_t j = 0; j < sccs[i].size(); ++j) {
                    sccFile << "\"" << sccs[i][j] << "\"";
                    if (j + 1 < sccs[i].size()) sccFile << ", ";
                }
                sccFile << "]";
                if (i + 1 < sccs.size()) sccFile << ",";
                sccFile << "\n";
            }
            sccFile << "  ]\n}\n";
            sccFile.close();
            std::cout << "  Exported Tarjan SCCs to 'visualizer/tarjan_sccs.json'.\n\n";
        }
    }

    // ============================================================================
    // 10. Topological Sort (DAG dependency & Cycle Detection)
    // ============================================================================
    std::cout << "[9] Running Topological Sort Demonstrations...\n";
    {
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
            std::cerr << "    [Error] " << e.what() << "\n";
        }

        std::cout << "  Testing Topological Sort on a Cyclic Graph (A -> B -> C -> A)...\n";
        Graph<std::string, int> cyclicDag(true);
        cyclicDag.addEdge("A", "B", 1);
        cyclicDag.addEdge("B", "C", 1);
        cyclicDag.addEdge("C", "A", 1);

        try {
            topologicalSort(cyclicDag);
            std::cout << "    [Warning] Topological sort succeeded on cyclic graph (Unexpected!)\n";
        } catch (const std::runtime_error& e) {
            std::cout << "    Caught expected exception: \"" << e.what() << "\" (Success! Cycle detected)\n";
        }
    }

    std::cout << "\n======================================================================\n";
    std::cout << "                  ALL DEMONSTRATIONS RUN SUCCESSFULLY                 \n";
    std::cout << "======================================================================\n";

    return 0;
}
