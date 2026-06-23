/**
 * @file Kruskal.hpp
 * @brief Kruskal's Minimum Spanning Tree (MST) algorithm.
 *
 * Finds the minimum spanning tree of a weighted undirected graph
 * using the greedy approach combined with Disjoint Set Union (DSU).
 *
 * Time Complexity: O(E log E) or O(E log V)
 * Space Complexity: O(V + E)
 */

#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <stdexcept>
#include "Graph.hpp"

/**
 * @struct DSU
 * @brief Disjoint Set Union (Union-Find) data structure.
 * Supports path compression and union by rank.
 */
template<typename NodeType, typename Hash = GraphUtils::SafeHash>
struct DSU {
    std::unordered_map<NodeType, NodeType, Hash> parent;
    std::unordered_map<NodeType, int, Hash> rank;

    /**
     * @brief Creates a new set containing only element x.
     * Time Complexity: O(1) average.
     */
    void makeSet(NodeType x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
            rank[x] = 0;
        }
    }

    /**
     * @brief Finds the representative (root) of the set containing x.
     * Applies path compression to flatten the set tree.
     * Time Complexity: Amortized O(alpha(N)) ~ O(1) where alpha is the inverse Ackermann.
     *
     * WHY Path Compression:
     * By redirecting every traversed node's parent pointer directly to the root during
     * a find operation, subsequent find calls for those nodes take O(1) time. This prevents
     * the tree from degenerating into a tall chain.
     */
    NodeType find(NodeType x) {
        if (parent[x] == x) {
            return x;
        }
        // Path compression step
        return parent[x] = find(parent[x]);
    }

    /**
     * @brief Unites the sets containing x and y.
     * Applies Union by Rank to attach the shorter tree under the taller tree.
     * @return true if united successfully, false if they were already in the same set.
     * Time Complexity: Amortized O(alpha(N)) ~ O(1).
     *
     * WHY Union by Rank:
     * Always attaching the root of the smaller rank tree under the root of the larger rank
     * tree ensures that the depth of the resulting tree remains small (bounded logarithmically).
     * Combining both path compression and union by rank guarantees near O(1) time per operation.
     */
    bool unite(NodeType x, NodeType y) {
        NodeType rootX = find(x);
        NodeType rootY = find(y);

        if (rootX == rootY) {
            return false; // Cycle detected/Already in same component
        }

        // Union by rank
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        return true;
    }
};

/**
 * @brief Computes the Minimum Spanning Tree (MST) of a weighted undirected graph using Kruskal's algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @return std::vector<std::tuple<WeightType, NodeType, NodeType>> Edges in the MST as {weight, u, v}.
 *
 * Time Complexity Derivation: O(E log E)
 * - Collecting all edges takes O(E).
 * - Sorting edges by weight takes O(E log E) time.
 * - Processing edges and running DSU find/unite takes O(E * alpha(V)) ~ O(E) time.
 * - Total time complexity is O(E log E). Since E <= V^2, log E <= 2 log V, which is also O(E log V).
 *
 * WHY sorting edges is the greedy choice (Cut Property):
 * For any cut of the graph (partitioning vertices into two sets S and V - S), the minimum weight edge
 * that crosses the cut must belong to some MST of the graph. By sorting edges globally and processing
 * them in ascending order of weight, Kruskal's algorithm always greedily selects the cheapest edge
 * crossing the current components cut, guaranteeing a minimum total weight.
 *
 * WHY DSU detects cycles efficiently:
 * If an edge connects u and v, and `find(u) == find(v)`, they are already connected by some path
 * in the current forest. Adding this edge would create a cycle. DSU lets us check this in O(1) time.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<std::tuple<WeightType, NodeType, NodeType>> kruskal(const Graph<NodeType, WeightType, Hash>& g) {
    std::vector<std::tuple<WeightType, NodeType, NodeType>> mstEdges;

    if (g.isDirected()) {
        throw std::invalid_argument("Kruskal's algorithm requires an undirected graph.");
    }

    // Collect all edges from the adjacency list
    std::vector<std::tuple<WeightType, NodeType, NodeType>> allEdges;
    for (const auto& u : g.getAllNodes()) {
        for (const auto& edge : g.neighbors(u)) {
            // In an undirected graph, u->v and v->u both exist. We collect all,
            // and the DSU cycle check will naturally skip the reverse duplicate edge.
            allEdges.push_back({edge.weight, u, edge.to});
        }
    }

    // Sort edges by weight
    std::sort(allEdges.begin(), allEdges.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    // Initialize DSU
    DSU<NodeType, Hash> dsu;
    for (const auto& node : g) {
        dsu.makeSet(node);
    }

    // Process sorted edges
    int edgesAdded = 0;
    int targetEdges = g.nodeCount() - 1; // An MST has exactly V - 1 edges

    for (const auto& edge : allEdges) {
        if (edgesAdded >= targetEdges) {
            break; // Found all edges of the MST
        }

        auto [w, u, v] = edge;
        if (dsu.unite(u, v)) {
            mstEdges.push_back(edge);
            edgesAdded++;
        }
    }

    return mstEdges;
}
