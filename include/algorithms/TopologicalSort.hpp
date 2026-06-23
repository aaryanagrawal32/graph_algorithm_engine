/**
 * @file TopologicalSort.hpp
 * @brief Topological Sort algorithms for Directed Acyclic Graphs (DAGs).
 *
 * Implements Kahn's BFS-based topological sort and DFS-based topological sort,
 * both including cycle detection mechanisms.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */

#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include "Graph.hpp"

namespace TopologicalSortInternal {
    /**
     * @brief DFS helper to build topological sorting and check for cycle presence using 3-state coloring.
     * State values: 0 = UNVISITED, 1 = VISITING, 2 = VISITED.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    bool topoDFSHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr,
                       std::unordered_map<NodeType, int, Hash>& state,
                       std::vector<NodeType>& result) {
        state[curr] = 1; // Mark as VISITING (active in stack)

        for (const auto& edge : g.neighbors(curr)) {
            if (state[edge.to] == 1) {
                return true; // Cycle detected: back-edge found
            } else if (state[edge.to] == 0) {
                if (topoDFSHelper(g, edge.to, state, result)) {
                    return true;
                }
            }
        }

        state[curr] = 2; // Mark as VISITED (backtracked)
        result.push_back(curr); // Add node when all its descendants are fully processed
        return false;
    }
}

/**
 * @brief Performs topological sorting using Kahn's BFS-based algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @return std::vector<NodeType> Topologically sorted list of nodes.
 * @throws std::runtime_error If the graph is not a DAG (contains a cycle).
 *
 * WHY In-Degree:
 * The in-degree of a node is the number of incoming edges targeting it. A node with an in-degree
 * of 0 has no active dependencies and can be safely placed first. Kahn's algorithm works by
 * greedily extracting 0-in-degree nodes and removing their outgoing edges, updating neighbors.
 *
 * WHY cycle check works in Kahn's:
 * If the graph has a cycle, the nodes involved in the cycle will never have their in-degrees
 * reduced to 0 (since each node in the loop depends on another). Consequently, they will never
 * be pushed onto the queue. If the final sorted list size is less than the total node count V,
 * it indicates a cycle exists.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> topologicalSort(const Graph<NodeType, WeightType, Hash>& g) {
    if (!g.isDirected()) {
        throw std::invalid_argument("Topological sort requires a directed graph.");
    }

    std::vector<NodeType> result;
    std::unordered_map<NodeType, int, Hash> inDegree;

    // Initialize all in-degrees to 0
    for (const auto& node : g) {
        inDegree[node] = 0;
    }

    // Compute in-degrees
    for (const auto& u : g) {
        for (const auto& edge : g.neighbors(u)) {
            inDegree[edge.to]++;
        }
    }

    // Queue nodes with 0 dependencies
    std::queue<NodeType> q;
    for (const auto& node : g) {
        if (inDegree[node] == 0) {
            q.push(node);
        }
    }

    while (!q.empty()) {
        NodeType u = q.front();
        q.pop();
        result.push_back(u);

        // Remove edge u->v
        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            inDegree[v]--;
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }

    // Cycle detection check
    if (static_cast<int>(result.size()) != g.nodeCount()) {
        throw std::runtime_error("Graph contains a cycle (not a DAG)");
    }

    return result;
}

/**
 * @brief Performs topological sorting using recursive DFS.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @return std::vector<NodeType> Topologically sorted list of nodes.
 * @throws std::runtime_error If the graph contains a cycle.
 *
 * HOW cycle detection works in DFS:
 * Uses 3-state coloring. Nodes currently undergoing search are marked VISITING. If DFS traverses
 * an edge pointing to a node marked VISITING, it represents a back-edge (looping back to a parent
 * still active in the call stack), indicating a cycle.
 *
 * DIFFERENCE between Kahn's and DFS-based output:
 * Both produce valid topological orderings, but Kahn's BFS-based traversal processes nodes level-by-level
 * (placing nodes with fewer dependencies as early as possible). DFS explores paths fully to the bottom
 * first. If there are multiple valid topological sorts, Kahn's and DFS may return different valid permutations.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> topologicalSortDFS(const Graph<NodeType, WeightType, Hash>& g) {
    if (!g.isDirected()) {
        throw std::invalid_argument("Topological sort requires a directed graph.");
    }

    std::vector<NodeType> result;
    std::unordered_map<NodeType, int, Hash> state; // 0 = unvisited, 1 = visiting, 2 = visited

    for (const auto& node : g) {
        state[node] = 0;
    }

    for (const auto& node : g.getAllNodes()) {
        if (state[node] == 0) {
            if (TopologicalSortInternal::topoDFSHelper(g, node, state, result)) {
                throw std::runtime_error("Graph contains a cycle (not a DAG)");
            }
        }
    }

    // The DFS helper pushes nodes to the result list when backtracking (bottom-up).
    // Reversing the list gives the correct top-down dependency order.
    std::reverse(result.begin(), result.end());
    return result;
}
