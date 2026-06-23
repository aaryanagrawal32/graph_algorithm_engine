# Agentic AI Master Prompt — Graph Algorithm Engine (C++)
# Repo: graph-algorithm-engine (standalone, self-contained project)

## How This File Is Used

This file is the MASTER SPECIFICATION. You will be given this file at the
start of every session along with:
  - `.agent/spec.md` — the step-by-step learning guide (read the relevant
    step for context, acceptance criteria, and what NOT to build yet)
  - `.agent/sessions/sNN.md` — the specific task for this session only

Always read all three files before generating any code.
Always implement ONLY what the session file specifies — nothing more.

---

## CRITICAL: Read This First

This is a **standalone repository** — a completely independent C++ project.
Build it as a fresh project from scratch.

Do NOT reference, import, or depend on any other codebase or repository.
Everything needed for this project is specified in this file.

---

## Context & Goal

You are helping build a **production-quality, portfolio-grade Graph Algorithm Engine in C++** for a 2nd year IIT student targeting SWE/quant internships. This is not a toy project — it should be structured like real open-source software with clean architecture, proper separation of concerns, and extensibility.

The student will **read, understand, and extend** your code — so every non-obvious design decision must have a comment explaining WHY, not just WHAT.

---

## Project Overview

Build a **generic, templated C++ graph engine** with:
- A reusable `Graph<NodeType, WeightType>` class using adjacency lists
- 7 pluggable graph algorithms
- Real OpenStreetMap data loader
- SFML-based visualizer
- JSON exporter for web-based D3.js visualizer
- Google Test unit test suite
- Performance benchmarks

---

## Exact Folder Structure to Generate

```
graph-engine/
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── Graph.hpp
│   ├── algorithms/
│   │   ├── BFS.hpp
│   │   ├── DFS.hpp
│   │   ├── Dijkstra.hpp
│   │   ├── AStar.hpp
│   │   ├── BellmanFord.hpp
│   │   ├── Kruskal.hpp
│   │   ├── TarjanSCC.hpp
│   │   └── TopologicalSort.hpp
│   └── utils/
│       ├── GraphLoader.hpp
│       ├── GraphExporter.hpp
│       └── Timer.hpp
│
├── src/
│   └── main.cpp
│
├── data/
│   ├── sample_graph.csv
│   └── road_network_sample.csv
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_graph.cpp
│   ├── test_bfs_dfs.cpp
│   ├── test_dijkstra.cpp
│   ├── test_astar.cpp
│   ├── test_mst.cpp
│   └── test_scc.cpp
│
├── benchmarks/
│   └── benchmark.cpp
│
└── visualizer/
    ├── index.html
    ├── graph.js
    └── style.css
```

---

## File-by-File Specification

---

### 1. `CMakeLists.txt` (Root)

```cmake
cmake_minimum_required(VERSION 3.15)
project(GraphEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
add_compile_options(-Wall -Wextra -O2)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Main executable
add_executable(graph_engine src/main.cpp)
target_include_directories(graph_engine PRIVATE include)
target_link_libraries(graph_engine sfml-graphics sfml-window sfml-system)

# Tests (Google Test) — see tests/CMakeLists.txt for how gtest itself is fetched
enable_testing()
add_subdirectory(tests)

# Benchmarks
add_executable(benchmark benchmarks/benchmark.cpp)
target_include_directories(benchmark PRIVATE include)
```

---

### 1b. `tests/CMakeLists.txt`

#### Why This File Needs Its Own FetchContent Block
GoogleTest is NOT assumed to be installed on the machine. Just like SFML
is `find_package`'d above, GoogleTest is fetched and built from source via
CMake's `FetchContent` — this is the standard modern-CMake way to depend on
gtest without requiring `apt install libgtest-dev` first.

```cmake
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Windows: force shared CRT linking to match gtest's defaults
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# One add_executable per test file, each linked against gtest_main
foreach(test_file
    test_graph
    test_bfs_dfs
    test_dijkstra
    test_astar
    test_mst
    test_scc
)
    add_executable(${test_file} ${test_file}.cpp)
    target_include_directories(${test_file} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_file} GTest::gtest_main)
    add_test(NAME ${test_file} COMMAND ${test_file})
endforeach()
```

With this, `ctest` (run from the `build/` directory) discovers and runs
every test binary automatically — no manual registration needed beyond
adding the filename to the `foreach` list above when a new test file is added.

---

### 2. `include/Graph.hpp`

Requirements:
- Templated on `<typename NodeType, typename WeightType = int>`
- Internal `struct Edge { NodeType to; WeightType weight; }`
- `unordered_map<NodeType, vector<Edge>>` as core adjacency list
- Boolean flag: `directed`
- Methods:
  - `void addNode(NodeType u)`
  - `void addEdge(NodeType u, NodeType v, WeightType w = 1)`
  - `void removeEdge(NodeType u, NodeType v)`
  - `bool hasNode(NodeType u) const`
  - `bool hasEdge(NodeType u, NodeType v) const`
  - `const vector<Edge>& neighbors(NodeType u) const`
  - `int nodeCount() const`
  - `int edgeCount() const`
  - `vector<NodeType> getAllNodes() const`
  - `begin() / end()` iterators for range-based for loops
- Every method must have a detailed comment explaining:
  - What it does
  - Time complexity
  - Any edge cases handled

---

### 3. `include/algorithms/BFS.hpp`

Requirements:
- Free function template: `bfs(Graph&, NodeType start) → vector<NodeType>`
- Also implement: `bfsShortestPath(Graph&, NodeType src, NodeType dst) → vector<NodeType>`
  - Returns actual path, empty vector if unreachable
- Uses `queue<NodeType>` and `unordered_set<NodeType>` for visited tracking
- Comments must explain WHY queue gives level-order traversal
- Comment explaining why this finds shortest path in UNWEIGHTED graphs only

---

### 4. `include/algorithms/DFS.hpp`

Requirements:
- Recursive version: `dfs(Graph&, NodeType start) → vector<NodeType>`
- Iterative version: `dfsIterative(Graph&, NodeType start) → vector<NodeType>`
- `hasCycle(Graph&) → bool` — using DFS cycle detection
- `isConnected(Graph&) → bool` — checks if all nodes reachable from any start
- Comments must explain the recursion stack and backtracking behavior
- Comment explaining difference between recursive and iterative DFS output order

---

### 5. `include/algorithms/Dijkstra.hpp`

Requirements:
- `struct DijkstraResult { unordered_map<NodeType, WeightType> dist; unordered_map<NodeType, NodeType> prev; }`
- `dijkstra(Graph&, NodeType src) → DijkstraResult`
- `reconstructPath(DijkstraResult&, NodeType src, NodeType dst) → vector<NodeType>`
- Uses `priority_queue` with `greater<>` comparator (min-heap)
- Must include the "stale entry" optimization: `if (d > dist[u]) continue;`
- Comments must explain:
  - WHY greater<> makes it a min-heap
  - WHY stale entries appear and how we handle them
  - WHY this fails with negative weights
  - Time complexity derivation: O(E log V)

---

### 6. `include/algorithms/AStar.hpp`

Requirements:
- Heuristic as `std::function<double(NodeType, NodeType)>` parameter
- `astar(Graph&, NodeType src, NodeType dst, Heuristic h) → vector<NodeType>`
- Provide built-in heuristics as lambdas in the header:
  - `manhattanHeuristic` for `pair<int,int>` node type
  - `euclideanHeuristic` for `pair<double,double>` node type
  - `zeroHeuristic` (degenerates to Dijkstra)
- Track and return `nodesVisited` count alongside path (for benchmarking vs Dijkstra)
- Comments must explain:
  - The f(n) = g(n) + h(n) formula in detail
  - What admissible heuristic means and WHY it guarantees optimality
  - Why Manhattan is admissible for 4-directional grid movement

---

### 7. `include/algorithms/BellmanFord.hpp`

Requirements:
- `bellmanFord(Graph&, NodeType src) → unordered_map<NodeType, WeightType>`
- Must detect negative cycles and throw `std::runtime_error("Negative cycle detected")`
- Implement V-1 relaxation passes with clear loop structure
- Comments must explain:
  - WHY V-1 iterations are sufficient for shortest path
  - WHY the V-th iteration detects negative cycles
  - Comparison with Dijkstra: when to use each

---

### 8. `include/algorithms/Kruskal.hpp`

Requirements:
- Implement `struct DSU` (Disjoint Set Union) with:
  - `makeSet(NodeType x)`
  - `find(NodeType x)` — with path compression
  - `unite(NodeType x, NodeType y)` — with union by rank
  - Comment explaining why path compression gives near O(1) amortized
- `kruskal(Graph&) → vector<tuple<WeightType, NodeType, NodeType>>`
  - Returns MST edges as {weight, u, v}
- Comments must explain:
  - Why sorting edges by weight is the greedy choice
  - Why DSU detects cycles efficiently
  - Time complexity: O(E log E)

---

### 9. `include/algorithms/TarjanSCC.hpp`

Requirements:
- Class-based implementation `class TarjanSCC<NodeType>`
- Private: `disc`, `low`, `onStack` maps, `stack<NodeType>`, `timer`
- Public: `findSCC(Graph&) → vector<vector<NodeType>>`
- Comments must explain:
  - What disc[] and low[] represent
  - Why a node is an SCC root when low[u] == disc[u]
  - The role of the onStack set
  - Why this runs in O(V + E)

---

### 10. `include/algorithms/TopologicalSort.hpp`

Requirements:
- Kahn's algorithm (BFS-based): `topologicalSort(Graph&) → vector<NodeType>`
- DFS-based version: `topologicalSortDFS(Graph&) → vector<NodeType>`
- Both must detect cycles and throw if graph is not a DAG
- Comments must explain:
  - What in-degree means and why 0-in-degree nodes go first
  - How cycle detection works in Kahn's (result size < nodeCount)
  - Difference between Kahn's and DFS-based output

---

### 11. `include/utils/GraphLoader.hpp`

Requirements:
- `loadFromCSV(filename, Graph&)` — parse edge list: `u,v,weight` per line
- `loadFromAdjacencyList(filename, Graph&)` — parse adjacency list format
- `loadFromRoadNetworkCSV(filename, Graph&)` — parse a simplified geographic
  road-network CSV (node_id,lat,lon and edge u,v,weight sections)
- Error handling: throw descriptive exceptions for malformed files
- Comments explaining each file format with example

---

### 12. `include/utils/GraphExporter.hpp`

Requirements:
- `exportToJSON(Graph&, filename)` — D3.js compatible format:
```json
{
  "nodes": [{"id": 1}, {"id": 2}],
  "edges": [{"source": 1, "target": 2, "weight": 4}]
}
```
- `exportAlgorithmSteps(vector<NodeType> visitOrder, filename)` — for animation
- `exportToCSV(Graph&, filename)`

---

### 13. `include/utils/Timer.hpp`

Requirements:
- Simple RAII timer class using `std::chrono`
- `Timer t("Dijkstra")` → prints elapsed time on destruction
- Also provide `elapsed_ms()` method for programmatic access

---

### 14. `src/main.cpp`

Requirements:
- Demonstrate ALL algorithms with a sample graph
- Load from `data/sample_graph.csv`
- Run each algorithm, print results
- Run A* vs Dijkstra comparison showing nodes visited
- Export results to JSON for visualizer
- Use `Timer` to benchmark each algorithm
- Well-commented main showing how to USE the engine as a library

---

### 15. `data/sample_graph.csv`

Generate a meaningful sample with 10-15 nodes, weights, demonstrating:
- Multiple connected components (for SCC)
- Negative weight edge (for Bellman-Ford demo)
- A clear shortest path that A* would find efficiently

---

### 16. `tests/test_*.cpp`

For EACH algorithm test file, include:
- Empty graph edge case
- Single node graph
- Disconnected graph
- Known correct output verification
- Performance test (large graph)

Use Google Test framework (`TEST(SuiteName, TestName) { EXPECT_EQ(...) }`).

---

### 17. `benchmarks/benchmark.cpp`

Requirements:
- Generate random graphs of sizes: 100, 1000, 10000, 100000 nodes
- Benchmark each algorithm at each size
- Compare:
  - Dijkstra vs A* (nodes visited + time)
  - BFS vs DFS (memory usage + time)
  - Kruskal on dense vs sparse graphs
- Output clean table:
```
Algorithm    | Nodes  | Edges   | Time(ms) | Notes
-------------|--------|---------|----------|-------
Dijkstra     | 10000  | 50000   | 45.2     |
A*           | 10000  | 50000   | 3.8      | 12x fewer nodes visited
```

---

### 18. `visualizer/index.html` + `graph.js`

Requirements:
- Load graph JSON exported by `GraphExporter`
- D3.js force-directed graph layout
- Color nodes by algorithm visit order (animation)
- Highlight shortest path in red
- Controls:
  - Dropdown to select algorithm
  - Input for source/destination nodes
  - Speed slider for animation
  - Button to step through algorithm one node at a time

---

### 19. `README.md`

Requirements:
- Project description and motivation
- Prerequisites: C++17, CMake, SFML, Google Test
- Build instructions (CMake commands)
- Usage examples with code snippets
- Algorithm complexity table
- Screenshots of visualizer
- Section: "What I Learned" — for CV/interview context

---

## Code Quality Requirements

### Every file must have:
```cpp
/**
 * @file Dijkstra.hpp
 * @brief Dijkstra's shortest path algorithm
 *
 * Finds shortest paths from a source node to all other nodes
 * in a weighted graph with non-negative edge weights.
 *
 * Time Complexity: O(E log V)
 * Space Complexity: O(V)
 *
 * WHY USE THIS: When you have non-negative weights and need
 * shortest path. Faster than Bellman-Ford for most cases.
 * Use Bellman-Ford when negative weights exist.
 */
```

### Every non-trivial code block must have WHY comments:
```cpp
// WHY greater<>: By default priority_queue is a max-heap.
// We need min-heap (always process cheapest node first).
// greater<> reverses the comparison, giving us min-heap behavior.
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
```

### Every function must document:
- Parameters and their meaning
- Return value
- Time and space complexity
- Preconditions (e.g., "Graph must be a DAG")
- Example usage

---

## Strict Requirements

1. **C++17** — use structured bindings (`auto& [u, v]`), `std::optional`, `if constexpr`
2. **No raw owning pointers** — use RAII, smart pointers where needed
3. **const correctness** — all read-only methods marked `const`
4. **No using namespace std** in headers — always qualify `std::`
5. **Template definitions in headers** — no separate .cpp for templates
6. **Compile cleanly** with `-Wall -Wextra -O2` — zero warnings

---

## What NOT to Generate

- Do NOT use global variables
- Do NOT use `#define` for constants — use `constexpr`
- Do NOT write spaghetti main.cpp — keep it clean and demonstrative
- Do NOT skip error handling — every file I/O must handle exceptions
- Do NOT write one giant class — keep algorithms as free functions/separate classes

---

## Final Deliverable Check

Before finishing, verify:
- [ ] All 7 algorithms implemented and tested
- [ ] CMakeLists.txt builds cleanly
- [ ] `ctest` (from the build directory) discovers and runs all 6 test binaries
- [ ] Sample CSV data provided
- [ ] README has build instructions
- [ ] Every algorithm has at least 3 unit tests
- [ ] Benchmark compares at least 3 algorithm pairs
- [ ] Visualizer loads and animates correctly
- [ ] Zero compiler warnings with `-Wall -Wextra`
