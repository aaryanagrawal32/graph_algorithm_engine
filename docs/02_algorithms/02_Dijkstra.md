# Dijkstra.hpp — Complete Line-by-Line Explanation

This file introduces several C++ constructs that didn't appear in
`DFS.hpp`/`BFS.hpp`. Part 0 covers everything new. Part 1 walks the file
top to bottom. Part 2 traces the algorithm by hand on a small weighted
graph — including the exact moment a "stale entry" gets skipped, which is
the trickiest part of this file to understand from code alone.

---

# Part 0 — New Building Blocks

### Quick recap (already covered if you've seen DFS.hpp/BFS.hpp)
`#pragma once` = stop double-inclusion. `#include` = paste another file
in. `template<typename T>` = write one function/class that works for any
type `T`, resolved at compile time. `const X& g` = a read-only alias to
`g`, no copy made. Range-based `for (const auto& x : collection)` = loop
over each element by reference. `.find(k) == .end()` = "key `k` is not in
this map/set."

### NEW: `struct`
A `struct` is almost identical to a `class` in C++ — the only difference
is that a `struct`'s members are `public` by default (a `class`'s are
`private` by default). Using `struct` here is a signal to the reader:
"this is just a plain bundle of data, not an object with hidden internals
and behavior." `DijkstraResult` below is exactly that — two maps grouped
under one name, nothing more.

### NEW: Default template parameter — `typename Hash = GraphUtils::SafeHash`
This says: "if whoever uses `DijkstraResult` doesn't specify a third type
argument, use `GraphUtils::SafeHash` automatically." So
`DijkstraResult<int, double>` and `DijkstraResult<int, double, GraphUtils::SafeHash>`
mean the exact same thing — the first is just shorter to write.
`GraphUtils::SafeHash` is presumably a custom hash function defined inside
`Graph.hpp` — used to convert a `NodeType` value into the number an
`unordered_map`/`unordered_set` needs internally.

Notice this default does **not** appear on the `dijkstra()` or
`reconstructPath()` function templates further down. That's fine: when you
actually call `dijkstra(myGraph, src)`, the compiler doesn't need a
default for `Hash` at all — it looks at the real type of `myGraph` and figures out
`NodeType`, `WeightType`, and `Hash` directly from that, via **template argument
deduction**. The default on the struct exists purely for the (rarer) case of
someone declaring a bare `DijkstraResult` variable directly.

### NEW: `std::pair<A, B>`
The simplest possible "bundle two values together" type. A
`std::pair<WeightType, NodeType>` holds exactly one `WeightType` (accessed
as `.first`) and one `NodeType` (accessed as `.second`). No names beyond
`.first`/`.second` — it's deliberately minimal, used here just to smuggle
two related values (a distance, and the node that distance belongs to)
through the priority queue as one unit.

### NEW: `std::priority_queue` and `std::greater<>`
A `std::priority_queue` is a container that always hands you, via `.top()`,
the "highest priority" element currently inside it — and removes it via
`.pop()`. Internally it's a **binary heap** stored inside a regular
container (here, the `std::vector<PQElement>` you see as its second
template argument) — a structure that keeps insertion and removal of the
top element to O(log n) time.

By default, `std::priority_queue<T>` is a **max-heap**: `.top()` gives you
the LARGEST element, because it internally compares elements with
`std::less<T>`. To get a **min-heap** instead — where `.top()` gives the
SMALLEST element — you supply `std::greater<T>` as the third template argument,
which is exactly what this code does:
```cpp
std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
```
`std::greater<PQElement>` is a tiny "function object" that, given two elements `a`
and `b`, answers `a > b`. Handing this to the priority queue as its comparison
rule flips its internal ordering logic. This matters enormously: Dijkstra always
needs to process the **closest** not-yet-finalized node next, so a min-heap is
exactly the tool needed.

When comparing `PQElement`s (which are `std::pair<WeightType, NodeType>`),
`<` and `>` on a `std::pair` compare `.first` first, and only look at
`.second` to break ties — so this priority queue is effectively ordered
by distance (`.first`), which is exactly the intent.

### NEW: Structured bindings — `auto [d, u] = pq.top();`
This is a C++17 feature that unpacks a pair into separate named variables in one line.
Without it, you'd write:
```cpp
auto topElement = pq.top();
WeightType d = topElement.first;
NodeType u = topElement.second;
```
`auto [d, u] = pq.top();` does exactly that in one line — `d` becomes `.first`
(the distance), `u` becomes `.second` (the node).

### NEW: `std::numeric_limits<T>` and the ternary operator `?:`
`std::numeric_limits<T>` is a compile-time-known set of facts about a
numeric type `T`: its maximum value (`::max()`), whether it supports a
special "infinity" value (`::has_infinity` — true for `float`/`double`),
and the actual infinity value itself (`::infinity()`).

The **ternary operator**, `condition ? valueIfTrue : valueIfFalse`, is a
compact inline if/else that evaluates to one value or the other. This code
uses it to pick a stand-in value for "infinitely far / unreached":
```cpp
const WeightType INF = std::numeric_limits<WeightType>::has_infinity
                       ? std::numeric_limits<WeightType>::infinity()
                       : std::numeric_limits<WeightType>::max();
```
Read as: "if `WeightType` actually supports a real infinity value, use that.
Otherwise, fall back to the largest representable value as our 'pretend infinity.'"
This makes the whole algorithm work correctly whether `WeightType` is `double`,
`float`, or `int`.

### NEW: `for (const auto& node : g)` — looping directly over the Graph
In `DFS.hpp`/`BFS.hpp`, getting "every node" meant calling `g.getAllNodes()`.
Here, the code instead writes `for (const auto& node : g)` — looping directly
over `g` itself, with no method call at all. This works because `Graph` implements
its own `begin()`/`end()` member functions. The practical result is the same
(you get every node in the graph, one at a time) — it's just a cleaner syntax.

### NEW: `continue;`
A control-flow statement that immediately jumps to the next iteration of
the nearest enclosing loop. Here, `if (d > result.dist[u]) { continue; }` means:
"skip everything below this point for the current loop pass, and go straight
back to `while (!pq.empty())` to pop the next element."

### NEW: "Relaxing an edge" and greedy algorithms
"Relaxing an edge (u, v)" means: "check whether going u → v gives a shorter known
path to v than whatever we currently have recorded; if so, update the record."
Dijkstra's algorithm is called **greedy** because every time it pops a node, it
treats that node's currently-known distance as **final** and never revisits it.
This greedy commitment is only *safe* when edge weights are non-negative.

---

# Part 1 — The File, Line by Line

```cpp
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
```

### 1.1 — File header comment
States what the file does (shortest paths from ONE source to EVERY other node).
`O(E log V)` time, `O(V)` space. The "WHY USE THIS" note tells you when to reach for
Dijkstra: non-negative weights, faster than Bellman-Ford.

```cpp
#pragma once
 
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "Graph.hpp"
```

### 1.2 / 1.3 — Header guard and includes
Pulls in all standard library functionality needed:
- `<unordered_map>` → `std::unordered_map`, for `dist` and `prev`.
- `<vector>` → `std::vector`, for priority queue backing storage and path result.
- `<queue>` → `std::priority_queue`, the core data structure.
- `<limits>` → `std::numeric_limits`, for `INF`.
- `<algorithm>` → `std::reverse`, used in `reconstructPath`.
- `<functional>` → `std::greater`, the min-heap comparator.
- `"Graph.hpp"` → your own Graph class.

```cpp
/**
 * @struct DijkstraResult
 * @brief Contains the resulting shortest distance and predecessor tracking maps.
 */
template<typename NodeType, typename WeightType, typename Hash = GraphUtils::SafeHash>
struct DijkstraResult {
    std::unordered_map<NodeType, WeightType, Hash> dist; // Node -> Shortest distance from source
    std::unordered_map<NodeType, NodeType, Hash> prev;   // Node -> Predecessor node on shortest path
};
```

### 1.4 — The `DijkstraResult` struct
This bundles the two pieces of output Dijkstra's algorithm produces:
- `dist` — the shortest distance found from the source to each node.
- `prev` — the predecessor node on the shortest path, used to reconstruct paths.

Both are `unordered_map`s keyed by `NodeType`, using the same `Hash` template
parameter, so lookups are O(1) average time.

```cpp
/**
 * @brief Computes shortest paths from a single source node using Dijkstra's algorithm.
 ...
 * Time Complexity Derivation: O(E log V)
 * - Initializing distances to infinity takes O(V) time.
 * - Each vertex is pushed and popped from the priority queue at most once per incoming edge.
 *   In the worst case, we do O(E) pushes and O(E) pops.
 * - Each priority queue insertion/deletion takes O(log(size_of_heap)) = O(log V) time.
 * - Relaxing edges takes O(deg(u)) per node, totaling O(E) across the whole run.
 * - Total time is O(E log V + V). For connected graphs (E >= V - 1), this is O(E log V).
 */
```

### 1.5 — `dijkstra()` documentation block
`WeightType` must support arithmetic operations (`+` and `<`).

**The time complexity derivation, explained:**
- Setting initial distances to `INF`: O(V).
- Worst-case edge relaxations: O(E) pushes and pops.
- Each push/pop on a heap of size V: O(log V).
- Examining neighbors: O(E) total.
- Sum: O(E log V + V). For connected graphs, O(E log V).

```cpp
 * WHY greater<>: By default, std::priority_queue is a max-heap (uses less<>).
 * We need a min-heap to always process the closest discovered node first.
```
Flips internal ordering to get smallest element first.

```cpp
 * WHY stale entries appear & how we handle them:
 * C++ std::priority_queue does not support a decreaseKey operation. When a shorter path
 * to an existing node u is found, we push the new shorter distance (new_d, u) to the heap.
 * The older, larger distance (old_d, u) remains in the heap.
 * When (old_d, u) is eventually popped, it is a "stale entry". We check:
 * `if (d > dist[u]) continue;`
 * This ignores the stale entry because we've already processed u at a shorter distance.
```
Instead of updating in-place (decrease-key, which `std::priority_queue` lacks), A new
entry is pushed, and the old one is left to become stale. When popped, if `d` is worse
than the currently recorded best distance, we `continue` (skip).

```cpp
 * WHY this fails with negative weights:
 * Dijkstra's algorithm is greedy. It assumes that once a node u is popped, its shortest
 * distance is finalized and cannot be improved by paths through other remaining nodes.
 * If negative edge weights exist, this assumption is violated.
 */
```
Greedy commitment is only safe when edge weights are non-negative. With negative weights,
a path that looks longer could later pick up a big negative weight and become shorter,
but Dijkstra has no mechanism to revisit already-finalized nodes.

```cpp
template<typename NodeType, typename WeightType, typename Hash>
DijkstraResult<NodeType, WeightType, Hash> dijkstra(const Graph<NodeType, WeightType, Hash>& g, NodeType src) {
    DijkstraResult<NodeType, WeightType, Hash> result;

    if (!g.hasNode(src)) {
        return result;
    }
```

### 1.6 — `dijkstra()` body
Creates an empty result. Bails out if `src` is not in the graph.

```cpp
    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    for (const auto& node : g) {
        result.dist[node] = INF;
    }
    result.dist[src] = 0;
```

Computes `INF` and initializes all nodes' distances to `INF`, except the source which is `0`.

```cpp
    using PQElement = std::pair<WeightType, NodeType>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    pq.push({0, src});
```

Declares the min-heap and pushes the starting node `{0, src}`.

```cpp
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Stale entry check: skip if we've already found a shorter path to u
        if (d > result.dist[u]) {
            continue;
        }
```

FIFO pop and structured unpacking. If the popped entry is stale, skip it.

```cpp
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
```

If not stale, relax all outgoing edges:
- If `dist[u] + w < dist[v]`, update `dist[v]`, update `prev[v]`, and push `{dist[v], v}` to `pq`.
Once `pq` is empty, return `result`.

```cpp
/**
 * @brief Reconstructs the shortest path from src to dst using the Dijkstra predecessor map.
 ...
 */
```

### 1.7 — `reconstructPath()` documentation
Takes a precomputed `DijkstraResult` to build a path for a specific destination.
Allows O(1) path retrieval without re-running the full search.

```cpp
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> reconstructPath(const DijkstraResult<NodeType, WeightType, Hash>& result, NodeType src, NodeType dst) {
    std::vector<NodeType> path;

    // Verify destination exists and was reached
    auto distIt = result.dist.find(dst);
    if (distIt == result.dist.end()) {
        return path;
    }
```

### 1.8 — `reconstructPath()` body
Bails out with empty path if `dst` is not found.

```cpp
    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    if (distIt->second == INF) {
        return path; // Unreachable
    }
```

If the distance is still `INF`, the destination is unreachable — return empty path.

```cpp
    // Backtrack from destination to source using prev map
    NodeType curr = dst;
    while (curr != src) {
        path.push_back(curr);
        auto prevIt = result.prev.find(curr);
        if (prevIt == result.prev.end()) {
            return std::vector<NodeType>(); // Path is broken
        }
        curr = prevIt->second;
    }
    path.push_back(src);
```

Walk backward from `dst` using the predecessor chain. If a predecessor is missing,
return an empty vector (defensive check). Appends `src` at the end. Path is now `[dst, ..., src]`.

```cpp
    // Reverse path to go from src to dst
    std::reverse(path.begin(), path.end());

    return path;
}
```

`std::reverse` flips it in-place to get `[src, ..., dst]`.

---

# Part 2 — Tracing It By Hand

Consider this small weighted directed graph:
```
A --4--> B
A --1--> C
C --1--> B
B --1--> D
C --6--> D
```

Goal: shortest path from A to D.

### Initialization
```
dist = {A: 0, B: INF, C: INF, D: INF}
pq = [(0, A)]
```

### Pop (0, A)
`d=0`, `dist[A]=0` → not stale, proceed.
Relax A's neighbors:
- B: `dist[A] + 4 = 4 < INF` → `dist[B]=4`, `prev[B]=A`, push `(4, B)`.
- C: `dist[A] + 1 = 1 < INF` → `dist[C]=1`, `prev[C]=A`, push `(1, C)`.
`pq` = `[(1, C), (4, B)]`.

### Pop (1, C)
`d=1`, `dist[C]=1` → not stale, proceed.
Relax C's neighbors:
- B: `dist[C] + 1 = 2 < 4` → update! `dist[B]=2`, `prev[B]=C`, push `(2, B)`.
- D: `dist[C] + 6 = 7 < INF` → update. `dist[D]=7`, `prev[D]=C`, push `(7, D)`.
`pq` = `[(2, B), (4, B), (7, D)]`.

### Pop (2, B)
`d=2`, `dist[B]=2` → not stale, proceed.
Relax B's neighbors:
- D: `dist[B] + 1 = 3 < 7` → update! `dist[D]=3`, `prev[D]=B`, push `(3, D)`.
`pq` = `[(3, D), (4, B), (7, D)]`.

### Pop (3, D)
`d=3`, `dist[D]=3` → not stale. D has no outgoing edges.

### Pop (4, B) — **stale entry**
`d=4`, but `dist[B]=2`. `4 > 2` → **true** → `continue` (discarded).

### Pop (7, D) — **stale entry**
`d=7`, but `dist[D]=3`. `7 > 3` → **true** → `continue` (discarded).

`pq` is now empty. Done.

### Final result
```
Final dist = {A:0, B:2, C:1, D:3}
Final prev = {B:C, C:A, D:B}
```

Path to D: `A → C → B → D`, with total cost **3**.

### Tracing `reconstructPath(result, A, D)`
```
curr = D → path = [D] → curr = B
curr = B → path = [D, B] → curr = C
curr = C → path = [D, B, C] → curr = A
Stop. Push A → path = [D, B, C, A]
Reverse → [A, C, B, D]
```
Returns: `[A, C, B, D]`.

---

# Key Takeaways & Summary

### Dijkstra's Algorithm Summary

| Aspect | Details |
| :--- | :--- |
| **Problem Solved** | Single-source shortest paths on weighted graphs. |
| **Constraint** | Edge weights must be **non-negative** ($w \ge 0$). |
| **Core Paradigm** | Greedy (always processes the closest unvisited node next). |
| **Data Structures** | Min-priority queue (`std::priority_queue` with `std::greater`), distance map, predecessor map. |
| **Time Complexity** | $O(E \log V)$ with a binary heap. |
| **Space Complexity** | $O(V)$ to store distances, predecessors, and heap elements. |

> [!IMPORTANT]
> Dijkstra's algorithm relies on the greedy assumption that once a node is popped from the priority queue, its shortest distance is finalized. This assumption fails if there are negative edge weights, as a later, longer path could have a negative weight that reduces the overall cost. In such cases, use **Bellman-Ford**.
