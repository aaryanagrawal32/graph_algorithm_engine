/**
 * @file Dijkstra.hpp
 * @brief Dijkstra's shortest path algorithm.
 *
 * Finds the shortest paths from a source node to all other nodes
 * in a weighted graph with non-negative edge weights.
 *
 * Time Complexity: O(E log V)
 * Space Complexity: O(V)
 *
 * WHY USE THIS: When you have non-negative weights and need the
 * shortest path. Faster than Bellman-Ford for most cases.
 * Use Bellman-Ford when negative weights exist.
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "Graph.hpp"

/**
 * @struct DijkstraResult
 * @brief Contains the resulting shortest distance and predecessor tracking maps.
 */
template<typename NodeType, typename WeightType, typename Hash = GraphUtils::SafeHash>
struct DijkstraResult {
    std::unordered_map<NodeType, WeightType, Hash> dist; // Node -> Shortest distance from source
    std::unordered_map<NodeType, NodeType, Hash> prev;   // Node -> Predecessor node on shortest path
};

/**
 * @brief Computes shortest paths from a single source node using Dijkstra's algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights. Must support arithmetic operations.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @param src The starting source node.
 * @return DijkstraResult containing the distance and predecessor maps.
 *
 * Time Complexity Derivation: O(E log V)
 * - Initializing distances to infinity takes O(V) time.
 * - Each vertex is pushed and popped from the priority queue at most once per incoming edge.
 *   In the worst case, we do O(E) pushes and O(E) pops.
 * - Each priority queue insertion/deletion takes O(log(size_of_heap)) = O(log V) time.
 * - Relaxing edges takes O(deg(u)) per node, totaling O(E) across the whole run.
 * - Total time is O(E log V + V). For connected graphs (E >= V - 1), this is O(E log V).
 *
 * WHY greater<>: By default, std::priority_queue is a max-heap (uses less<>).
 * We need a min-heap to always process the closest discovered node first.
 * greater<> reverses the comparison, giving us min-heap behavior.
 *
 * WHY stale entries appear & how we handle them:
 * C++ std::priority_queue does not support a decreaseKey operation. When a shorter path
 * to an existing node u is found, we push the new shorter distance (new_d, u) to the heap.
 * The older, larger distance (old_d, u) remains in the heap.
 * When (old_d, u) is eventually popped, it is a "stale entry". We check:
 * `if (d > dist[u]) continue;`
 * This ignores the stale entry because we've already processed u at a shorter distance,
 * preventing redundant neighbor scans.
 *
 * WHY this fails with negative weights:
 * Dijkstra's algorithm is greedy. It assumes that once a node u is popped, its shortest
 * distance is finalized and cannot be improved by paths through other remaining nodes.
 * If negative edge weights exist, a longer hop-count path could have a smaller total weight,
 * violating this assumption. Nodes already popped would require re-evaluation, which
 * Dijkstra does not perform.
 */
template<typename NodeType, typename WeightType, typename Hash>
DijkstraResult<NodeType, WeightType, Hash> dijkstra(const Graph<NodeType, WeightType, Hash>& g, NodeType src) {
    DijkstraResult<NodeType, WeightType, Hash> result;

    if (!g.hasNode(src)) {
        return result;
    }

    // Initialize all distances to infinity
    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    for (const auto& node : g) {
        result.dist[node] = INF;
    }
    result.dist[src] = 0;

    // Min-heap storing pair of: {distance, node_id}
    using PQElement = std::pair<WeightType, NodeType>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Stale entry check: skip if we've already found a shorter path to u
        if (d > result.dist[u]) {
            continue;
        }

        // Relax neighbors
        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            WeightType w = edge.weight;

            if (result.dist[u] + w < result.dist[v]) {
                result.dist[v] = result.dist[u] + w;
                result.prev[v] = u;
                pq.push({result.dist[v], v});
            }
        }
    }

    return result;
}

/**
 * @brief Reconstructs the shortest path from src to dst using the Dijkstra predecessor map.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param result The result object returned by the dijkstra function.
 * @param src The source node.
 * @param dst The destination node.
 * @return std::vector<NodeType> The path from src to dst. Empty if unreachable.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> reconstructPath(const DijkstraResult<NodeType, WeightType, Hash>& result, NodeType src, NodeType dst) {
    std::vector<NodeType> path;

    // Verify destination exists and was reached
    auto distIt = result.dist.find(dst);
    if (distIt == result.dist.end()) {
        return path;
    }

    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    if (distIt->second == INF) {
        return path; // Unreachable
    }

    // Backtrack from destination to source using prev map
    NodeType curr = dst;
    while (curr != src) {
        path.push_back(curr);
        auto prevIt = result.prev.find(curr);
        if (prevIt == result.prev.end()) {
            return std::vector<NodeType>(); // Path is broken (should not happen if reachable)
        }
        curr = prevIt->second;
    }
    path.push_back(src);

    // Reverse path to go from src to dst
    std::reverse(path.begin(), path.end());

    return path;
}

/**
 * @struct DijkstraStatsResult
 * @brief Contains the reconstructed shortest path and execution statistics.
 */
template<typename NodeType>
struct DijkstraStatsResult {
    std::vector<NodeType> path;
    int nodesVisited = 0;
};

/**
 * @brief Performs Dijkstra's algorithm from src to dst, tracking unique visited nodes.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph object.
 * @param src The source node.
 * @param dst The destination node.
 * @return DijkstraStatsResult containing the path and nodesVisited count.
 */
template<typename NodeType, typename WeightType, typename Hash>
DijkstraStatsResult<NodeType> dijkstraWithStats(
    const Graph<NodeType, WeightType, Hash>& g, NodeType src, NodeType dst) {
    
    DijkstraStatsResult<NodeType> result;
    if (!g.hasNode(src) || !g.hasNode(dst)) {
        return result;
    }

    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    std::unordered_map<NodeType, WeightType, Hash> dist;
    std::unordered_map<NodeType, NodeType, Hash> prev;

    for (const auto& node : g) {
        dist[node] = INF;
    }
    dist[src] = 0;

    using PQElement = std::pair<WeightType, NodeType>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    pq.push({0, src});

    std::unordered_set<NodeType, Hash> closedSet;

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (closedSet.find(u) != closedSet.end()) {
            continue;
        }
        closedSet.insert(u);
        result.nodesVisited++;

        if (u == dst) {
            // Reconstruct path
            NodeType curr = dst;
            while (curr != src) {
                result.path.push_back(curr);
                curr = prev[curr];
            }
            result.path.push_back(src);
            std::reverse(result.path.begin(), result.path.end());
            return result;
        }

        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            WeightType w = edge.weight;

            if (closedSet.find(v) != closedSet.end()) {
                continue;
            }

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    return result; // Empty path if unreachable
}

