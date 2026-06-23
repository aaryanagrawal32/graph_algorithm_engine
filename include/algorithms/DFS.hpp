/**
 * @file DFS.hpp
 * @brief Depth-First Search (DFS) algorithms.
 *
 * Implements recursive and iterative DFS traversals, cycle detection,
 * and connectivity checks for the generic Graph class template.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 *
 * WHY USE THIS: DFS is fundamental for pathfinding, topological sorting,
 * cycle detection, and connected components.
 */

#pragma once

#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "Graph.hpp"

namespace DFSInternal {
    /**
     * @brief Helper function for recursive DFS traversal.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    void dfsHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr,
                   std::unordered_set<NodeType, Hash>& visited,
                   std::vector<NodeType>& visitOrder) {
        visited.insert(curr);
        visitOrder.push_back(curr);

        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                // Recursion stack implicitly tracks traversal path and handles backtracking.
                dfsHelper(g, edge.to, visited, visitOrder);
            }
        }
    }

    /**
     * @brief Helper function to detect cycles in a directed graph using 3-state coloring.
     * State values: 0 = UNVISITED, 1 = VISITING (currently in recursion stack), 2 = VISITED.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    bool hasCycleDirectedHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr,
                                std::unordered_map<NodeType, int, Hash>& state) {
        state[curr] = 1; // Mark as VISITING

        for (const auto& edge : g.neighbors(curr)) {
            if (state[edge.to] == 1) {
                // Found a back-edge pointing to an ancestor in the active call stack
                return true;
            } else if (state[edge.to] == 0) {
                if (hasCycleDirectedHelper(g, edge.to, state)) {
                    return true;
                }
            }
        }

        state[curr] = 2; // Mark as VISITED (and backtrack)
        return false;
    }

    /**
     * @brief Helper function to detect cycles in an undirected graph using parent tracking.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    bool hasCycleUndirectedHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr, NodeType parent,
                                  std::unordered_set<NodeType, Hash>& visited) {
        visited.insert(curr);

        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                if (hasCycleUndirectedHelper(g, edge.to, curr, visited)) {
                    return true;
                }
            } else if (edge.to != parent) {
                // Reached an already visited node that is NOT the direct parent of this node.
                // This means there is another path to it (a cycle).
                return true;
            }
        }
        return false;
    }
}

/**
 * @brief Performs a recursive DFS traversal starting from a given node.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @param start The node to start the traversal from.
 * @return std::vector<NodeType> Order in which nodes were visited.
 *
 * Time Complexity: O(V + E) average.
 * Space Complexity: O(V) for the implicit recursion call stack and visited set.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> dfs(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    std::vector<NodeType> visitOrder;
    if (!g.hasNode(start)) {
        return visitOrder;
    }
    std::unordered_set<NodeType, Hash> visited;
    DFSInternal::dfsHelper(g, start, visited, visitOrder);
    return visitOrder;
}

/**
 * @brief Performs an iterative DFS traversal starting from a given node using an explicit stack.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @param start The node to start the traversal from.
 * @return std::vector<NodeType> Order in which nodes were visited.
 *
 * Time Complexity: O(V + E) average.
 * Space Complexity: O(V) for the explicit std::stack.
 *
 * WHY recursive and iterative DFS traversal orders can differ:
 * 1. Stack Push Order: In recursive DFS, we visit the first neighbor, explore its entire branch,
 *    and only visit the second neighbor during backtracking. In iterative DFS, we push all neighbors
 *    of the current node onto the stack first. If we push neighbors left-to-right, the rightmost
 *    neighbor is on top and gets explored first (reversing the branch order).
 * 2. Visited Mark Timing: Recursive DFS marks a node visited *immediately* when starting its call.
 *    Iterative DFS marks a node visited when it is *popped* from the stack. Because nodes can be pushed
 *    onto the stack multiple times before being popped and marked visited, the discovery sequence can shift.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> dfsIterative(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    std::vector<NodeType> visitOrder;
    if (!g.hasNode(start)) {
        return visitOrder;
    }

    std::unordered_set<NodeType, Hash> visited;
    std::stack<NodeType> s;

    s.push(start);

    while (!s.empty()) {
        NodeType curr = s.top();
        s.pop();

        if (visited.find(curr) == visited.end()) {
            visited.insert(curr);
            visitOrder.push_back(curr);

            // Traverse neighbors in reverse order (right-to-left) to attempt to match the recursive
            // left-to-right branch exploration order as closely as possible.
            const auto& neighbors = g.neighbors(curr);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (visited.find(it->to) == visited.end()) {
                    s.push(it->to);
                }
            }
        }
    }
    return visitOrder;
}

/**
 * @brief Detects if the graph contains any cycles.
 * Supports both directed and undirected graphs.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @return true if a cycle is detected, false otherwise.
 *
 * Time Complexity: O(V + E) average.
 * Space Complexity: O(V) for visited and recursion stack structures.
 *
 * WHY back-edges detect cycles:
 * A back-edge is an edge that points from a node to one of its ancestors in the DFS tree.
 * Finding a back-edge means we have reached a node currently in the recursion stack (which is
 * active and not fully completed), establishing a cycle back to an earlier ancestor.
 */
template<typename NodeType, typename WeightType, typename Hash>
bool hasCycle(const Graph<NodeType, WeightType, Hash>& g) {
    if (g.isDirected()) {
        std::unordered_map<NodeType, int, Hash> state; // 0 = unvisited, 1 = visiting, 2 = visited
        for (const auto& node : g.getAllNodes()) {
            if (state[node] == 0) {
                if (DFSInternal::hasCycleDirectedHelper(g, node, state)) {
                    return true;
                }
            }
        }
    } else {
        std::unordered_set<NodeType, Hash> visited;
        for (const auto& node : g.getAllNodes()) {
            if (visited.find(node) == visited.end()) {
                // For undirected cycle detection, we set the initial parent to itself
                if (DFSInternal::hasCycleUndirectedHelper(g, node, node, visited)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Checks if the graph is connected.
 * For directed graphs, it checks for weak connectivity (ignoring edge direction).
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @return true if the graph is connected, false otherwise.
 *
 * Time Complexity: O(V + E) average.
 * Space Complexity: O(V) for visited set and queue.
 */
template<typename NodeType, typename WeightType, typename Hash>
bool isConnected(const Graph<NodeType, WeightType, Hash>& g) {
    if (g.nodeCount() <= 1) {
        return true;
    }

    std::vector<NodeType> allNodes = g.getAllNodes();
    NodeType start = allNodes[0];

    std::unordered_set<NodeType, Hash> visited;
    std::queue<NodeType> q;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        NodeType curr = q.front();
        q.pop();

        // Standard outgoing neighbors
        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                q.push(edge.to);
            }
        }

        // If directed, we must check incoming edges as well to ensure weak connectivity
        if (g.isDirected()) {
            for (const auto& otherNode : allNodes) {
                if (visited.find(otherNode) == visited.end()) {
                    // Check if otherNode has an edge targeting curr
                    if (g.hasEdge(otherNode, curr)) {
                        visited.insert(otherNode);
                        q.push(otherNode);
                    }
                }
            }
        }
    }

    return static_cast<int>(visited.size()) == g.nodeCount();
}
