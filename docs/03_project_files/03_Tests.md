# Tests — Understanding How the Code Is Verified
### A Beginner-Friendly Guide to All 6 Test Files

> **Who is this for?**
> This guide is for someone who has never written automated tests before. We start from scratch — explaining what testing is, why we do it, and how Google Test works — before looking at a single line of test code.

---

## What Are Unit Tests, and Why Do We Need Them?

Imagine you wrote a Dijkstra function. How do you know it gives the right answer?

**Option A (without tests):** Run the program, look at the console output, and think "that looks about right." This works once, but:
- What if you change something later and introduce a bug?
- What if the graph is complex and you can't easily verify by eye?
- What if a teammate changes the code and doesn't know what the expected output should be?

**Option B (with unit tests):** Write a small program that creates a tiny, hand-crafted graph, runs Dijkstra on it, and automatically checks that the result matches the hand-computed correct answer.

```
Hand-drawn graph → Run algorithm → Compare output to known correct answer
```

If the algorithm is later changed and breaks, the test catches it immediately with a clear failure message.

> **"Unit" testing** means testing one small unit of code at a time (one function, one class) in isolation.

---

## What Is Google Test?

Google Test (often called `gtest`) is a **C++ testing framework** — a library that provides:
- A way to declare tests and group them
- Assertion macros to check expected vs actual values
- A test runner that executes all tests and reports results

Without Google Test, you'd have to write your own `if (result != expected) { print("FAIL"); }` checks everywhere. Google Test does this automatically and gives you beautiful output.

---

## The Core Concepts of Google Test

### Declaring a Test: `TEST(SuiteName, TestName)`

```cpp
TEST(GraphTest, EmptyGraph) {
    // ... test body
}
```

- **`SuiteName`** (`GraphTest`) — groups related tests together (like a chapter title)
- **`TestName`** (`EmptyGraph`) — names the specific scenario being tested
- The body is a normal C++ function body — you create objects, call functions, check results

### Assertion Macros — The Checking Tools

These macros check a condition and report failure if it's not met:

| Macro | What It Checks | Behaviour on Failure |
|---|---|---|
| `EXPECT_EQ(a, b)` | `a == b` (equal) | Marks test failed, continues running |
| `EXPECT_NE(a, b)` | `a != b` (not equal) | Marks test failed, continues running |
| `EXPECT_TRUE(expr)` | `expr` is true | Marks test failed, continues running |
| `EXPECT_FALSE(expr)` | `expr` is false | Marks test failed, continues running |
| `EXPECT_LT(a, b)` | `a < b` (less than) | Marks test failed, continues running |
| `ASSERT_EQ(a, b)` | `a == b` (equal) | **Stops the test immediately** if failed |
| `EXPECT_THROW(code, ExceptionType)` | `code` throws that exception | Marks test failed if no throw |

> **EXPECT vs ASSERT — when to use which:**
> - `EXPECT_` continues running after a failure, collecting multiple errors
> - `ASSERT_` stops immediately
>
> Use `ASSERT_` when a failure makes the remaining checks meaningless. For example: if `ASSERT_EQ(path.size(), 3)` fails, accessing `path[0]` could crash. So stop early with ASSERT.

---
---

# File 1: `test_graph.cpp` — Testing the Graph Data Structure

These tests verify that the `Graph` class itself works correctly, before testing any algorithms.

## Test 1: EmptyGraph — The Simplest Possible Case

```cpp
TEST(GraphTest, EmptyGraph) {
    Graph<int> g;
    EXPECT_EQ(g.nodeCount(), 0);
    EXPECT_EQ(g.edgeCount(), 0);
    EXPECT_FALSE(g.hasNode(1));
}
```

**Why test an empty graph?** Many bugs only appear at boundaries and edge cases. Questions like "does `nodeCount()` return `0` or throw an exception?" and "does `hasNode(1)` return `false` or access invalid memory?" are only answered by actually testing them.

We create a default `Graph<int>` and check that it starts completely empty.

## Test 2: SingleNode — One Node, No Edges

```cpp
TEST(GraphTest, SingleNode) {
    Graph<int> g;
    g.addNode(1);
    EXPECT_EQ(g.nodeCount(), 1);
    EXPECT_TRUE(g.hasNode(1));
    EXPECT_EQ(g.neighbors(1).size(), 0);
}
```

A single isolated node with no connections. The key check is `g.neighbors(1).size() == 0` — does asking for the neighbors of a node with no edges return an empty list, or does it crash?

## Test 3: DisconnectedGraph — Two Nodes, No Edges Between Them

```cpp
TEST(GraphTest, DisconnectedGraph) {
    Graph<int> g;
    g.addNode(1);
    g.addNode(2);
    EXPECT_EQ(g.nodeCount(), 2);
    EXPECT_EQ(g.edgeCount(), 0);
    EXPECT_FALSE(g.hasEdge(1, 2));
}
```

Verifies that adding nodes doesn't automatically create edges between them.

## Test 4: AddRemoveEdges — The Full Lifecycle

```cpp
TEST(GraphTest, AddRemoveEdges) {
    Graph<std::string, double> g(true);  // directed, double weights
    g.addEdge("A", "B", 1.5);
    // ...
    g.removeEdge("A", "B");
    EXPECT_FALSE(g.hasEdge("A", "B"));
    EXPECT_EQ(g.edgeCount(), 2);
}
```

Two important things happening here:
1. **String nodes and double weights** — the template works for non-integer types too
2. **Full lifecycle** — add an edge, verify it exists, remove it, verify it's gone

## Test 5: PerformanceLargeGraph — Correctness at Scale

```cpp
TEST(GraphTest, PerformanceLargeGraph) {
    Graph<int> g(false);  // undirected
    int N = 1000;
    for (int i = 0; i < N; ++i) { g.addNode(i); }
    for (int i = 0; i < N - 1; ++i) { g.addEdge(i, i + 1, 1); }
    EXPECT_EQ(g.nodeCount(), 1000);
    EXPECT_EQ(g.edgeCount(), 999);
}
```

> **This is NOT a speed test** — it's a correctness test at scale. Adding 1,000 nodes and 999 edges and verifying the counts helps catch bugs that only appear with many elements, like hash collisions or off-by-one errors in counters.

---
---

# File 2: `test_bfs_dfs.cpp` — Testing BFS and DFS

## Test 1: BFSTraversal — Level-by-Level Order

```cpp
// Graph: 0→1, 0→2, 1→3, 2→3
auto order = bfs(g, 0);
EXPECT_EQ(order.size(), 4);
EXPECT_EQ(order[0], 0);  // Source always first
```

BFS visits nodes **level by level** — all direct neighbors of 0 before any neighbors of those neighbors. We verify the traversal order reflects this.

## Test 2: BFSShortestPath — Fewest Hops

```cpp
auto path = bfsShortestPath(g, 0, 3);
ASSERT_EQ(path.size(), 3);  // Using ASSERT — if wrong size, accessing path[0] might crash
EXPECT_EQ(path[0], 0);      // Must start at source
EXPECT_EQ(path.back(), 3);  // Must end at destination
```

BFS finds the shortest path in terms of **number of edges** (hops), not weight. We verify the path starts at the source and ends at the destination.

> **`path.back()`** returns the last element of a vector — equivalent to `path[path.size()-1]` but safer and more readable.

> **Why `ASSERT_EQ` for size, but `EXPECT_EQ` for elements?**
> If `path.size()` is not 3, then `path[0]` might be accessing beyond the vector's bounds (undefined behavior, potential crash). So we ASSERT first — if size is wrong, stop immediately. The element checks are only safe after confirming the size.

## Test 3: BFSUnreachable — The Dead End Case

```cpp
g.addNode(10);  // Add an isolated node with no edges
auto path = bfsShortestPath(g, 0, 10);
EXPECT_TRUE(path.empty());  // Node 10 is unreachable from 0
```

When the destination is unreachable, `bfsShortestPath` must return an **empty vector**, not crash or return partial data.

## Test 4: DFSCycleDetection — Finding Loops

```cpp
TEST(DFSTest, CycleDetectionDirected) {
    Graph<int> directed(true);
    directed.addEdge(0, 1);
    directed.addEdge(1, 2);
    directed.addEdge(2, 0);  // Creates cycle: 0→1→2→0
    EXPECT_TRUE(hasCycle(directed));
}
```

A **cycle** is a path that loops back to where it started (like A→B→C→A). DFS detects cycles using 3-color marking:
- White (0) = not yet visited
- Gray (1) = currently being visited (on the current DFS path)
- Black (2) = fully processed

If DFS finds a gray node while exploring, it means we've looped back to a node already on our current path — that's a cycle.

## Test 5: ConnectivityCheck

```cpp
EXPECT_TRUE(isConnected(connected));
EXPECT_FALSE(isConnected(disconnected));
```

`isConnected` does BFS from one node and checks if ALL nodes are reachable. We test both a connected graph (should return `true`) and a disconnected graph (should return `false`).

---
---

# File 3: `test_dijkstra.cpp` — Testing Dijkstra's Algorithm

## Test 1: EmptyGraph — No Nodes

```cpp
auto res = dijkstra(g, 1);
EXPECT_TRUE(res.dist.empty());
EXPECT_TRUE(reconstructPath(res, 1, 2).empty());
```

When the source node doesn't exist in the graph, Dijkstra should return an empty result. `reconstructPath` on an empty result should return an empty vector, not crash.

## Test 2: SingleNode — Source Equals Destination

```cpp
auto res = dijkstra(g, 1);
EXPECT_EQ(res.dist.size(), 1);
EXPECT_EQ(res.dist[1], 0.0);   // Distance to yourself is always 0

auto path = reconstructPath(res, 1, 1);  // Start and end at the same node
ASSERT_EQ(path.size(), 1);
EXPECT_EQ(path[0], 1);  // Path to self = [self]
```

Special case: when source and destination are the same node, the path should be a list containing just that one node, and the distance should be 0.

## Test 3: DisconnectedGraph — Unreachable Node

```cpp
auto res = dijkstra(g, 1);
double INF = std::numeric_limits<double>::infinity();
EXPECT_EQ(res.dist[3], INF);  // Node 3 is in a different component

auto path = reconstructPath(res, 1, 3);
EXPECT_TRUE(path.empty());  // Can't reconstruct a path to an unreachable node
```

> **`std::numeric_limits<double>::infinity()`** is how C++ represents mathematical infinity. Dijkstra initializes all distances to infinity, and unreachable nodes stay at infinity.

When `dist[3]` is infinity, `reconstructPath` should detect "this node is unreachable" and return an empty path.

## Test 4: KnownOutput — The Most Important Test

This test uses a small graph where we've computed the shortest paths by hand and verify Dijkstra gives exactly those results:

```cpp
// Graph:
// A→B (cost 4), A→C (cost 2), C→B (cost 1), B→D (cost 5), C→D (cost 8)
// Shortest paths from A:
//   A→B: go A→C→B = 2+1 = 3 (NOT A→B directly = 4)
//   A→D: go A→C→B→D = 2+1+5 = 8 (NOT A→C→D = 2+8 = 10)

auto res = dijkstra(g, std::string("A"));
EXPECT_EQ(res.dist["B"], 3);  // Verify shortest distance to B
EXPECT_EQ(res.dist["D"], 8);  // Verify shortest distance to D

auto path = reconstructPath(res, std::string("A"), std::string("D"));
EXPECT_EQ(path[0], "A");
EXPECT_EQ(path[1], "C");
EXPECT_EQ(path[2], "B");
EXPECT_EQ(path[3], "D");
```

This is the core correctness test. If Dijkstra has a bug, this test catches it precisely.

---
---

# File 4: `test_astar.cpp` — Testing A\* Algorithm

A\* tests focus on grid graphs with `pair<int,int>` node types, since that's where A\* shines.

## The Key Test: HeuristicPruning — Does A\* Actually Visit Fewer Nodes?

```cpp
// Create a grid graph, run both algorithms on same source/destination
auto dijkRes   = dijkstraWithStats(g, src, dst);
auto astarRes  = astar(g, src, dst, manhattanHeuristic);

// A* must find the SAME optimal path (same length)
EXPECT_EQ(dijkRes.path.size(), astarRes.path.size());

// A* must visit FEWER nodes (heuristic guides it toward the goal)
EXPECT_LT(astarRes.nodesVisited, dijkRes.nodesVisited);
```

> **`EXPECT_LT(a, b)`** checks `a < b` (less than). This is the definitive proof that A\* works: it finds the same optimal path as Dijkstra but explores far fewer nodes along the way.

**Manhattan heuristic** for grids: `|x1-x2| + |y1-y2|` — the minimum number of moves needed if there were no walls. It's an admissible heuristic because you can never reach the destination in fewer moves than this.

---
---

# File 5: `test_mst.cpp` — Testing Kruskal and the DSU Data Structure

## What Is a DSU?

Before testing Kruskal, we test the **DSU (Disjoint Set Union)** that Kruskal uses internally. DSU is also called "Union-Find." It answers one question very fast:

> "Are these two nodes in the same connected component?"

It does this by assigning each node a "representative" (the root of its component). Two nodes are in the same component if they have the same representative.

## Test 1: DSUBasic — Testing Union-Find Internals

```cpp
DSU<int> dsu;
dsu.makeSet(1);
dsu.makeSet(2);
dsu.makeSet(3);

// Initially each node is its own representative
EXPECT_EQ(dsu.find(1), 1);

// After uniting 1 and 2, they share a representative
EXPECT_TRUE(dsu.unite(1, 2));
EXPECT_EQ(dsu.find(1), dsu.find(2));  // Same representative!

// Trying to unite two nodes already in the same component returns false
EXPECT_FALSE(dsu.unite(1, 2));
```

We test DSU **independently** from Kruskal. This is good practice — if Kruskal fails, check if DSU itself is broken first.

> **`EXPECT_EQ(dsu.find(1), dsu.find(2))`** — we don't care which specific representative they share, just that it's THE SAME ONE. DSU's "path compression" optimization may change which node becomes the root. The test is robust to this.

## Test 2: DirectedGraphThrows — Input Validation

```cpp
Graph<int, int> g(true);  // directed graph
g.addEdge(1, 2, 5);
EXPECT_THROW(kruskal(g), std::invalid_argument);
```

Kruskal's algorithm only works on undirected graphs. If you give it a directed graph, it should throw `std::invalid_argument` instead of silently giving a wrong answer.

> **`EXPECT_THROW(expr, ExceptionType)`** verifies that the given code throws the given exception type. If `kruskal(g)` doesn't throw, the test fails.

## Test 3: DisconnectedForest — MST on a Disconnected Graph

```cpp
// Two completely separate components: {1,2} and {3,4}
Graph<int, int> g(false);
g.addEdge(1, 2, 3);
g.addEdge(3, 4, 5);

auto mst = kruskal(g);
EXPECT_EQ(mst.size(), 2);   // Should have 2 edges total
EXPECT_EQ(weightSum, 8);    // 3 + 5 = 8
```

> **Spanning forest, not spanning tree:** When the graph is disconnected, there's no single spanning tree that covers all nodes. Instead, Kruskal produces a spanning tree for EACH separate component — together called a "spanning forest." For a graph with V nodes and C components, the result has exactly **V - C** edges. Here: 4 nodes, 2 components → 4-2 = 2 edges.

## Test 6: KnownMST — Classical Textbook Example

```cpp
// Famous 9-node textbook graph (nodes A through I)
// Known MST total weight: 37 (verified in graph theory textbooks)

EXPECT_EQ(mst.size(), 8);     // V - 1 = 9 - 1 = 8 edges in a spanning tree
EXPECT_EQ(totalWeight, 37);   // The known correct answer
```

> **A spanning tree for N nodes always has exactly N-1 edges.** This is a fundamental property of trees: enough edges to connect everything, but not so many that there are any cycles.

---
---

# File 6: `test_scc.cpp` — Testing Tarjan's SCC Algorithm

## What Is a Strongly Connected Component?

In a **directed graph**, a Strongly Connected Component (SCC) is a group of nodes where you can reach any node from any other node by following directed edges.

Example:
```
0 → 1 → 2 → 0  (a cycle — all three can reach each other)
1 → 3           (bridge edge — one-directional, doesn't create a new SCC)
```
Nodes {0, 1, 2} form one SCC. Node {3} is its own SCC (it can't get back to 0, 1, or 2).

## Helper Functions — The Problem with Order

```cpp
template<typename T>
bool componentEquals(std::vector<T> a, std::vector<T> b) {
    if (a.size() != b.size()) return false;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}
```

> **Why sort before comparing?**
> Tarjan's algorithm returns nodes in DFS discovery order, which depends on the iteration order of the internal hash map (essentially unpredictable). The SCC `{0, 1, 2}` might come back as `{2, 0, 1}`. By sorting both vectors before comparing, we ignore order — we only check that the same nodes are present.

```cpp
template<typename T>
bool hasSCC(const std::vector<std::vector<T>>& sccs, const std::vector<T>& target) {
    for (const auto& scc : sccs) {
        if (componentEquals(scc, target)) { return true; }
    }
    return false;
}
```

Checks whether any SCC in the results matches the target group of nodes (regardless of order within the SCC, or the order of SCCs in the result list).

## Test 5: CyclicComponents — The Main Test

```cpp
// Three separate cycles:
// Cycle 1: 0→1→2→0
// Cycle 2: 3→4→5→3
// Cycle 3: 6→7→6
// Bridge edges: 1→3 and 4→6 (connect the cycles but don't form new SCCs)

EXPECT_EQ(sccs.size(), 3);
EXPECT_TRUE(hasSCC(sccs, {0, 1, 2}));
EXPECT_TRUE(hasSCC(sccs, {3, 4, 5}));
EXPECT_TRUE(hasSCC(sccs, {6, 7}));
```

This is the canonical Tarjan test from graph theory textbooks. Three SCCs of sizes 3, 3, and 2. The bridge edges `1→3` and `4→6` connect components but are one-directional, so they don't create larger SCCs.

---
---

# The Test Build System: `tests/CMakeLists.txt`

## Why Does the Test Folder Have Its Own CMakeLists?

The project has two CMake files:
- `CMakeLists.txt` — the root configuration (builds the main program and benchmarks)
- `tests/CMakeLists.txt` — the test configuration (downloads Google Test, builds all 6 test programs)

Separating them keeps concerns organized — the main build doesn't need to know about testing details.

## Downloading Google Test Automatically

```cmake
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
```

> **`FetchContent`** is CMake's built-in system for downloading dependencies at build time. When you run `cmake -B build`, it automatically downloads Google Test from GitHub and compiles it alongside your project.
>
> This means you **don't** need to separately install Google Test (`apt install libgtest-dev` or similar). Anyone who clones this repo and runs `cmake` gets a working test setup automatically.

> **`GIT_TAG v1.14.0`** — pins to a specific version tag. If we used `GIT_TAG main`, a future change to Google Test could break our tests. Pinning to a version tag ensures everyone gets identical code, no matter when they clone.

```cmake
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
```

> This is a Windows-specific workaround. On Windows with MinGW, Google Test and the project code must link against the same C runtime library (CRT). Without this flag, the linker might complain about mismatched versions.

```cmake
FetchContent_MakeAvailable(googletest)
```

This is the line that actually triggers the download and compilation.

## Building All 6 Test Files with One Loop

```cmake
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

> **`foreach`** loops over a list. On each iteration, `${test_file}` expands to the current name (like `test_graph`, `test_bfs_dfs`, etc.).

For each test file, four things happen:
1. `add_executable(...)` — creates a separate compiled program
2. `target_include_directories(... PRIVATE include)` — tells the compiler to look in `include/` when resolving `#include "Graph.hpp"` etc.
3. `target_link_libraries(... GTest::gtest_main)` — links Google Test AND its built-in `main()` function. Without this, each test file would need to write its own `main()`.
4. `add_test(...)` — registers the binary with CTest so running `ctest` discovers and runs them all

## How to Run the Tests

After building:
```bash
cd build
ctest --output-on-failure
```

`ctest` runs every registered test and prints a summary. `--output-on-failure` shows the full failure message for any test that fails, instead of just "FAILED."

---

## Complete Summary

| File | What It Tests | Key Scenarios |
|---|---|---|
| `test_graph.cpp` | `Graph` class itself | Empty, single node, add/remove edges, scale |
| `test_bfs_dfs.cpp` | BFS and DFS | Traversal order, shortest path, unreachable nodes, cycle detection |
| `test_dijkstra.cpp` | Dijkstra + path reconstruction | Empty graph, self-path, disconnected, known output |
| `test_astar.cpp` | A\* + heuristic pruning | Same path as Dijkstra, fewer nodes visited |
| `test_mst.cpp` | Kruskal + DSU | Union-find operations, directed graph rejection, spanning forest, known MST |
| `test_scc.cpp` | Tarjan's SCC | No SCCs, single SCC, multiple SCCs, acyclic graph |
