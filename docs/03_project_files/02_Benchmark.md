# benchmark.cpp — The Performance Measurement Suite
### A Beginner-Friendly Guide

> **Who is this for?**
> This guide is for someone who understands what an algorithm does, but has never thought about *how fast* it runs or *how to measure* that. Every new concept is explained before the code.

---

## What Is This File?

`benchmarks/benchmark.cpp` answers the question: **"How fast is each algorithm, and how does it scale?"**

There is an important difference between two types of program verification:

| Type | What It Checks | How |
|---|---|---|
| **Unit tests** (`tests/`) | Correctness — does the algorithm give the right answer? | Run on small hand-crafted graphs with known expected output |
| **Benchmarks** (`benchmarks/`) | Performance — how fast is it? How does speed change with graph size? | Run on large randomly-generated graphs, measure time |

This file only measures **speed and memory usage** — it doesn't check whether results are correct (that's the tests' job).

---

## What Questions Does It Answer?

- **Scaling:** Does BFS take 10× longer on a graph 10× bigger? (It should, since BFS is O(V+E))
- **Comparison:** Is A\* faster than Dijkstra on the same graph? (Yes — by 5-12× on grids)
- **Density:** Does Kruskal slow down dramatically on dense (fully connected) graphs?

---

## The Include Block

```cpp
#include <iostream>      // std::cout for printing tables
#include <vector>        // std::vector — resizable list
#include <queue>         // std::queue — for the BFS benchmark
#include <stack>         // std::stack — for the DFS benchmark
#include <unordered_set> // std::unordered_set — for tracking visited nodes
#include <unordered_map> // std::unordered_map — dictionary/hash map
#include <utility>       // std::pair — holds 2 values
#include <random>        // std::mt19937 — random number generator
#include <iomanip>       // std::setw, std::fixed — table formatting
#include <cmath>         // std::sqrt — square root
```

The most important new include here is `<random>` — used to generate large test graphs programmatically with controlled, reproducible randomness.

---

## Part 1 — Custom BFS Benchmark Function

Why write a separate BFS function just for benchmarking? The regular `bfs()` in `BFS.hpp` only returns the traversal order — it doesn't track how much memory BFS used. The benchmark version adds **peak queue size tracking**.

```cpp
template<typename NodeType, typename WeightType, typename Hash = GraphUtils::SafeHash>
std::pair<double, size_t> benchmarkBFS(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
```

> **Return type `std::pair<double, size_t>`:**
> This function returns two values bundled together:
> - `double` — elapsed time in milliseconds
> - `size_t` — peak queue size (maximum number of nodes ever in the queue at once)
>
> C++ functions can only return one thing. `std::pair` is the simple solution for returning two.

> **`size_t`** is an unsigned integer type used for sizes. It's guaranteed to be large enough to hold the size of any object in memory. Using it for sizes and counts avoids "comparing signed and unsigned" compiler warnings.

### Tracking Peak Queue Size

```cpp
size_t maxQueueSize = 0;

while (!q.empty()) {
    maxQueueSize = std::max(maxQueueSize, q.size()); // Check BEFORE popping
    NodeType curr = q.front();
    q.pop();
    // ... process curr, push unvisited neighbors
}
```

> **`std::max(a, b)`** returns whichever of `a` or `b` is larger. By calling it every loop iteration, `maxQueueSize` always holds the largest value seen so far — a running maximum.

> **Why check BEFORE popping?** At the start of each loop iteration, the queue has its maximum possible size for that cycle — we haven't removed anything yet. If we checked after popping, we'd always miss the peak by one.

**What does peak queue size tell us?**
- **BFS on a tree:** Peak size = width of the widest level in the tree
- **BFS on a random graph:** Roughly proportional to the highest degree (number of connections) any node has
- It's a proxy for **memory usage** — a bigger peak queue means BFS needs more RAM at its busiest moment

---

## Part 2 — Custom DFS Benchmark Function

Same structure as BFS, but uses a `std::stack` instead of `std::queue`. The two data structures are what make the algorithms different:

| Algorithm | Data Structure | Order |
|---|---|---|
| BFS | Queue (first-in, first-out) | Visit nearby nodes first |
| DFS | Stack (last-in, first-out) | Go as deep as possible first |

### A Subtle Difference: When to Mark "Visited"

```cpp
while (!s.empty()) {
    maxStackSize = std::max(maxStackSize, s.size());
    NodeType curr = s.top();
    s.pop();

    if (visited.find(curr) == visited.end()) {  // Check AFTER popping
        visited.insert(curr);
        // Push neighbors in reverse order
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            if (visited.find(it->to) == visited.end()) {
                s.push(it->to);
            }
        }
    }
}
```

> **BFS marks visited when PUSHING** — so a node is never added to the queue twice.
>
> **Iterative DFS marks visited when POPPING** — because the same node can legitimately be pushed onto the stack multiple times before it's processed. If you marked it when pushing, you might miss valid paths. This is a known quirk of iterative (non-recursive) DFS.

### Reverse Iteration for Consistent Order

```cpp
for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
```

> **`rbegin()` / `rend()`** are **reverse iterators** — they go from last to first instead of first to last. We push neighbors in reverse so the **first neighbor** ends up on top of the stack and gets processed first. This matches the order that a recursive DFS would use, making the output predictable and comparable.

---

## Part 3 — Generating Large Random Graphs

You can't manually write a 10,000-node graph. These generator functions create large test graphs automatically, with controlled randomness.

### Generator 1: `generateRandomSparse()` — A Directed Sparse Graph

```cpp
Graph<int, int> generateRandomSparse(int numNodes, int edgeFactor) {
    Graph<int, int> g(true); // directed

    // Step 1: Add all nodes
    for (int i = 0; i < numNodes; ++i) { g.addNode(i); }

    // Step 2: Create a line path to guarantee connectivity
    for (int i = 0; i < numNodes - 1; ++i) { g.addEdge(i, i + 1, 1); }

    // Step 3: Add random extra edges
    std::mt19937 rng(42);  // Seed = 42
    std::uniform_int_distribution<int> nodeDist(0, numNodes - 1);

    int extraEdges = numNodes * edgeFactor;
    for (int i = 0; i < extraEdges; ++i) {
        int u = nodeDist(rng);
        int v = nodeDist(rng);
        if (u != v) { g.addEdge(u, v, 1); }
    }
    return g;
}
```

#### Why Create a Line Path First?

Without the `0→1→2→...→(N-1)` chain, random edge generation might create disconnected islands. Some nodes might only be reachable from certain starting points, making BFS/DFS unable to explore the whole graph. The line path guarantees **every node is reachable from node 0**.

#### What is `std::mt19937`?

> **`std::mt19937`** is the **Mersenne Twister** — C++11's built-in high-quality random number generator. The number 19937 is part of its mathematical design.
>
> ```cpp
> std::mt19937 rng(42);  // 42 is the "seed"
> ```
>
> The **seed** is the starting value. The same seed always produces the same sequence of random numbers. This is called **reproducibility** — everyone who runs this benchmark on any machine gets the exact same graph, making comparisons fair.

> **`std::uniform_int_distribution<int> nodeDist(0, numNodes - 1)`** converts raw random bits into integers in a specific range. `nodeDist(rng)` generates one random integer between 0 and `numNodes - 1`.

#### What Makes a Graph "Sparse"?

With `edgeFactor = 2`, we add `numNodes * 2` random edges. For N=10,000 nodes:
- Random edges: ~20,000
- Line path: 9,999
- Total: ~30,000 edges
- Edge-to-node ratio ≈ 3

A **dense graph** would have O(N²) edges — for N=10,000 that's ~100,000,000 edges. Our sparse graph is much smaller by comparison.

---

### Generator 2: `generateGridGraph()` — A 2D Grid for A\* Benchmarking

```cpp
Graph<std::pair<int, int>, double> generateGridGraph(int W, int H) {
    Graph<std::pair<int, int>, double> g(true);
    std::mt19937 rng(1337);
    std::uniform_real_distribution<double> weightDist(1.0, 10.0);
```

> **Why a grid?**
> A\* is most commonly used in grid-based pathfinding (robot navigation, video game maps). A grid is also the perfect test case because:
> - Nodes are naturally `(x, y)` coordinate pairs
> - Manhattan distance (`|x1-x2| + |y1-y2|`) is a perfect admissible heuristic
> - The "diagonal path" from corner to corner is a well-defined challenge

> **Node type `std::pair<int,int>`** — instead of integer node IDs (0, 1, 2...), each node IS its own coordinate. Node `{3, 5}` means column 3, row 5.

```cpp
auto addBidirectionalEdge = [&](std::pair<int,int> u, std::pair<int,int> v) {
    double w = weightDist(rng);
    g.addEdge(u, v, w);
    g.addEdge(v, u, w); // Same weight in both directions
};
```

> **Lambda `[&](...) { }` — an anonymous function.** Instead of writing the same two `addEdge` calls repeatedly, we define a tiny helper function on the spot. `[&]` means "capture all local variables by reference" (so the lambda can use `rng`, `g`, etc.).

```cpp
for (int x = 0; x < W; ++x) {
    for (int y = 0; y < H; ++y) {
        std::pair<int,int> curr = {x, y};
        if (x + 1 < W) { addBidirectionalEdge(curr, {x + 1, y}); }  // Connect right
        if (y + 1 < H) { addBidirectionalEdge(curr, {x, y + 1}); }  // Connect up
    }
}
```

Each node connects to its right neighbor and upper neighbor (boundary checks stop us from going out of bounds). Since edges are bidirectional, this creates a standard 4-directional grid.

---

### Generator 3: `generateMSTGraph()` — Undirected Graph for Kruskal

```cpp
Graph<int, int> generateMSTGraph(int numNodes, bool dense) {
    Graph<int, int> g(false); // undirected — MST requires this!
```

The `bool dense` parameter switches between two completely different graph shapes:

**Dense (complete graph — every node connected to every other):**
```cpp
for (int i = 0; i < numNodes; ++i) {
    for (int j = i + 1; j < numNodes; ++j) {
        g.addEdge(i, j, weightDist(rng));
    }
}
```
Number of edges = N × (N-1) / 2 = **O(N²)**. For N=500, that's ~125,000 edges. Kruskal must sort all of them — gets very slow.

**Sparse (a ring with random extras):**
```cpp
for (int i = 0; i < numNodes; ++i) {
    g.addEdge(i, (i + 1) % numNodes, weightDist(rng));
}
```
> **`% numNodes`** is the modulo operator — it wraps around. When `i = numNodes - 1`, `(i + 1) % numNodes = 0`, connecting the last node back to the first, forming a ring/cycle.

Number of edges ≈ **O(N)**. For N=10,000, that's ~10,000 edges. Kruskal is very fast.

---

## Part 4 — The Benchmark Routines

### BFS vs DFS Table Output

```cpp
std::cout << std::left << std::setw(12) << "Node Count"
          << " | " << std::setw(14) << "BFS Time (ms)"
          // ...
```

> **`std::left`** — left-aligns text within its allocated space.
> **`std::setw(12)`** — sets the minimum width of the next printed item to 12 characters. If shorter, it's padded with spaces. This creates aligned columns.
> **`std::fixed << std::setprecision(3)`** — `std::fixed` turns off scientific notation (so `0.001` shows as `0.001`, not `1e-3`). `std::setprecision(3)` shows exactly 3 decimal places.

```cpp
std::vector<int> sizes = {100, 1000, 10000, 100000};
for (int N : sizes) {
    auto g = generateRandomSparse(N, 2);
    auto bfsRes = benchmarkBFS(g, 0);
    auto dfsRes = benchmarkDFS(g, 0);
```

We test **four different sizes** to reveal the scaling behavior. If BFS on N=100 takes 0.01ms and N=1000 takes 0.1ms (10× time for 10× nodes), that confirms the expected O(V+E) linear scaling.

---

### Dijkstra vs A\* — The Key Benchmark

```cpp
std::vector<std::pair<int, int>> dimensions = {{10, 10}, {32, 32}, {100, 100}};
for (auto [W, H] : dimensions) {
```

> **`auto [W, H]`** is C++17 **structured binding** — it unpacks a `pair<int,int>` directly into two named variables. Without it you'd write `dimensions[i].first` and `dimensions[i].second`.

```cpp
std::pair<int,int> src = {0, 0};      // Top-left corner
std::pair<int,int> dst = {W-1, H-1};  // Bottom-right corner
```

Source is always the top-left and destination is always the bottom-right. This is the "diagonal crossing" — it forces the algorithm to search through the middle of the grid.

```cpp
auto dijRes   = dijkstraWithStats(g, src, dst); // Returns path + nodes visited
auto astarRes = astar(g, src, dst, manhattanHeuristic);
```

Both algorithms run on the **identical graph** with the **identical source and destination**. `dijkstraWithStats` tracks how many nodes were settled (popped from the priority queue) — a measure of how much of the graph was explored.

**Expected result:** On a 100×100 grid (10,000 nodes), Dijkstra explores ≈10,000 nodes. A\* with Manhattan heuristic explores only ≈800-1,500 nodes. That's a **5-12× speedup** while still finding the same optimal path.

---

### Kruskal — Sparse vs Dense Comparison

```cpp
// Sparse: test up to 10,000 nodes
for (int N : sparseSizes) {
    auto g = generateMSTGraph(N, false);  // sparse
    // time kruskal(g)
}

// Dense: only test up to 2,000 nodes
for (int N : denseSizes) {
    auto g = generateMSTGraph(N, true);   // dense = O(N²) edges
    // time kruskal(g)
}
```

**Why does the dense test stop at 2,000 while sparse goes to 10,000?**

On a dense graph:
- N=2,000 → ~2,000,000 edges → Kruskal takes several seconds
- N=10,000 → ~50,000,000 edges → Would take many minutes just to generate!

We choose test sizes that are **large enough to be meaningful** but **small enough to complete in reasonable time**.

---

## Part 5 — The `main()` Function

```cpp
int main() {
    std::cout << "======================================================================\n";
    std::cout << "                  GRAPH ENGINE PERFORMANCE BENCHMARKS\n";
    std::cout << "======================================================================\n";

    runBFSvsDFS();
    runDijkstraVsAStar();
    runKruskalMST();

    std::cout << "\nBenchmarks completed successfully.\n";
    return 0;
}
```

Simple and clean — calls the three benchmark suites in order. No exception handling here because benchmark failures (like a graph generation error) would crash with a visible error message — acceptable for a development tool that developers run manually.

---

## Why Does This File Have Its Own `main()`?

This file has `int main()` — and so does `src/main.cpp`. Normally you can't have two `main()` functions. The solution is that they're **separate programs**:

```cmake
add_executable(graph_engine src/main.cpp)          # Demo program
add_executable(benchmark benchmarks/benchmark.cpp)  # Benchmark program
```

CMake compiles them into two different executables. This separation means:
- You can run the demo without waiting for slow benchmarks
- Benchmark output is clean (no demo results mixed in)
- Benchmarks can use much larger graphs than the demo needs

---

## Key Concepts Summary

| Concept | Where Used | Plain-English Meaning |
|---|---|---|
| `std::pair<A, B>` | Return types, grid nodes | A container for exactly two values |
| `std::mt19937` | All generators | A high-quality random number generator |
| Seed `42` or `1337` | `std::mt19937 rng(seed)` | Fixed starting value → reproducible results |
| `std::uniform_int_distribution` | Edge generation | Converts random bits into a range of integers |
| `std::max(a, b)` | Peak tracking | Returns whichever is larger; builds running maximum |
| `rbegin() / rend()` | DFS benchmark | Reverse iteration (last-to-first) |
| `std::setw(N)` | Table printing | Pads output to N characters wide |
| `std::fixed` | Table printing | No scientific notation in floating-point output |
| Lambda `[&]` | `addBidirectionalEdge` | Tiny helper function defined on the spot |
| `auto [W, H]` structured binding | Grid loop | Unpack a pair into named variables |
| `% numNodes` (modulo) | Ring graph | Wraps around: `(N-1+1) % N = 0` |
