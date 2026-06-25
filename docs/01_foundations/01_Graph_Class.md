# Complete Line-by-Line Explanation of Graph.hpp
## How It's Different from LeetCode Solutions

---

## File Header & Documentation

```cpp
/**
 * @file Graph.hpp
 * @brief Generic templated Graph implementation using Adjacency List.
 */
```

**What this is:**
- `@file` and `@brief` are Doxygen comments (auto-documentation tools read these)
- LeetCode solutions rarely have these — they're just code
- **Why it matters:** Professional code documents itself. Future you (or interviewers) can generate a website showing all functions

---

## The Design Decision Comment Block

```cpp
/**
 * DESIGN DECISION: Adjacency List vs Adjacency Matrix
 * We choose an Adjacency List (implemented via std::unordered_map) over an
 * Adjacency Matrix for several key reasons:
 */
```

**LeetCode vs Production:**

LeetCode solutions never explain WHY they chose a data structure.
```cpp
// LEETCODE STYLE (bad)
unordered_map<int, vector<int>> adj;  // Just... exists
```

Production code ALWAYS explains the choice:
```cpp
// PRODUCTION STYLE (good)
/**
 * WHY Adjacency List instead of Matrix?
 * 1. Space: O(V+E) vs O(V²)
 * 2. Flexibility: NodeType can be any type, not just 0..V-1
 * 3. Speed: Finding neighbors is O(degree) not O(V)
 */
```

**The three reasons explained:**

```text
1. Space Complexity: Adjacency Lists require O(V + E) memory
   LeetCode typical: 100 nodes, 150 edges
   - List: 100 + 150 = 250 units
   
   Adjacency Matrix:
   - Matrix: 100 × 100 = 10,000 units (40x more memory!)
   
   With 1000 nodes: List = 1,150, Matrix = 1,000,000
   This MATTERS in production.
```

```text
2. Node Flexibility: unordered_map<NodeType, ...> means:
   - LeetCode: nodes must be 0, 1, 2, ..., N-1
   - Production: nodes can be strings ("New York", "Boston"),
                 pairs (x,y for grid), custom objects, etc.
   
   This is THE key difference.
```

```text
3. Edge Enumeration: Finding neighbors of node u:
   - Adjacency List: O(1) to find node u, then O(degree of u) to list neighbors
   - Matrix: O(1) lookup, but must scan entire row (O(V)) to find non-zero entries
```

---

## Includes & Namespaces

```cpp
#pragma once

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <functional>
```

**What each does:**

```cpp
#pragma once
// Prevents double-inclusion of this header file
// LeetCode: not needed (single file)
// Production: CRITICAL (code included from many places)

#include <unordered_map>
// Hash map: O(1) average lookup
// LeetCode: map<int, vector<int>>
// Production: unordered_map<NodeType, vector<Edge>, Hash>

#include <vector>
// Dynamic array (adjacency list)
// LeetCode: vector<vector<int>>
// Production: vector<Edge> for each node

#include <stdexcept>
// For throwing std::invalid_argument
// LeetCode: just returns false or null
// Production: throws exceptions for programmer errors

#include <utility>
// For std::pair (used later in hashing)
// LeetCode: not needed

#include <algorithm>
// For std::find_if, std::any_of
// LeetCode: manual for loops
// Production: STL algorithms (cleaner, optimized)

#include <functional>
// For std::function
// LeetCode: not needed
// Production: used in graph analysis functions
```

---

## GraphUtils Namespace - The Hash Function

```cpp
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
```

**This is COMPLETELY missing from LeetCode solutions.**

**What is hash_combine?**

When you want to hash a pair (x, y), you need to combine two hashes:

```cpp
// NAIVE (bad)
hash(x) + hash(y)  // Two different pairs can give same hash!
// Example: hash(1) + hash(2) = hash(2) + hash(1) 

// CORRECT (this function)
seed = 0
seed ^= hash(1) + magic_constant + shifts
seed ^= hash(2) + magic_constant + shifts
// Returns completely different hashes for (1,2) vs (2,1)
```

**The magic constant 0x9e3779b9:**
```text
0x9e3779b9 = 2654435769 in decimal
This is ((√5 - 1) / 2) × 2^32
It's a carefully chosen number that avoids collisions in bit patterns
Research: Boost C++ library found this through analysis
```

**The XOR (^) and bitshifts:**
```cpp
seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
```

**Breaking it down:**

```text
1. hasher(v)      ← Hash the new value
2. + 0x9e3779b9   ← Add magic constant (prevents patterns)
3. + (seed << 6)  ← Shift seed left 6 bits (use old seed bits)
4. + (seed >> 2)  ← Shift seed right 2 bits (remix old seed)
5. ^=             ← XOR with current seed (scramble bits)

Result: a hash function that:
- Mixes bits well
- Avoids clustering
- Works across multiple values in a compound structure
```

**Why std::pair needs custom hashing:**

```cpp
unordered_map<pair<int,int>, int> grid;  // ERROR in C++!
// pair<int,int> doesn't have a built-in hash function

// Solution: Use SafeHash (defined next)
unordered_map<pair<int,int>, int, SafeHash> grid;  // WORKS!
```

---

## SafeHash Struct

```cpp
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
```

**What is this?**

A "functor" (function-like object) that hashes values.

**Two overloads:**

```cpp
// Overload 1: Generic type T
template <typename T>
std::size_t operator()(const T& val) const {
    return std::hash<T>{}(val);  // Use standard hash
}

// For int, string, long long, etc.
SafeHash h;
h(42)           ← Uses std::hash<int>
h("hello")      ← Uses std::hash<string>
```

```cpp
// Overload 2: Pairs (x, y)
template <typename T1, typename T2>
std::size_t operator()(const std::pair<T1, T2>& val) const {
    std::size_t seed = 0;                      // Start with 0
    hash_combine(seed, val.first);             // Combine hash of first
    hash_combine(seed, val.second);            // Combine hash of second
    return seed;                               // Return mixed hash
}

// Example:
SafeHash h;
h(pair(3, 4))   ← Uses hash_combine to mix both values
```

**Why this matters:**

LeetCode doesn't care about hashing pairs. Production code MUST handle it correctly.

---

## The Graph Class Definition

```cpp
template<typename NodeType, typename WeightType = int, typename Hash = GraphUtils::SafeHash>
class Graph {
```

**Breaking down the template parameters:**

```cpp
typename NodeType
// The type of node IDs
// LeetCode: always int (0, 1, 2, ..., n-1)
// Production: can be int, string, pair<int,int>, custom class
// Example:
//   Graph<int, int>                    // Standard LeetCode
//   Graph<string, double>              // City routing
//   Graph<pair<int,int>, int>          // Grid pathfinding


typename WeightType = int
// The type of edge weights
// = int means "default to int if not specified"
// Examples:
//   Graph<int>                         // NodeType=int, WeightType=int (default)
//   Graph<int, double>                 // NodeType=int, WeightType=double
//   Graph<string, long long>           // Both specified


typename Hash = GraphUtils::SafeHash
// The hashing function for NodeType
// = GraphUtils::SafeHash means "default to SafeHash if not specified"
// Why? Because unordered_map needs a hash function
// Example:
//   unordered_map<NodeType, vector<Edge>, Hash> adjList;
//   This uses the Hash function to hash NodeType values
```

---

## The Edge Struct

```cpp
struct Edge {
    NodeType to;
    WeightType weight;

    Edge(NodeType target, WeightType w) : to(target), weight(w) {}

    bool operator==(const Edge& other) const {
        return to == other.to && weight == other.weight;
    }
};
```

**What is this?**

A pair of (destination node, weight). Represents one edge.

**LeetCode approach:**
```cpp
// LEETCODE
vector<vector<int>> adj;
// Stores just the target node
// Weight implicit (adjacency list, no weights shown)
```

**Production approach:**
```cpp
// PRODUCTION
struct Edge {
    NodeType to;        // Where does this edge point?
    WeightType weight;  // How much does this edge cost?
}

vector<Edge> neighbors;  // All outgoing edges
```

**The constructor:**
```cpp
Edge(NodeType target, WeightType w) : to(target), weight(w) {}

// Syntax: parameter : initializer_list
// This is the modern C++ way to initialize members
// LeetCode: rarely uses member initialization lists
// Production: ALWAYS uses them (safer, more efficient)

// Example usage:
Edge e("Boston", 5.5);
// to = "Boston", weight = 5.5
```

**The equality operator:**
```cpp
bool operator==(const Edge& other) const {
    return to == other.to && weight == other.weight;
}

// This lets us compare two edges:
Edge e1("Boston", 5.0);
Edge e2("Boston", 5.0);
if (e1 == e2) { ... }  // TRUE

// LeetCode: doesn't need this
// Production: needed for find_if (checking if edge exists)
```

---

## NodeIterator Class

```cpp
class NodeIterator {
    typename std::unordered_map<NodeType, std::vector<Edge>, Hash>::const_iterator it;
public:
    NodeIterator(typename std::unordered_map<NodeType, std::vector<Edge>, Hash>::const_iterator iterator) 
        : it(iterator) {}

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
```

**What is this?**

An iterator wrapper that lets you do this:
```cpp
for (const auto& node : graph) {
    cout << node << "\n";
}
```

**Why it's complex:**

Internally, the graph stores:
```cpp
unordered_map<NodeType, vector<Edge>> adjList;
// Keys are nodes
// Values are adjacency lists
```

But we want to iterate over KEYS only, not key-value pairs.

**How it works:**

```cpp
// Member variable:
typename std::unordered_map<NodeType, std::vector<Edge>, Hash>::const_iterator it;
// This is a pointer into the unordered_map
// const means it can't modify what it points to
// This line is LONG because of the template parameters


// Constructor:
NodeIterator(typename std::unordered_map<...>::const_iterator iterator) : it(iterator) {}
// Takes an unordered_map iterator and stores it


// Dereference operator (*):
NodeType operator*() const {
    return it->first;  // Return the KEY (node ID), not the value
}
// When you do *iterator in the for loop, you get the node ID


// Increment operator (++):
NodeIterator& operator++() {
    ++it;      // Move to next entry in map
    return *this;  // Return reference to self (allows chaining)
}
// Lets iterator++; work in for loops


// Inequality operator (!=):
bool operator!=(const NodeIterator& other) const {
    return it != other.it;
}
// Lets for loops check: while (iterator != end)
```

**LeetCode vs Production:**

LeetCode: Just iterate with indices or raw iterators
```cpp
for (auto& edges : adjList) { ... }
```

Production: Custom iterator for clean, safe iteration
```cpp
for (const auto& node : graph) { ... }  // Much cleaner!
```

---

## Private Members

```cpp
private:
    std::unordered_map<NodeType, std::vector<Edge>, Hash> adjList;
    bool directed;
```

**What are these?**

```cpp
std::unordered_map<NodeType, std::vector<Edge>, Hash> adjList;
// The CORE data structure
// Key: Node ID (can be any type)
// Value: Vector of edges from this node
// Hash: The hash function to use

// Example:
adjList["Boston"] = [
    Edge("NYC", 215),
    Edge("Philly", 305)
];
// "Boston" connects to NYC (215 units away) and Philly (305 units away)


bool directed;
// Is the graph directed (A→B only) or undirected (A↔B)?
// LeetCode: hardcoded (usually directed)
// Production: user chooses in constructor
```

---

## Constructor

```cpp
explicit Graph(bool directed = true) : directed(directed) {}
```

**Breaking it down:**

```cpp
explicit
// Prevents accidental implicit conversions
// Example WITHOUT explicit:
//   Graph g = true;  // WRONG! Accidentally creates a directed graph
// Example WITH explicit:
//   Graph g = true;  // ERROR! Must write: Graph g(true);

Graph(bool directed = true)
// Takes a bool parameter
// = true means "default to true if not specified"
// Usage:
//   Graph<int, int> g1;           // directed=true (default)
//   Graph<int, int> g2(true);     // directed=true (explicit)
//   Graph<int, int> g3(false);    // directed=false (undirected)

: directed(directed)
// Member initializer list
// Sets the member variable directed to the parameter directed
// Modern C++ way (better than: this->directed = directed;)
```

**LeetCode vs Production:**

```cpp
// LEETCODE
class Graph {
    vector<vector<int>> adj;
    // Graph is always directed, always stores integers
};

// PRODUCTION
template<typename NodeType, typename WeightType>
class Graph {
    unordered_map<NodeType, vector<Edge>, Hash> adjList;
    bool directed;
    // Flexible: any node type, any weight type, choice of directed/undirected
};
```

---

## addNode Method

```cpp
void addNode(NodeType u) {
    adjList.emplace(u, std::vector<Edge>());
}
```

**What does this do?**

Adds a node (with no edges) to the graph.

```cpp
adjList.emplace(u, std::vector<Edge>())
// If u doesn't exist, insert it with an empty adjacency list
// If u already exists, do nothing (emplace doesn't overwrite)

// Equivalent to:
if (adjList.find(u) == adjList.end()) {
    adjList[u] = vector<Edge>();
}
```

**emplace vs operator[]:**

```cpp
adjList[u] = vector<Edge>();
// Creates an empty vector if u doesn't exist (works)
// But calls vector constructor and assignment

adjList.emplace(u, vector<Edge>());
// Creates in-place if u doesn't exist
// More efficient (one operation instead of two)

// LeetCode: doesn't care, uses adjList[u]
// Production: uses emplace for efficiency
```

**Usage:**

```cpp
Graph<string, double> g;
g.addNode("Boston");
g.addNode("NYC");
// Now g has two isolated nodes
```

---

## addEdge Method - Part 1

```cpp
void addEdge(NodeType u, NodeType v, WeightType w = 1) {
    addNode(u);
    addNode(v);
```

**What's happening:**

```cpp
void addEdge(NodeType u, NodeType v, WeightType w = 1)
// Add an edge from u to v with weight w
// w = 1 means "default to 1 if not specified"
// Usage:
//   graph.addEdge(1, 2);        // weight = 1 (default)
//   graph.addEdge(1, 2, 5);     // weight = 5 (explicit)

addNode(u);
addNode(v);
// Ensure both nodes exist
// LeetCode: assumes nodes already exist
// Production: creates them if needed (safer)
```

---

## addEdge Method - Part 2

```cpp
    auto& uNeighbors = adjList[u];
    auto uIt = std::find_if(uNeighbors.begin(), uNeighbors.end(), [&v](const Edge& e) {
        return e.to == v;
    });

    if (uIt != uNeighbors.end()) {
        uIt->weight = w;
    } else {
        uNeighbors.emplace_back(v, w);
    }
```

**What's this doing?**

Checking if edge u→v already exists, and either updating or adding it.

**Line by line:**

```cpp
auto& uNeighbors = adjList[u];
// Get a reference to the edge list of node u
// auto& = "automatic type, by reference"
// Reference means we can modify uNeighbors directly

auto uIt = std::find_if(uNeighbors.begin(), uNeighbors.end(), [&v](const Edge& e) {
    return e.to == v;
});
// Find the first edge where destination == v
// std::find_if takes:
//   1. begin() = start of list
//   2. end() = end of list
//   3. Lambda function = the condition to check
// 
// Lambda explanation: [&v](const Edge& e) { return e.to == v; }
//   [&v]        = capture v by reference
//   const Edge& = iterate over each Edge
//   return e.to == v  = check if this edge points to v


if (uIt != uNeighbors.end()) {
    uIt->weight = w;
}
// If edge found (uIt is not end), update its weight

else {
    uNeighbors.emplace_back(v, w);
}
// If edge not found, add new edge to the list
```

**LeetCode vs Production:**

```cpp
// LEETCODE
adj[u].push_back(v);
// Always adds, even if edge exists (duplicates allowed!)

// PRODUCTION
// Check first, then either update or add (no duplicates)
```

---

## addEdge Method - Part 3 (Undirected Handling)

```cpp
    if (!directed) {
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
```

**What's this?**

For undirected graphs, add the reverse edge too.

```cpp
if (!directed) {
    // Same process as before, but for the reverse direction
    // If graph is undirected and we add u→v with weight 5
    // We must also add v→u with weight 5
}
```

**Example:**

```cpp
Graph<int, int> g(false);  // Undirected
g.addEdge(1, 2, 10);
// Now:
// 1→2 exists (weight 10)
// 2→1 exists (weight 10)  ← Added automatically
```

---

## removeEdge Method

```cpp
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
```

**What's this?**

Removes edge u→v from the graph (and v→u if undirected).

```cpp
if (!hasNode(u) || !hasNode(v)) {
    return;
}
// Safety check: if either node doesn't exist, do nothing
// LeetCode: would crash or assume nodes exist
// Production: handles edge cases gracefully

auto& uNeighbors = adjList[u];
auto uIt = std::find_if(uNeighbors.begin(), uNeighbors.end(), [&v](const Edge& e) {
    return e.to == v;
});
// Find the edge from u to v

if (uIt != uNeighbors.end()) {
    uNeighbors.erase(uIt);
}
// If found, erase it
```

---

## hasNode Method

```cpp
bool hasNode(NodeType u) const {
    return adjList.find(u) != adjList.end();
}
```

**What's this?**

Check if a node exists in the graph.
