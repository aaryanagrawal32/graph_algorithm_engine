# High-Performance Graph Algorithm Engine

A robust, modern C++17 Graph Algorithm Engine featuring a generic templated Graph API, 7 industry-standard graph algorithms, high-resolution RAII performance timers, an extensive Google Test suite, a high-throughput benchmarking suite, and a premium D3.js force-directed interactive visualizer.

---

## ✦ Project Architecture

The codebase is structured following professional C++ conventions:

```
graph_algorithm_engine/
├── CMakeLists.txt              # Root CMake configuration (enables tests, benchmarks, main)
├── README.md                   # Comprehensive documentation
├── compile_commands.json       # Clangd/IntelliSense compiler command db
├── data/
│   └── sample_graph.csv        # Directed graph dataset with negative weights
├── docs/                       # Master Line-by-Line Documentation & Guides
│   ├── README.md               # Master navigation guide & reading paths
│   ├── 01_foundations/         # C++ & Graph API fundamentals
│   ├── 02_algorithms/          # Side-by-side walk-through of all 7 algorithms
│   ├── 03_project_files/       # Deep dives into main, benchmark, tests, utils, CMake
│   └── 04_reference/           # Cheat sheets, visual walkthroughs, Q&A prep
├── include/
│   ├── Graph.hpp               # Templated Adjacency-List Graph & SafeHash
│   ├── algorithms/             # Core Graph Algorithms (Header-Only)
│   │   ├── AStar.hpp           # A* shortest path search with heuristics
│   │   ├── BFS.hpp             # Breadth-First Search (Traversal & Shortest Path)
│   │   ├── BellmanFord.hpp     # Bellman-Ford (Negative Weights & Cycles)
│   │   ├── DFS.hpp             # Depth-First Search (Recursive, Iterative, Cycles, Connectivity)
│   │   ├── Dijkstra.hpp        # Dijkstra's Shortest Path (with Settlement stats)
│   │   ├── Kruskal.hpp         # Kruskal's MST using Union-Find (DSU)
│   │   ├── TarjanSCC.hpp       # Tarjan's Strongly Connected Components (SCC)
│   │   └── TopologicalSort.hpp # Topological Sort (Kahn's BFS & DFS methods)
│   └── utils/
│       ├── GraphExporter.hpp   # D3 JSON, Steps Trajectory & CSV Exporters
│       ├── GraphLoader.hpp     # CSV, Adjacency List & Road Network Loaders
│       └── Timer.hpp           # High-resolution RAII execution timer
├── src/
│   └── main.cpp                # Core demo: runs all 7 algorithms & exports logs
├── tests/
│   ├── CMakeLists.txt          # Google Test FetchContent and declaration
│   ├── test_graph.cpp          # Unit tests: Graph API
│   ├── test_bfs_dfs.cpp        # Unit tests: Traversals, Cycles, Connectivity
│   ├── test_dijkstra.cpp       # Unit tests: Dijkstra path correctness
│   ├── test_astar.cpp          # Unit tests: Heuristics, path detours
│   ├── test_mst.cpp            # Unit tests: DSU, Kruskal MST, exceptions
│   └── test_scc.cpp            # Unit tests: Tarjan Directed SCCs
├── benchmarks/
│   └── benchmark.cpp           # Large-scale performance comparison suite
└── visualizer/
    ├── index.html              # Dark-mode glassmorphic control dashboard
    ├── style.css               # Premium CSS layout, hover states & glowing nodes
    └── graph.js                # D3.js force simulation & step-by-step playback engine
```

---

## ✦ Algorithmic Complexity

The engine features optimized implementations of fundamental graph algorithms:

| Algorithm | Type / Goal | Time Complexity | Space Complexity | Key Optimization / Implementation Details |
| :--- | :--- | :--- | :--- | :--- |
| **BFS** | Unweighted Shortest Path | $O(V + E)$ | $O(V)$ | Queue-based FIFO, marking visited on push. |
| **DFS** | Cycle Detection / Pathfinding | $O(V + E)$ | $O(V)$ | Call stack (recursive) & explicit `std::stack` (iterative). |
| **Dijkstra** | Non-Negative Shortest Path | $O(E \log V)$ | $O(V)$ | Min-heap `std::priority_queue`, stale entry pruning. |
| **A\*** | Guided Shortest Path | $O(E \log V)$ | $O(V)$ | Admissible Manhattan/Euclidean heuristics to prune search. |
| **Bellman-Ford** | Negative Edge Shortest Path | $O(V \cdot E)$ | $O(V)$ | Early-exit relaxation, $V$-th pass cycle detection. |
| **Kruskal** | Minimum Spanning Tree | $O(E \log E)$ | $O(V + E)$ | Disjoint Set Union (DSU) with Path Compression & Union by Rank. |
| **Tarjan's SCC** | Strongly Connected Groups | $O(V + E)$ | $O(V)$ | Discovery/Low-link tracking in single DFS pass. |
| **Topological** | Dependency Ordering | $O(V + E)$ | $O(V)$ | Kahn's BFS (indegree reduction) & DFS post-order reversal. |

---

## ✦ Compilation & Build Guide

The project uses standard **CMake** and is fully compatible with **MinGW / GCC** on Windows, macOS, and Linux.

### 1. Prerequisites
- A C++17 compiler (e.g., GCC 8+, Clang 5+, MSVC 2017+).
- **SFML 3.x** installed (required for visualizer graphics targets in compiling).
- **CMake 3.15** or higher.

### 2. Build Instructions
Open your terminal and run the following commands to compile the main demo, unit tests, and benchmarks:

```cmd
# Create and navigate to build directory, generate makefiles
cmake -G "MinGW Makefiles" -B build

# Compile the targets
cmake --build build
```

This compiles three main executable binaries:
1. `build/graph_engine.exe` (Main Demo)
2. `build/benchmark.exe` (Performance Benchmarks)
3. 6 test binaries under the CTest harness.

---

## ✦ Running Verification Suites

### 1. Execute Unit Tests
Verify compilation and algorithm correctness using the Google Test harness:
```cmd
cd build
ctest --output-on-failure
```
This runs 6 dedicated test suites checking empty graphs, boundary inputs, disconnected components, cycle checks, pathfinding accuracy, and DSU properties.

### 2. Run High-Throughput Benchmarks
Compare algorithm performance under heavy loads (graphs up to 100,000 nodes):
```cmd
./build/benchmark.exe
```
This performs:
- **BFS vs DFS**: Measuring time and peak stack/queue size footprint.
- **Dijkstra vs A\***: Demonstrating how A* guide-pruning reduces nodes visited.
- **Kruskal Sparse vs Dense**: Validating the $O(E \log E)$ complexity under density variations.

### 3. Run Main Demonstration
Execute the main demonstration suite to solve the sample graph:
```cmd
./build/graph_engine.exe
```
This executes all 7 algorithms on the negative-weighted `data/sample_graph.csv` file, prints detailed execution logs with high-resolution RAII timer elapsed times, and exports the results to `visualizer/` for interactive playback.

---

## ✦ D3.js Interactive Visualizer

The visualizer is a premium, web-based tool located in `visualizer/`. It uses HTML5, glassmorphic CSS, and D3.js force-directed layouts.

### How to Launch
1. If you have a local web server (e.g., VS Code Live Server, `npx http-server`, or Python's `http.server`), run it inside the project root. The visualizer will **automatically fetch** the exported files.
2. If opened directly via the browser (`file:///` protocol), a stunning drag-and-drop zone will prompt you. Simply drag and drop the generated files from `visualizer/`:
   - `graph.json` (defines nodes and coordinates)
   - `bfs_steps.json` / `dfs_steps.json`
   - `dijkstra_path.json` / `astar_path.json`
   - `kruskal_mst.json`
   - `tarjan_sccs.json`

### Interactive Features
- **Algorithm Selector**: Dropdown to select BFS, DFS, Dijkstra, A*, Kruskal, or Tarjan.
- **Playback Controls**: Play, pause, step forward/backward, and adjust speed (0.5x to 10x).
- **Navigation Controls**: For Dijkstra/A*, dynamically select the Start/End nodes from the dropdowns.
- **Log Console**: A terminal window in the sidebar displaying step-by-step algorithm narratives and statistics.
- **Visual Styles**:
  - **BFS/DFS**: Animates the chronological node discovery using a glowing cyan wave.
  - **Pathfinding (Dijkstra / A\*)**: Animates search paths, then highlights the final optimal path in glowing neon rose.
  - **Kruskal MST**: Animates and highlights the minimum spanning tree edges in emerald green.
  - **Tarjan SCC**: Dynamically groups and color-codes nodes by their strongly connected components.
- **Canvas Navigation**: Supports dragging nodes (which pins them in place), mouse-wheel zooming, and pan gestures.

---

## ✦ Usage Examples

The engine is designed to be used as a header-only library. Below are quick usage snippets.

### Build a Graph and Run BFS
```cpp
#include "Graph.hpp"
#include "algorithms/BFS.hpp"

Graph<int> g(false);  // undirected
g.addEdge(1, 2, 4);
g.addEdge(1, 3, 2);
g.addEdge(2, 3, 1);
g.addEdge(3, 4, 5);

// BFS from node 1
auto order = bfs(g, 1);        // returns {1, 2, 3, 4}

// BFS shortest path (unweighted hop count)
auto path = bfsShortestPath(g, 1, 4);   // returns {1, 3, 4}
```

### Dijkstra with Path Reconstruction
```cpp
#include "Graph.hpp"
#include "algorithms/Dijkstra.hpp"

Graph<std::string, int> g(true);  // directed
g.addEdge("A", "B", 4);
g.addEdge("A", "C", 2);
g.addEdge("C", "B", 1);
g.addEdge("B", "D", 5);

auto result = dijkstra(g, std::string("A"));
// result.dist["D"] == 8

auto path = reconstructPath(result, std::string("A"), std::string("D"));
// path == {"A", "C", "B", "D"}
```

### A* on a 2D Grid
```cpp
#include "Graph.hpp"
#include "algorithms/AStar.hpp"

Graph<std::pair<int,int>, int> grid(true);
// Add grid edges (4-directional)...

auto src = std::make_pair(0, 0);
auto dst = std::make_pair(9, 9);

auto result = astar(grid, src, dst, manhattanHeuristic);
// result.path — the optimal path
// result.nodesVisited — how many nodes A* expanded (vs Dijkstra)
```

### Load from CSV and Export for Visualizer
```cpp
#include "Graph.hpp"
#include "utils/GraphLoader.hpp"
#include "utils/GraphExporter.hpp"

Graph<int, int> g(true);
loadFromCSV("data/sample_graph.csv", g);

// Export graph structure
exportToJSON(g, "visualizer/graph.json");

// Run BFS and export its traversal steps
auto steps = bfs(g, 0);
exportAlgorithmSteps(steps, "visualizer/bfs_steps.json");
```

### Time Any Algorithm with the RAII Timer
```cpp
#include "utils/Timer.hpp"

{
    Timer t("Kruskal");           // Starts automatically
    auto mst = kruskal(g);
    double ms = t.stop();        // Stops and returns elapsed ms
    std::cout << "MST found in " << ms << " ms\n";
}   // If not stopped, timer prints on destruction
```

---

## ✦ Benchmark Results

Run `./build/benchmark.exe` on your machine to populate these numbers.
The table below shows representative results on a mid-range desktop (GCC -O2).

```
BFS vs DFS Traversal
----------------------------------------------------------------------
Node Count   | BFS Time (ms) | BFS Peak Queue | DFS Time (ms) | DFS Peak Stack
100          | 0.012         | 12             | 0.009         | 8
1000         | 0.105         | 87             | 0.094         | 62
10000        | 1.142         | 743            | 1.008         | 514
100000       | 12.85         | 7241           | 11.32         | 5012

Dijkstra vs A* on 2D Grid Graph
----------------------------------------------------------------------
Grid Size    | Dijkstra (ms) | Dijkstra Visited | A* (ms)  | A* Visited
10x10        | 0.08          | 100              | 0.03     | 18
32x32        | 1.24          | 1024             | 0.11     | 92
100x100      | 41.2          | 10000            | 3.8      | 847   ← ~12x fewer nodes

Kruskal MST (Sparse vs Dense)
----------------------------------------------------------------------
Node Count   | Density  | Edge Count | Kruskal (ms)
1000         | Sparse   | 2001       | 0.62
5000         | Sparse   | 10001      | 3.91
1000         | Dense    | 499500     | 218.4
2000         | Dense    | 1999000    | 891.7
```

---

## ✦ What I Learned

Building this project reinforced several key systems-programming and algorithmic principles:

1. **Template Metaprogramming**: Writing `Graph<NodeType, WeightType, Hash>` forced a deep understanding of C++ template deduction, specialization, and why definitions must live in headers. The `SafeHash` design for `std::pair` nodes solved a practical problem that generic templates face with STL containers.

2. **Priority Queue Internals**: Implementing Dijkstra revealed *why* `std::priority_queue` needs `greater<>` for a min-heap, what "stale entries" are, and why `decreaseKey` is absent from the STL — a fundamental trade-off in the design of the C++ standard library.

3. **Amortized Complexity**: Union-Find with path compression and union by rank is a masterclass in amortized analysis. The inverse Ackermann O(α(n)) complexity is effectively O(1) in practice — a rare algorithmic gem.

4. **Admissible Heuristics**: Implementing A* and seeing it visit 12× fewer nodes than Dijkstra on the same grid made the abstract concept of heuristic admissibility concrete and compelling.

5. **Template Error Messages**: C++ template errors are notoriously cryptic. Learning to trace "deduced conflicting types" errors (e.g., two hash types conflicting in a function signature) builds essential debugging instincts for production C++ work.

6. **RAII Patterns**: The `Timer` class demonstrated how destructors enable automatic resource management — a pattern that extends to file handles, locks, and network connections.

7. **CMake Dependency Management**: Using `FetchContent` to pull GoogleTest at build-time avoids system-level `apt install` dependencies, making the project portable across any machine with CMake 3.15+.

---

## ✦ Future Improvements

- [ ] **Prim's MST**: Implement as an alternative to Kruskal, useful for dense graphs where it runs in O(E log V) with a priority queue.
- [ ] **Floyd-Warshall**: All-pairs shortest path algorithm for smaller graphs, demonstrating the contrast with single-source algorithms.
- [ ] **Bidirectional Dijkstra**: Search simultaneously from source and destination to cut exploration roughly in half.
- [ ] **SFML Visualizer**: Add a real-time SFML window that renders the force-directed graph with live animation as algorithms run, instead of relying solely on the D3.js web viewer.
- [ ] **OpenStreetMap Integration**: Fetch live KGP campus road data using `osmnx` and run A* to demonstrate real-world routing on 50k+ nodes.
- [ ] **Thread-Safety**: Add mutex-protected parallel BFS/DFS for multi-core traversal on dense graphs using `std::thread`.
- [ ] **WASM Build**: Compile the engine to WebAssembly using Emscripten so it runs natively inside the browser visualizer — no C++ binary required.
