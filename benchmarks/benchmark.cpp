#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <random>
#include <iomanip>
#include <cmath>
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/Kruskal.hpp"
#include "utils/Timer.hpp"

// ============================================================================
// Custom Traversals to track Peak Queue/Stack sizes
// ============================================================================
template<typename NodeType, typename WeightType, typename Hash = GraphUtils::SafeHash>
std::pair<double, size_t> benchmarkBFS(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    Timer t("BFS");
    std::unordered_set<NodeType, Hash> visited;
    std::queue<NodeType> q;
    size_t maxQueueSize = 0;

    if (!g.hasNode(start)) {
        double elapsed = t.stop();
        return {elapsed, 0};
    }

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        maxQueueSize = std::max(maxQueueSize, q.size());
        NodeType curr = q.front();
        q.pop();

        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                q.push(edge.to);
            }
        }
    }
    double elapsed = t.stop();
    return {elapsed, maxQueueSize};
}

template<typename NodeType, typename WeightType, typename Hash = GraphUtils::SafeHash>
std::pair<double, size_t> benchmarkDFS(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    Timer t("DFS");
    std::unordered_set<NodeType, Hash> visited;
    std::stack<NodeType> s;
    size_t maxStackSize = 0;

    if (!g.hasNode(start)) {
        double elapsed = t.stop();
        return {elapsed, 0};
    }

    s.push(start);

    while (!s.empty()) {
        maxStackSize = std::max(maxStackSize, s.size());
        NodeType curr = s.top();
        s.pop();

        if (visited.find(curr) == visited.end()) {
            visited.insert(curr);
            const auto& neighbors = g.neighbors(curr);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (visited.find(it->to) == visited.end()) {
                    s.push(it->to);
                }
            }
        }
    }
    double elapsed = t.stop();
    return {elapsed, maxStackSize};
}

// ============================================================================
// Random Graph Generators
// ============================================================================

// Generates a random sparse graph (connected via a line, then random edges)
Graph<int, int> generateRandomSparse(int numNodes, int edgeFactor) {
    Graph<int, int> g(true); // directed
    for (int i = 0; i < numNodes; ++i) {
        g.addNode(i);
    }
    // Ensure connectivity: line path
    for (int i = 0; i < numNodes - 1; ++i) {
        g.addEdge(i, i + 1, 1);
    }

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> nodeDist(0, numNodes - 1);
    
    int extraEdges = numNodes * edgeFactor;
    for (int i = 0; i < extraEdges; ++i) {
        int u = nodeDist(rng);
        int v = nodeDist(rng);
        if (u != v) {
            g.addEdge(u, v, 1);
        }
    }
    return g;
}

// Generates a grid graph of dimensions W x H with random weights
Graph<std::pair<int, int>, double> generateGridGraph(int W, int H) {
    Graph<std::pair<int, int>, double> g(true); // directed
    std::mt19937 rng(1337);
    std::uniform_real_distribution<double> weightDist(1.0, 10.0);

    for (int x = 0; x < W; ++x) {
        for (int y = 0; y < H; ++y) {
            g.addNode({x, y});
        }
    }

    auto addBidirectionalEdge = [&](std::pair<int, int> u, std::pair<int, int> v) {
        double w = weightDist(rng);
        g.addEdge(u, v, w);
        g.addEdge(v, u, w);
    };

    for (int x = 0; x < W; ++x) {
        for (int y = 0; y < H; ++y) {
            std::pair<int, int> curr = {x, y};
            if (x + 1 < W) {
                addBidirectionalEdge(curr, {x + 1, y});
            }
            if (y + 1 < H) {
                addBidirectionalEdge(curr, {x, y + 1});
            }
        }
    }
    return g;
}

// Generates a random undirected graph for MST benchmarks
Graph<int, int> generateMSTGraph(int numNodes, bool dense) {
    Graph<int, int> g(false); // undirected
    for (int i = 0; i < numNodes; ++i) {
        g.addNode(i);
    }

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> weightDist(1, 100);

    if (dense) {
        // Complete graph or semi-dense
        for (int i = 0; i < numNodes; ++i) {
            for (int j = i + 1; j < numNodes; ++j) {
                g.addEdge(i, j, weightDist(rng));
            }
        }
    } else {
        // Sparse graph: connect nodes in a cycle first, then add O(V) edges
        for (int i = 0; i < numNodes; ++i) {
            g.addEdge(i, (i + 1) % numNodes, weightDist(rng));
        }
        std::uniform_int_distribution<int> nodeDist(0, numNodes - 1);
        int extraEdges = numNodes;
        for (int i = 0; i < extraEdges; ++i) {
            int u = nodeDist(rng);
            int v = nodeDist(rng);
            if (u != v) {
                g.addEdge(u, v, weightDist(rng));
            }
        }
    }
    return g;
}

// ============================================================================
// Benchmarking Routines
// ============================================================================
void runBFSvsDFS() {
    std::cout << "\n============================================================\n";
    std::cout << "             BENCHMARK 1: BFS vs DFS Traversal\n";
    std::cout << "============================================================\n";
    std::cout << std::left << std::setw(8) << "Nodes"
              << " | " << std::setw(10) << "BFS(ms)"
              << " | " << std::setw(10) << "BFS Queue"
              << " | " << std::setw(10) << "DFS(ms)"
              << " | " << std::setw(10) << "DFS Stack" << "\n";
    std::cout << "------------------------------------------------------------\n";

    std::vector<int> sizes = {100, 1000, 10000, 100000};
    for (int N : sizes) {
        auto g = generateRandomSparse(N, 2);
        auto bfsRes = benchmarkBFS(g, 0);
        auto dfsRes = benchmarkDFS(g, 0);

        std::cout << std::left << std::setw(8) << N << " | "
                  << std::fixed << std::setprecision(3)
                  << std::setw(10) << bfsRes.first << " | "
                  << std::setw(10) << bfsRes.second << " | "
                  << std::setw(10) << dfsRes.first << " | "
                  << std::setw(10) << dfsRes.second << "\n";
    }
    std::cout << "============================================================\n";
}

void runDijkstraVsAStar() {
    std::cout << "\n============================================================\n";
    std::cout << "         BENCHMARK 2: Dijkstra vs A* on 2D Grid Graph\n";
    std::cout << "============================================================\n";
    std::cout << std::left << std::setw(10) << "Grid Size"
              << " | " << std::setw(10) << "Dij(ms)"
              << " | " << std::setw(12) << "Dij Visited"
              << " | " << std::setw(10) << "A*(ms)"
              << " | " << std::setw(12) << "A* Visited" << "\n";
    std::cout << "------------------------------------------------------------\n";

    // 10x10 (100 nodes), 32x32 (~1000 nodes), 100x100 (10000 nodes)
    std::vector<std::pair<int, int>> dimensions = {{10, 10}, {32, 32}, {100, 100}};
    for (auto [W, H] : dimensions) {
        auto g = generateGridGraph(W, H);
        std::pair<int, int> src = {0, 0};
        std::pair<int, int> dst = {W - 1, H - 1};

        // Dijkstra
        Timer t1("Dijkstra");
        auto dijRes = dijkstraWithStats(g, src, dst);
        double dijTime = t1.stop();

        // A*
        Timer t2("AStar");
        auto astarRes = astar(g, src, dst, manhattanHeuristic);
        double astarTime = t2.stop();

        std::string gridStr = std::to_string(W) + "x" + std::to_string(H);
        std::cout << std::left << std::setw(10) << gridStr << " | "
                  << std::fixed << std::setprecision(3)
                  << std::setw(10) << dijTime << " | "
                  << std::setw(12) << dijRes.nodesVisited << " | "
                  << std::setw(10) << astarTime << " | "
                  << std::setw(12) << astarRes.nodesVisited << "\n";
    }
    std::cout << "============================================================\n";
}

void runKruskalMST() {
    std::cout << "\n============================================================\n";
    std::cout << "         BENCHMARK 3: Kruskal's MST (Sparse vs Dense)\n";
    std::cout << "============================================================\n";
    std::cout << std::left << std::setw(10) << "Nodes"
              << " | " << std::setw(10) << "Density"
              << " | " << std::setw(10) << "Edges"
              << " | " << std::setw(15) << "Kruskal(ms)" << "\n";
    std::cout << "------------------------------------------------------------\n";

    // Sparse benchmarks
    std::vector<int> sparseSizes = {100, 1000, 5000, 10000};
    for (int N : sparseSizes) {
        auto g = generateMSTGraph(N, false);
        Timer t("Kruskal");
        auto mst = kruskal(g);
        double timeMs = t.stop();

        std::cout << std::left << std::setw(10) << N << " | "
                  << std::setw(10) << "Sparse" << " | "
                  << std::setw(10) << g.edgeCount() << " | "
                  << std::fixed << std::setprecision(3)
                  << std::setw(15) << timeMs << "\n";
    }

    std::cout << "------------------------------------------------------------\n";

    // Dense benchmarks
    std::vector<int> denseSizes = {100, 500, 1000, 2000};
    for (int N : denseSizes) {
        auto g = generateMSTGraph(N, true);
        Timer t("Kruskal");
        auto mst = kruskal(g);
        double timeMs = t.stop();

        std::cout << std::left << std::setw(10) << N << " | "
                  << std::setw(10) << "Dense" << " | "
                  << std::setw(10) << g.edgeCount() << " | "
                  << std::fixed << std::setprecision(3)
                  << std::setw(15) << timeMs << "\n";
    }
    std::cout << "============================================================\n";
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "            GRAPH ENGINE PERFORMANCE BENCHMARKS\n";
    std::cout << "============================================================\n";

    runBFSvsDFS();
    runDijkstraVsAStar();
    runKruskalMST();

    std::cout << "\nBenchmarks completed successfully.\n";
    return 0;
}
