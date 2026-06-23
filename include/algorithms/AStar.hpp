/**
 * @file AStar.hpp
 * @brief A* shortest path search algorithm.
 *
 * Implements the A* search algorithm using heuristics to find the shortest
 * path between a source node and a destination node.
 *
 * Time Complexity: O(E log V) worst case (same as Dijkstra), but significantly
 * faster in practice with a good heuristic.
 * Space Complexity: O(V)
 */

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <functional>
#include <cmath>
#include "Graph.hpp"

/**
 * @struct AStarResult
 * @brief Contains the reconstructed shortest path and search statistics.
 */
template<typename NodeType>
struct AStarResult {
    std::vector<NodeType> path; // Shortest path from src to dst. Empty if unreachable.
    int nodesVisited = 0;       // Number of unique nodes popped and processed (closed)
};

// ============================================================================
// Built-in Heuristics
// ============================================================================

/**
 * @brief Zero heuristic. ALWAYS returns 0.0.
 * Passing this to A* degenerates the algorithm exactly into Dijkstra's algorithm.
 */
inline auto zeroHeuristic = [](const auto&, const auto&) {
    return 0.0;
};

/**
 * @brief Manhattan distance heuristic for grid-based coordinates (pair<int, int>).
 * Calculates |x1 - x2| + |y1 - y2|.
 */
inline auto manhattanHeuristic = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return static_cast<double>(std::abs(a.first - b.first) + std::abs(a.second - b.second));
};

/**
 * @brief Euclidean distance heuristic for geographic/spatial coordinates (pair<double, double>).
 * Calculates sqrt((x1 - x2)^2 + (y1 - y2)^2).
 */
inline auto euclideanHeuristic = [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
    double dx = a.first - b.first;
    double dy = a.second - b.second;
    return std::sqrt(dx * dx + dy * dy);
};

// ============================================================================
// A* Implementation
// ============================================================================

/**
 * @brief Performs A* search algorithm from a source node to a destination node.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @tparam Heuristic A callable type with signature double(NodeType, NodeType).
 * @param g Reference to the Graph.
 * @param src Starting node.
 * @param dst Target destination node.
 * @param h Heuristic function estimating the cost from a node to the destination.
 * @return AStarResult containing the shortest path and nodes visited.
 *
 * HOW A* WORKS: f(n) = g(n) + h(n)
 * - g(n) is the exact cost from the source node to node n.
 * - h(n) is the estimated remaining cost from node n to the destination.
 * - f(n) is the total estimated cost of a path passing through node n.
 * By ordering our priority queue by f(n) instead of just g(n) (as Dijkstra does),
 * A* focuses its search space in the direction of the goal, pruning branches that
 * lead away from the destination.
 *
 * HEURISTIC ADMISSIBILITY:
 * A heuristic h(n) is "admissible" if it never overestimates the actual cost to reach
 * the destination (i.e., h(n) <= h*(n) where h*(n) is the true optimal cost).
 * If a heuristic is admissible, A* is GUARANTEED to find the mathematically optimal
 * shortest path without missing shortcuts.
 *
 * WHY MANHATTAN IS ADMISSIBLE FOR GRIDS:
 * In a grid layout with 4-directional movement (up, down, left, right), the shortest
 * possible path between two points without obstacles is exactly their Manhattan distance.
 * Since obstacles can only make the actual path longer, the Manhattan distance will never
 * overestimate the true distance, making it an admissible heuristic.
 */
template<typename NodeType, typename WeightType, typename Hash, typename Heuristic>
AStarResult<NodeType> astar(const Graph<NodeType, WeightType, Hash>& g, NodeType src, NodeType dst, Heuristic h) {
    AStarResult<NodeType> result;

    if (!g.hasNode(src) || !g.hasNode(dst)) {
        return result;
    }

    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    // gScore[n] is the cost of the cheapest path from start to n currently known
    std::unordered_map<NodeType, WeightType, Hash> gScore;
    std::unordered_map<NodeType, NodeType, Hash> prev;

    for (const auto& node : g) {
        gScore[node] = INF;
    }
    gScore[src] = 0;

    // Min-heap storing pair of {fScore, node_id}
    using PQElement = std::pair<double, NodeType>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    // fScore[src] is h(src, dst) because gScore[src] is 0
    pq.push({static_cast<double>(h(src, dst)), src});

    // Set of nodes already processed (closed) to prevent re-evaluating nodes
    std::unordered_set<NodeType, Hash> closedSet;

    while (!pq.empty()) {
        auto [f, u] = pq.top();
        pq.pop();

        // Skip if u has already been processed at a lower f-score
        if (closedSet.find(u) != closedSet.end()) {
            continue;
        }

        closedSet.insert(u);
        result.nodesVisited++;

        // Goal reached: reconstruct and return path
        if (u == dst) {
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
            WeightType weight = edge.weight;

            if (closedSet.find(v) != closedSet.end()) {
                continue;
            }

            WeightType tentativeG = gScore[u] + weight;
            if (tentativeG < gScore[v]) {
                gScore[v] = tentativeG;
                prev[v] = u;
                double fScore = static_cast<double>(tentativeG) + h(v, dst);
                pq.push({fScore, v});
            }
        }
    }

    return result; // Empty path if unreachable
}
