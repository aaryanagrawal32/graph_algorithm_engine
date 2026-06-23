/**
 * @file BellmanFord.hpp
 * @brief Bellman-Ford shortest path algorithm.
 *
 * Finds the shortest paths from a single source vertex to all other
 * vertices in a weighted graph, supporting negative edge weights.
 * Detects negative cycles and throws if one is reachable from the source.
 *
 * Time Complexity: O(V * E)
 * Space Complexity: O(V)
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <limits>
#include <stdexcept>
#include "Graph.hpp"

/**
 * @brief Computes shortest paths from a single source node using the Bellman-Ford algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @param src The starting source node.
 * @return std::unordered_map<NodeType, WeightType, Hash> Map of shortest distances from src to all nodes.
 * @throws std::runtime_error If a negative weight cycle is detected reachable from src.
 *
 * WHY V-1 iterations of edge relaxation:
 * A shortest path in a graph with V vertices (without negative cycles) will visit at most
 * V - 1 edges. Each iteration of relaxing all edges propagates the correct distance estimates
 * by at least one edge hop. Thus, V - 1 iterations are mathematically guaranteed to find
 * the optimal shortest paths to all reachable vertices.
 *
 * WHY the V-th iteration detects negative cycles:
 * If there are no negative weight cycles, all shortest paths have been finalized during the
 * V-1 passes, and a V-th pass will make zero modifications to the distance values. If any
 * distance decreases during the V-th relaxation pass:
 * `dist[u] + w < dist[v]`
 * it means there exists a path whose cost keeps shrinking infinitely by traversing more edges.
 * This indicates a reachable cycle with a net negative cost (a negative cycle).
 *
 * DIJKSTRA vs BELLMAN-FORD:
 * - Dijkstra's algorithm runs in O(E log V) time, which is much faster than Bellman-Ford's O(VE).
 *   Use Dijkstra when all edge weights are guaranteed to be non-negative.
 * - Bellman-Ford must be used when the graph contains negative weights or if you need to detect
 *   negative cycles.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::unordered_map<NodeType, WeightType, Hash> bellmanFord(
    const Graph<NodeType, WeightType, Hash>& g, NodeType src) {

    if (!g.hasNode(src)) {
        throw std::invalid_argument("Source node does not exist in the graph.");
    }

    std::unordered_map<NodeType, WeightType, Hash> dist;
    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    // Initialize distances
    for (const auto& node : g) {
        dist[node] = INF;
    }
    dist[src] = 0;

    int V = g.nodeCount();
    std::vector<NodeType> allNodes = g.getAllNodes();

    // V - 1 iterations of edge relaxation
    for (int i = 0; i < V - 1; ++i) {
        bool anyChange = false;

        for (const auto& u : allNodes) {
            // Skip nodes that are currently unreachable to prevent underflow/overflow errors
            if (dist[u] == INF) {
                continue;
            }

            for (const auto& edge : g.neighbors(u)) {
                NodeType v = edge.to;
                WeightType w = edge.weight;

                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    anyChange = true;
                }
            }
        }

        // Optimization: If no edge was relaxed in an iteration, we've already finalized all paths.
        if (!anyChange) {
            break;
        }
    }

    // V-th iteration to check for negative cycles
    for (const auto& u : allNodes) {
        if (dist[u] == INF) {
            continue;
        }

        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            WeightType w = edge.weight;

            if (dist[u] + w < dist[v]) {
                throw std::runtime_error("Negative cycle detected");
            }
        }
    }

    return dist;
}
