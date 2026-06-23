/**
 * @file Graph.hpp
 * @brief Generic templated Graph implementation using Adjacency List.
 *
 * This implementation represents a Graph with vertices of type NodeType
 * and edge weights of type WeightType (defaults to int).
 *
 * DESIGN DECISION: Adjacency List vs Adjacency Matrix
 * We choose an Adjacency List (implemented via std::unordered_map) over an
 * Adjacency Matrix for several key reasons:
 * 1. Space Complexity: Adjacency Lists require O(V + E) memory, which is highly
 *    efficient for sparse graphs. Adjacency Matrices require O(V^2) memory, which
 *    wastes space when the number of edges is much less than V^2.
 * 2. Node Flexibility: The keys of std::unordered_map can be generic types
 *    (e.g., std::string, std::pair, custom objects) rather than just contiguous integers,
 *    making it easy to model real-world networks (like street intersections).
 * 3. Edge Enumeration: Finding the neighbors of a node takes O(1) average lookup
 *    time to find the node, and then O(deg(u)) to traverse its edges, which is faster
 *    than scanning an entire row of size V in an adjacency matrix.
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <functional>

namespace GraphUtils {
    /**
     * @brief Combines a seed hash with another hash value.
     * Often used to hash compound structures like std::pair.
     */
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    /**
     * @brief SafeHash provides a universal hash functor that supports standard library types
     * and automatically supports std::pair, enabling coordinate-based nodes (e.g. grids) out-of-the-box.
     */
    struct SafeHash {
        template <typename T>
        std::size_t operator()(const T& val) const {
            return std::hash<T>{}(val);
        }

        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& val) const {
            std::size_t seed = 0;
            hash_combine(seed, val.first);
            hash_combine(seed, val.second);
            return seed;
        }
    };
}

template<typename NodeType, typename WeightType = int, typename Hash = GraphUtils::SafeHash>
class Graph {
public:
    /**
     * @struct Edge
     * @brief Represents a directed, weighted edge targeting a node.
     */
    struct Edge {
        NodeType to;
        WeightType weight;

        Edge(NodeType target, WeightType w) : to(target), weight(w) {}

        bool operator==(const Edge& other) const {
            return to == other.to && weight == other.weight;
        }
    };

    /**
     * @class NodeIterator
     * @brief A custom read-only iterator wrapper to enable range-based for loops over Graph nodes.
     * Dereferencing this iterator yields a NodeType.
     */
    class NodeIterator {
        typename std::unordered_map<NodeType, std::vector<Edge>, Hash>::const_iterator it;
    public:
        NodeIterator(typename std::unordered_map<NodeType, std::vector<Edge>, Hash>::const_iterator iterator) : it(iterator) {}

        NodeType operator*() const {
            return it->first;
        }

        NodeIterator& operator++() {
            ++it;
            return *this;
        }

        bool operator!=(const NodeIterator& other) const {
            return it != other.it;
        }
    };

private:
    std::unordered_map<NodeType, std::vector<Edge>, Hash> adjList;
    bool directed;

public:
    /**
     * @brief Constructs a Graph object.
     * @param directed If true, the graph is directed. Otherwise, it is undirected.
     * Time Complexity: O(1)
     */
    explicit Graph(bool directed = true) : directed(directed) {}

    /**
     * @brief Adds a node to the graph if it does not already exist.
     * @param u The node to add.
     * Time Complexity: O(1) average lookup and insertion.
     * Edge Cases: Handles duplicate additions gracefully by doing nothing.
     */
    void addNode(NodeType u) {
        // Inserts u with an empty adjacency list if not already present.
        adjList.emplace(u, std::vector<Edge>());
    }

    /**
     * @brief Adds a weighted edge between two nodes. Automatically inserts nodes if they do not exist.
     * If the edge already exists, its weight is updated.
     * @param u Source node (or endpoint 1 if undirected).
     * @param v Destination node (or endpoint 2 if undirected).
     * @param w Weight of the edge (defaults to 1).
     * Time Complexity: O(1) average lookup, plus O(deg(u)) to check for duplicate edge.
     * Edge Cases: Handles duplicate edges by updating the weight, preventing parallel edges.
     */
    void addEdge(NodeType u, NodeType v, WeightType w = 1) {
        addNode(u);
        addNode(v);

        // Check if edge already exists to u->v to prevent duplicates (graceful handling)
        auto& uNeighbors = adjList[u];
        auto uIt = std::find_if(uNeighbors.begin(), uNeighbors.end(), [&v](const Edge& e) {
            return e.to == v;
        });

        if (uIt != uNeighbors.end()) {
            uIt->weight = w; // Update existing edge weight
        } else {
            uNeighbors.emplace_back(v, w); // Add new edge
        }

        if (!directed) {
            // Replicate the edge check/addition in the reverse direction for undirected graphs
            auto& vNeighbors = adjList[v];
            auto vIt = std::find_if(vNeighbors.begin(), vNeighbors.end(), [&u](const Edge& e) {
                return e.to == u;
            });

            if (vIt != vNeighbors.end()) {
                vIt->weight = w;
            } else {
                vNeighbors.emplace_back(u, w);
            }
        }
    }

    /**
     * @brief Removes a directed edge u->v from the graph. If undirected, removes v->u as well.
     * @param u Source node.
     * @param v Destination node.
     * Time Complexity: O(deg(u)) to find and erase the edge. For undirected, also O(deg(v)).
     * Edge Cases: Does nothing if the nodes or the edge do not exist.
     */
    void removeEdge(NodeType u, NodeType v) {
        if (!hasNode(u) || !hasNode(v)) {
            return;
        }

        auto& uNeighbors = adjList[u];
        auto uIt = std::find_if(uNeighbors.begin(), uNeighbors.end(), [&v](const Edge& e) {
            return e.to == v;
        });
        if (uIt != uNeighbors.end()) {
            uNeighbors.erase(uIt);
        }

        if (!directed) {
            auto& vNeighbors = adjList[v];
            auto vIt = std::find_if(vNeighbors.begin(), vNeighbors.end(), [&u](const Edge& e) {
                return e.to == u;
            });
            if (vIt != vNeighbors.end()) {
                vNeighbors.erase(vIt);
            }
        }
    }

    /**
     * @brief Checks if a node exists in the graph.
     * @param u The node to check.
     * @return true if the node exists, false otherwise.
     * Time Complexity: O(1) average.
     */
    bool hasNode(NodeType u) const {
        return adjList.find(u) != adjList.end();
    }

    /**
     * @brief Checks if a directed edge exists from u to v.
     * @param u Source node.
     * @param v Destination node.
     * @return true if the edge exists, false otherwise.
     * Time Complexity: O(deg(u)) to scan neighbors of u.
     */
    bool hasEdge(NodeType u, NodeType v) const {
        auto it = adjList.find(u);
        if (it == adjList.end()) {
            return false;
        }
        const auto& neighbors = it->second;
        return std::any_of(neighbors.begin(), neighbors.end(), [&v](const Edge& e) {
            return e.to == v;
        });
    }

    /**
     * @brief Returns a read-only reference to the neighbor edge list of node u.
     * @param u The node whose neighbors are requested.
     * @return const std::vector<Edge>& The list of outgoing edges.
     * Time Complexity: O(1) average lookup.
     * Edge Cases: Throws std::invalid_argument if node u does not exist.
     */
    const std::vector<Edge>& neighbors(NodeType u) const {
        auto it = adjList.find(u);
        if (it == adjList.end()) {
            throw std::invalid_argument("Node does not exist in the graph.");
        }
        return it->second;
    }

    /**
     * @brief Returns the total number of unique nodes in the graph.
     * @return int Node count.
     * Time Complexity: O(1).
     */
    int nodeCount() const {
        return static_cast<int>(adjList.size());
    }

    /**
     * @brief Returns the total number of unique edges in the graph.
     * @return int Edge count.
     * Time Complexity: O(V) to sum edge list sizes.
     * Note: For undirected graphs, each edge is counted once, meaning
     * the sum of adjacency list sizes is divided by 2.
     */
    int edgeCount() const {
        int total = 0;
        for (const auto& [node, edges] : adjList) {
            total += static_cast<int>(edges.size());
        }
        return directed ? total : (total / 2);
    }

    /**
     * @brief Retrieves all node keys currently in the graph.
     * @return std::vector<NodeType> A vector of all node keys.
     * Time Complexity: O(V) to copy keys.
     */
    std::vector<NodeType> getAllNodes() const {
        std::vector<NodeType> nodes;
        nodes.reserve(adjList.size());
        for (const auto& [node, edges] : adjList) {
            nodes.push_back(node);
        }
        return nodes;
    }

    /**
     * @brief Checks if the graph is directed.
     * @return true if directed, false if undirected.
     * Time Complexity: O(1).
     */
    bool isDirected() const {
        return directed;
    }

    /**
     * @brief Returns an iterator to the beginning of the graph nodes.
     * Time Complexity: O(1)
     */
    NodeIterator begin() const {
        return NodeIterator(adjList.begin());
    }

    /**
     * @brief Returns an iterator to the end of the graph nodes.
     * Time Complexity: O(1)
     */
    NodeIterator end() const {
        return NodeIterator(adjList.end());
    }
};
