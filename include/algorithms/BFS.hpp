/**
 * @file BFS.hpp
 * @brief Breadth-First Search (BFS) algorithms.
 *
 * Implements basic BFS traversal and BFS shortest path algorithms for
 * the generic Graph class template.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 *
 * WHY USE THIS: Useful for level-order traversals and finding shortest
 * paths in UNWEIGHTED graphs.
 */

#pragma once

#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "Graph.hpp"

/**
 * @brief Performs a Breadth-First Search (BFS) traversal starting from a given node.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @param start The node to start the traversal from.
 * @return std::vector<NodeType> Order in which nodes were visited.
 *
 * Time Complexity: O(V + E) average, where V is the number of vertices and E is the number of edges.
 * Space Complexity: O(V) for the queue and visited set.
 *
 * WHY queue (FIFO): A queue ensures level-order traversal (nodes are processed in the order
 * they are discovered). All nodes at distance d are visited before any node at distance d + 1.
 * Using a stack (LIFO) would change the behavior to Depth-First Search.
 *
 * WHY mark visited on PUSH, not POP: Marking visited when pushing to the queue prevents the
 * same node from being pushed multiple times if it has multiple incoming edges from the current level.
 * This keeps the queue size bounded by O(V) and avoids redundant work.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> bfs(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    std::vector<NodeType> visitOrder;

    // Edge Case: If the start node is not present in the graph, return an empty traversal
    if (!g.hasNode(start)) {
        return visitOrder;
    }

    std::unordered_set<NodeType, Hash> visited;
    std::queue<NodeType> q;

    // Initialize search
    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        NodeType curr = q.front();
        q.pop();
        visitOrder.push_back(curr);

        // Traverse all neighbors of the current node
        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to); // Mark visited immediately on push
                q.push(edge.to);
            }
        }
    }

    return visitOrder;
}

/**
 * @brief Computes the shortest path from a source node to a destination node using BFS.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @param src The source node.
 * @param dst The destination node.
 * @return std::vector<NodeType> The shortest path from src to dst. Empty if unreachable.
 *
 * Time Complexity: O(V + E) average.
 * Space Complexity: O(V) for queue, visited set, and prev map.
 *
 * WHY BFS finds shortest path only in UNWEIGHTED graphs:
 * BFS explores the graph level-by-level, meaning it finds the path with the fewest *edges*
 * (minimum hop count) from src to dst. In an unweighted graph, all edge weights are equal (e.g., 1),
 * so minimum hop count is equivalent to minimum path cost. In a weighted graph, a path with more
 * edges can have a smaller total weight than a path with fewer edges, which BFS cannot detect.
 * For weighted graphs, Dijkstra's algorithm must be used.
 *
 * WHY prev map helps reconstruct path:
 * By storing prev[neighbor] = current when we discover a node, we build a tree of parent pointers
 * tracing back to the source. Once we reach the destination, we can walk backward from dst to src
 * and reverse the list to get the path.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> bfsShortestPath(const Graph<NodeType, WeightType, Hash>& g, NodeType src, NodeType dst) {
    std::vector<NodeType> path;

    // Edge Case: Ensure both source and destination exist in the graph
    if (!g.hasNode(src) || !g.hasNode(dst)) {
        return path;
    }

    // Edge Case: Source is the destination
    if (src == dst) {
        path.push_back(src);
        return path;
    }

    std::unordered_set<NodeType, Hash> visited;
    std::unordered_map<NodeType, NodeType, Hash> prev;
    std::queue<NodeType> q;

    q.push(src);
    visited.insert(src);

    bool reachedDst = false;

    while (!q.empty()) {
        NodeType curr = q.front();
        q.pop();

        if (curr == dst) {
            reachedDst = true;
            break;
        }

        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                prev[edge.to] = curr;
                q.push(edge.to);
            }
        }
    }

    // If destination was not reached, return empty vector
    if (!reachedDst) {
        return path;
    }

    // Reconstruct the shortest path by walking backward from dst to src
    NodeType curr = dst;
    while (curr != src) {
        path.push_back(curr);
        curr = prev[curr];
    }
    path.push_back(src);

    // Reverse path to change order from [dst -> ... -> src] to [src -> ... -> dst]
    std::reverse(path.begin(), path.end());

    return path;
}
