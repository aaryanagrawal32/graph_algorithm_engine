/**
 * @file GraphExporter.hpp
 * @brief Utilities to export Graphs and algorithm logs.
 *
 * Implements exporters for CSV edge lists, JSON representation of graphs
 * (compatible with D3.js), and JSON arrays for animating algorithm steps.
 */

#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <iostream>
#include "Graph.hpp"

/**
 * @brief Exports a graph to a D3.js-compatible JSON format.
 * Can optionally embed node coordinates if a coordinate map is supplied.
 *
 * Format:
 * {
 *   "nodes": [
 *     {"id": "0", "x": 10.0, "y": 20.0},
 *     {"id": "1", "x": 15.0, "y": 25.0}
 *   ],
 *   "links": [
 *     {"source": "0", "target": "1", "weight": 5.5}
 *   ]
 * }
 */
template<typename NodeType, typename WeightType, typename Hash,
         typename CoordHash = std::hash<NodeType>>
void exportToJSON(
    const Graph<NodeType, WeightType, Hash>& g,
    const std::string& filename,
    const std::unordered_map<NodeType, std::pair<double, double>, CoordHash>& coordMap = {}) {

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Warning] Failed to export JSON to: " << filename << "\n";
        return;
    }

    file << "{\n";
    
    // 1. Export Nodes
    file << "  \"nodes\": [\n";
    std::vector<NodeType> nodes = g.getAllNodes();
    for (size_t i = 0; i < nodes.size(); ++i) {
        file << "    { \"id\": \"" << nodes[i] << "\"";
        
        // Append coordinates if available in the map
        auto it = coordMap.find(nodes[i]);
        if (it != coordMap.end()) {
            file << ", \"x\": " << it->second.first << ", \"y\": " << it->second.second;
        }
        
        file << " }";
        if (i + 1 < nodes.size()) {
            file << ",";
        }
        file << "\n";
    }
    file << "  ],\n";

    // 2. Export Links (Edges)
    file << "  \"links\": [\n";
    bool firstLink = true;
    for (const auto& u : nodes) {
        for (const auto& edge : g.neighbors(u)) {
            // For undirected graphs, each edge u-v is listed in both u and v.
            // In D3.js, we can export both or just one. Exporting all is safer
            // for directed, and harmless for undirected.0
            if (!firstLink) {
                file << ",\n";
            }
            file << "    { \"source\": \"" << u << "\", \"target\": \"" << edge.to 
                 << "\", \"weight\": " << edge.weight << " }";
            firstLink = false;
        }
    }
    file << "\n  ]\n";
    
    file << "}\n";
}

/**
 * @brief Exports a sequence of visited nodes (from an algorithm run) to a JSON file.
 * Used by the visualizer to animate traversal/path steps.
 *
 * Format:
 * {
 *   "steps": ["0", "1", "2"]
 * }
 */
template<typename NodeType>
void exportAlgorithmSteps(const std::vector<NodeType>& visitOrder, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Warning] Failed to export steps to: " << filename << "\n";
        return;
    }

    file << "{\n";
    file << "  \"steps\": [\n";
    for (size_t i = 0; i < visitOrder.size(); ++i) {
        file << "    \"" << visitOrder[i] << "\"";
        if (i + 1 < visitOrder.size()) {
            file << ",";
        }
        file << "\n";
    }
    file << "  ]\n";
    file << "}\n";
}

/**
 * @brief Exports the graph to a simple CSV edge list.
 * Format: source,destination,weight
 */
template<typename NodeType, typename WeightType, typename Hash>
void exportToCSV(const Graph<NodeType, WeightType, Hash>& g, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Warning] Failed to export CSV to: " << filename << "\n";
        return;
    }

    file << "# source,destination,weight\n";
    for (const auto& u : g.getAllNodes()) {
        for (const auto& edge : g.neighbors(u)) {
            file << u << "," << edge.to << "," << edge.weight << "\n";
        }
    }
}
