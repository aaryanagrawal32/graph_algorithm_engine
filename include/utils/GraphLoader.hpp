/**
 * @file GraphLoader.hpp
 * @brief Utilities to load Graphs from various file formats.
 *
 * Implements loaders for CSV edge lists, adjacency lists, and simplified
 * OpenStreetMap (OSM) geographic road network files.
 */

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <iostream>
#include "Graph.hpp"

namespace GraphLoaderInternal {
    /**
     * @brief Generic string-to-token converter using stringstreams.
     */
    template<typename T>
    inline T parseToken(const std::string& token) {
        std::stringstream ss(token);
        T val;
        ss >> val;
        return val;
    }

    /**
     * @brief Specialization for std::string to preserve exact spacing and strings.
     */
    template<>
    inline std::string parseToken<std::string>(const std::string& token) {
        return token;
    }
}

/**
 * @brief Loads a graph from a CSV file containing an edge list.
 * Expects lines formatted as: source,destination,weight
 * Comments starting with '#' are ignored.
 *
 * Example:
 * # source,destination,weight
 * 0,1,5.5
 * 1,2,3.0
 */
template<typename NodeType, typename WeightType, typename Hash>
void loadFromCSV(const std::string& filename, Graph<NodeType, WeightType, Hash>& g) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Trim or skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string uStr, vStr, wStr;

        if (std::getline(ss, uStr, ',') && std::getline(ss, vStr, ',') && std::getline(ss, wStr, ',')) {
            try {
                NodeType u = GraphLoaderInternal::parseToken<NodeType>(uStr);
                NodeType v = GraphLoaderInternal::parseToken<NodeType>(vStr);
                WeightType w = GraphLoaderInternal::parseToken<WeightType>(wStr);

                g.addEdge(u, v, w);
            } catch (const std::exception& e) {
                throw std::runtime_error("Malformed CSV line at " + filename + ":" + std::to_string(lineNumber) + " -> \"" + line + "\". Error: " + e.what());
            }
        } else {
            throw std::runtime_error("Malformed CSV line at " + filename + ":" + std::to_string(lineNumber) + " -> \"" + line + "\". Expected 3 columns (u,v,w).");
        }
    }
}

/**
 * @brief Loads a graph from a space-separated adjacency list file.
 * Expects lines formatted as: source neighbor1,weight1 neighbor2,weight2 ...
 *
 * Example:
 * 0 1,4.5 2,1.2
 * 1 2,3.0
 */
template<typename NodeType, typename WeightType, typename Hash>
void loadFromAdjacencyList(const std::string& filename, Graph<NodeType, WeightType, Hash>& g) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;

        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string uStr;
        if (!(ss >> uStr)) {
            continue; // Empty line
        }

        NodeType u = GraphLoaderInternal::parseToken<NodeType>(uStr);
        g.addNode(u);

        std::string neighborBlock;
        while (ss >> neighborBlock) {
            std::stringstream nss(neighborBlock);
            std::string vStr, wStr;

            if (std::getline(nss, vStr, ',') && std::getline(nss, wStr, ',')) {
                try {
                    NodeType v = GraphLoaderInternal::parseToken<NodeType>(vStr);
                    WeightType w = GraphLoaderInternal::parseToken<WeightType>(wStr);
                    g.addEdge(u, v, w);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Malformed adjacency block in " + filename + ":" + std::to_string(lineNumber) + " -> \"" + neighborBlock + "\".");
                }
            } else {
                throw std::runtime_error("Malformed adjacency block in " + filename + ":" + std::to_string(lineNumber) + " -> \"" + neighborBlock + "\". Expected neighbor,weight.");
            }
        }
    }
}

/**
 * @brief Loads a geographic road network from a CSV file.
 * The file is divided into two sections marked by headers:
 * [Nodes] containing: node_id,latitude,longitude
 * [Edges] containing: source_id,destination_id,distance
 *
 * Populates both the Graph structure and a coordinate map (for A* heuristics).
 */
template<typename NodeType, typename WeightType, typename Hash>
void loadFromRoadNetworkCSV(
    const std::string& filename,
    Graph<NodeType, WeightType, Hash>& g,
    std::unordered_map<NodeType, std::pair<double, double>, Hash>& coordMap) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open road network file: " + filename);
    }

    std::string line;
    int lineNumber = 0;
    bool parsingNodes = false;
    bool parsingEdges = false;

    while (std::getline(file, line)) {
        lineNumber++;

        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Section Headers
        if (line == "[Nodes]") {
            parsingNodes = true;
            parsingEdges = false;
            continue;
        } else if (line == "[Edges]") {
            parsingNodes = false;
            parsingEdges = true;
            continue;
        }

        std::stringstream ss(line);
        if (parsingNodes) {
            std::string idStr, latStr, lonStr;
            if (std::getline(ss, idStr, ',') && std::getline(ss, latStr, ',') && std::getline(ss, lonStr, ',')) {
                try {
                    NodeType id = GraphLoaderInternal::parseToken<NodeType>(idStr);
                    double lat = std::stod(latStr);
                    double lon = std::stod(lonStr);
                    
                    coordMap[id] = {lat, lon};
                    g.addNode(id);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Malformed [Nodes] line at " + filename + ":" + std::to_string(lineNumber) + " -> \"" + line + "\".");
                }
            }
        } else if (parsingEdges) {
            std::string uStr, vStr, wStr;
            if (std::getline(ss, uStr, ',') && std::getline(ss, vStr, ',') && std::getline(ss, wStr, ',')) {
                try {
                    NodeType u = GraphLoaderInternal::parseToken<NodeType>(uStr);
                    NodeType v = GraphLoaderInternal::parseToken<NodeType>(vStr);
                    WeightType w = GraphLoaderInternal::parseToken<WeightType>(wStr);

                    g.addEdge(u, v, w);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Malformed [Edges] line at " + filename + ":" + std::to_string(lineNumber) + " -> \"" + line + "\".");
                }
            }
        }
    }
}
