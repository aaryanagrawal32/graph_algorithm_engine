/**
 * @file TarjanSCC.hpp
 * @brief Tarjan's Strongly Connected Components (SCC) algorithm.
 *
 * Implements a class-based Tarjan's algorithm to partition a directed graph
 * into its strongly connected components.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */

#pragma once

#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include "Graph.hpp"

/**
 * @class TarjanSCC
 * @brief Computes strongly connected components in a directed graph.
 */
template<typename NodeType, typename WeightType = int, typename Hash = GraphUtils::SafeHash>
class TarjanSCC {
private:
    std::unordered_map<NodeType, int, Hash> disc;      // Discovery time of each node
    std::unordered_map<NodeType, int, Hash> low;       // Lowest discovery time node reachable from node
    std::unordered_set<NodeType, Hash> onStack;       // Quick check if node is in the active search stack
    std::stack<NodeType> s;                           // Active exploration stack
    int timer;                                         // Step ordering timer
    std::vector<std::vector<NodeType>> sccs;          // List of found SCC components

    /**
     * @brief Recursive DFS helper to discover strongly connected components.
     */
    void dfs(const Graph<NodeType, WeightType, Hash>& g, NodeType u) {
        disc[u] = low[u] = ++timer;
        s.push(u);
        onStack.insert(u);

        // Relax neighbors
        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;

            if (disc.find(v) == disc.end()) {
                // v is not visited yet; recurse
                dfs(g, v);
                low[u] = std::min(low[u], low[v]);
            } else if (onStack.find(v) != onStack.end()) {
                // v is already visited and on stack (back-edge)
                low[u] = std::min(low[u], disc[v]);
            }
        }

        // If u is a root node of an SCC, pop all members off the stack
        // WHY low[u] == disc[u]:
        // If low[u] == disc[u], it means there is no path from u or any of its descendants
        // to any ancestor of u. Thus, u is the "highest" node in its strongly connected component.
        if (low[u] == disc[u]) {
            std::vector<NodeType> scc;
            while (true) {
                NodeType v = s.top();
                s.pop();
                onStack.erase(v);
                scc.push_back(v);
                if (v == u) {
                    break;
                }
            }
            sccs.push_back(scc);
        }
    }

public:
    /**
     * @brief Identifies Strongly Connected Components in a directed graph.
     * @param g Reference to the Graph.
     * @return std::vector<std::vector<NodeType>> A vector containing all SCCs (each SCC is a vector of nodes).
     *
     * WHY disc[] vs low[]:
     * - disc[u] records the chronological step order in which node u was first discovered by DFS.
     * - low[u] tracks the smallest discovery time node that u (or its DFS subtree descendants)
     *   can reach, including active ancestors currently in the DFS stack.
     *
     * WHY O(V + E) Complexity:
     * - Every vertex is visited by DFS exactly once, and each edge is traversed once.
     * - Nodes are pushed onto and popped from the stack at most once.
     * - Lookups in unordered sets/maps take O(1) average time.
     * - Total execution time is bounded strictly by O(V + E).
     */
    std::vector<std::vector<NodeType>> findSCC(const Graph<NodeType, WeightType, Hash>& g) {
        if (!g.isDirected()) {
            throw std::invalid_argument("Tarjan's SCC algorithm requires a directed graph.");
        }

        // Reset state between runs
        disc.clear();
        low.clear();
        onStack.clear();
        while (!s.empty()) {
            s.pop();
        }
        sccs.clear();
        timer = 0;

        // Run DFS from all unvisited nodes
        for (const auto& node : g.getAllNodes()) {
            if (disc.find(node) == disc.end()) {
                dfs(g, node);
            }
        }

        return sccs;
    }
};
