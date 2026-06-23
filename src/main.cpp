#include <iostream>
#include <string>
#include <vector>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"

int main() {
    std::cout << "==================================================\n";
    std::cout << "    Graph Algorithm Engine - DFS Demonstration    \n";
    std::cout << "==================================================\n\n";

    // 1. Constructing the 12-node sample graph
    std::cout << "[1] Constructing Directed Sample Graph...\n";
    Graph<int, int> g(true); // directed graph
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

    std::cout << "Nodes count: " << g.nodeCount() << " (Expected: 12)\n";
    std::cout << "Edges count: " << g.edgeCount() << " (Expected: 15)\n\n";

    // 2. Comparing Recursive DFS and Iterative DFS
    std::cout << "[2] Comparing Recursive and Iterative DFS Traversal (starting from 0):\n";
    std::vector<int> recDFS = dfs(g, 0);
    std::vector<int> iterDFS = dfsIterative(g, 0);

    std::cout << "  Recursive DFS: ";
    for (size_t i = 0; i < recDFS.size(); ++i) {
        std::cout << recDFS[i] << (i + 1 < recDFS.size() ? " -> " : "");
    }
    std::cout << "\n";

    std::cout << "  Iterative DFS: ";
    for (size_t i = 0; i < iterDFS.size(); ++i) {
        std::cout << iterDFS[i] << (i + 1 < iterDFS.size() ? " -> " : "");
    }
    std::cout << "\n\n";
    std::cout << "  *Note: Traversal orders may differ because recursive DFS explores branches\n";
    std::cout << "  immediately and marks nodes visited on start, while iterative DFS pushes\n";
    std::cout << "  all neighbors onto the stack and marks them visited when popped.\n\n";

    // 3. Cycle Detection
    std::cout << "[3] Testing Cycle Detection (hasCycle):\n";
    std::cout << "  Sample Graph cycle status: " << (hasCycle(g) ? "CYCLE DETECTED" : "NO CYCLE") 
              << " (Expected: CYCLE DETECTED)\n";

    // Create an acyclic directed graph (Tree)
    Graph<int, int> tree(true);
    tree.addEdge(1, 2, 1);
    tree.addEdge(1, 3, 1);
    tree.addEdge(2, 4, 1);
    tree.addEdge(2, 5, 1);
    std::cout << "  Acyclic Tree cycle status: " << (hasCycle(tree) ? "CYCLE DETECTED" : "NO CYCLE")
              << " (Expected: NO CYCLE)\n\n";

    // 4. Connectivity Checks
    std::cout << "[4] Testing Connectivity Checks (isConnected):\n";
    std::cout << "  Sample Graph connectivity: " << (isConnected(g) ? "CONNECTED" : "DISCONNECTED")
              << " (Expected: CONNECTED)\n";

    // Create a disconnected graph
    Graph<int, int> disconnectedGraph(false); // undirected
    disconnectedGraph.addEdge(1, 2, 1);
    disconnectedGraph.addEdge(3, 4, 1); // Component {1, 2} and Component {3, 4} are separate
    std::cout << "  Undirected Forest connectivity: " << (isConnected(disconnectedGraph) ? "CONNECTED" : "DISCONNECTED")
              << " (Expected: DISCONNECTED)\n";

    // Add isolated node to sample graph g to make it disconnected
    g.addNode(100); // 100 is completely isolated
    std::cout << "  Sample Graph + Isolated Node connectivity: " << (isConnected(g) ? "CONNECTED" : "DISCONNECTED")
              << " (Expected: DISCONNECTED)\n\n";

    std::cout << "==================================================\n";
    std::cout << "All DFS operations demonstrated successfully!\n";
    std::cout << "==================================================\n";

    return 0;
}
