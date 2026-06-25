# AStar.hpp — Complete Line-by-Line Explanation

This file introduces the biggest new concept in the series so far —
**lambda expressions** — plus a few smaller new ideas (the open/closed
set pattern, a generic "callable" template parameter, default member
initializers). Part 0 covers everything new. Part 1 walks the file top
to bottom. Part 2 traces A* by hand on a small grid graph, specifically
chosen to show it skip an entire dead-end branch that a plain Dijkstra
search would have wasted time exploring.

---

# Part 0 — New Building Blocks

### Quick recap (covered in earlier files in this series)
`template<typename T>` = one function/class definition that works for
any type, resolved at compile time. `const X&` = read-only alias, no
copy. `struct` = plain data bundle, public by default. `std::pair<A,B>`
= two values glued together as `.first`/`.second`. `std::priority_queue`
with `std::greater<>` = a min-heap (smallest `.top()` first). Structured
bindings (`auto [a, b] = pair;`) = unpack a pair into two named
variables. `std::numeric_limits<T>::has_infinity ? infinity() : max()`
= a type-safe stand-in for "infinitely far / unreached." Looping
`for (const auto& node : g)` = Graph supports being iterated directly.
`.find(k) == .end()` = "key not present." "Relaxing an edge (u,v)" =
checking whether going through u→v improves the best known way to reach
v, and updating if so.

### NEW: Lambda expressions
A **lambda** is a way to write a small, nameless function *inline*,
right where you need it, instead of declaring a separate named function
elsewhere. The general shape is:

```
[capture](parameters) { body }
```

Look at the simplest one in this file:
```cpp
inline auto zeroHeuristic = [](const auto&, const auto&) {
    return 0.0;
};
```
Breaking this into its pieces:
- `[]` — the **capture clause**. Controls which variables from the surrounding
  code the lambda can access. Here it's empty — it only depends on parameters.
- `(const auto&, const auto&)` — the **parameter list**. Unnamed parameters since
  they are unused. `auto` indicates a C++14 **generic lambda**, so it can be called
  with any types. This is essential since `zeroHeuristic` must work for any `NodeType`.
- `{ return 0.0; }` — the **body**. Always returns `0.0`.

A lambda expression produces a callable object. `inline auto zeroHeuristic = [...];`
stores that callable object, allowing us to call `zeroHeuristic(nodeA, nodeB)`.

### NEW: `inline` on a variable
Headers can get `#include`d into multiple `.cpp` files. Without `inline`, each file
would define its own copy of the variable, causing linker conflicts. `inline` tells
the linker to merge them into a single definition.

### NEW: Default member initializers
```cpp
template<typename NodeType>
struct AStarResult {
    std::vector<NodeType> path;
    int nodesVisited = 0;
};
```
- `path` is a class type (`std::vector`), so it automatically runs its own default
  constructor, guaranteeing it starts empty.
- `int` is a primitive type with no automatic initialization. Without `= 0`, it would
  contain random garbage memory. `= 0` is a **default member initializer** (C++11)
  that guarantees it always starts at `0`.

### NEW: Generic "callable" template parameter — `typename Heuristic`
```cpp
template<typename NodeType, typename WeightType, typename Hash, typename Heuristic>
AStarResult<NodeType> astar(const Graph<NodeType, WeightType, Hash>& g, NodeType src, NodeType dst, Heuristic h)
```
`Heuristic` represents any callable type (lambda, function pointer, functor). The body
can simply call `h(u, dst)` generically without knowing the underlying type.

### NEW: Open set / closed set, and why this file uses a `closedSet`
- The **open set** — discovered nodes not yet fully processed (`pq`).
- The **closed set** — fully processed nodes (`closedSet`), tracked using a
  `std::unordered_set`.

In `Dijkstra.hpp`, this was managed with `if (d > dist[u])`. A* uses an explicit
`closedSet` which also serves as our source for the `nodesVisited` counter.

### NEW: Early termination with inlined path reconstruction
Dijkstra computed distances to all reachable nodes. `astar()` does something different:
the moment the destination `dst` is popped and closed, it reconstructs the path **inline**
and returns immediately — pruning all remaining nodes. Since the heuristic steers the
search toward `dst`, continuing after finding the goal is wasteful.

### Recap: `std::abs` and `std::sqrt`
Math functions from `<cmath>`. Note that `^` is bitwise XOR, not exponentiation.
Squaring in C++ is done via `dx * dx`.

---

# Part 1 — The File, Line by Line

```cpp
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
```

### 1.1 — Header comment
A* is built for point-to-point queries, not full shortest-path trees.
With a good heuristic, it explores far fewer nodes than Dijkstra.

```cpp
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
```

### 1.2 — Header guard and includes
Pulls in `<unordered_set>` (for `closedSet`) and `<cmath>` (for heuristics).

```cpp
/**
 * @struct AStarResult
 * @brief Contains the reconstructed shortest path and search statistics.
 */
template<typename NodeType>
struct AStarResult {
    std::vector<NodeType> path; // Shortest path from src to dst. Empty if unreachable.
    int nodesVisited = 0;       // Number of unique nodes popped and processed (closed)
};
```

### 1.3 — `AStarResult` struct
Templated on `NodeType` only since the output does not need to know about weights or hashing.

```cpp
// ============================================================================
// Built-in Heuristics
// ============================================================================
```

```cpp
/**
 * @brief Zero heuristic. ALWAYS returns 0.0.
 * Passing this to A* degenerates the algorithm exactly into Dijkstra's algorithm.
 */
inline auto zeroHeuristic = [](const auto&, const auto&) {
    return 0.0;
};
```

### 1.4 — `zeroHeuristic`
If `h` always returns `0.0`, `f(n) = g(n) + h(n)` simplifies to `f(n) = g(n)`.
The queue is ordered purely by cost from start, behaving like Dijkstra.

```cpp
/**
 * @brief Manhattan distance heuristic for grid-based coordinates (pair<int, int>).
 * Calculates |x1 - x2| + |y1 - y2|.
 */
inline auto manhattanHeuristic = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return static_cast<double>(std::abs(a.first - b.first) + std::abs(a.second - b.second));
};
```

### 1.5 — `manhattanHeuristic`
Concrete types `const std::pair<int, int>&` because Manhattan distance is a grid concept.
`static_cast<double>` converts the integer sum to a `double` for consistency.

```cpp
/**
 * @brief Euclidean distance heuristic for geographic/spatial coordinates (pair<double, double>).
 * Calculates sqrt((x1 - x2)^2 + (y1 - y2)^2).
 */
inline auto euclideanHeuristic = [](const std::pair<double, double>& a, const std::pair<double, double>& b) {
    double dx = a.first - b.first;
    double dy = a.second - b.second;
    return std::sqrt(dx * dx + dy * dy);
};
```

### 1.6 — `euclideanHeuristic`
Concrete heuristic for `std::pair<double, double>` spatial nodes. Straight-line distance.

```cpp
// ============================================================================
// A* Implementation
// ============================================================================
```

```cpp
/**
 * @brief Performs A* search algorithm from a source node to a destination node.
 ...
 * HOW A* WORKS: f(n) = g(n) + h(n)
 * - g(n) is the exact cost from the source node to node n.
 * - h(n) is the estimated remaining cost from node n to the destination.
 * - f(n) is the total estimated cost of a path passing through node n.
 * By ordering our priority queue by f(n) instead of just g(n) (as Dijkstra does),
 * A* focuses its search space in the direction of the goal.
```

### 1.7 — Core A* idea
Dijkstra ripples in all directions. A* uses `f(n) = g(n) + h(n)` to penalize nodes heading
away from the goal, focusing the search space toward the destination.

```cpp
 * HEURISTIC ADMISSIBILITY:
 * A heuristic h(n) is "admissible" if it never overestimates the actual cost to reach
 * the destination (i.e., h(n) <= h*(n)).
 * If a heuristic is admissible, A* is GUARANTEED to find the mathematically optimal path.
 *
 * WHY MANHATTAN IS ADMISSIBLE FOR GRIDS:
 * In a grid with 4-directional movement, the shortest possible path without obstacles
 * is exactly their Manhattan distance. Obstacles can only make the path longer, so the
 * estimate never overestimates the true distance.
 */
```
**Admissibility** guarantees optimal results.

```cpp
template<typename NodeType, typename WeightType, typename Hash, typename Heuristic>
AStarResult<NodeType> astar(const Graph<NodeType, WeightType, Hash>& g, NodeType src, NodeType dst, Heuristic h) {
    AStarResult<NodeType> result;

    if (!g.hasNode(src) || !g.hasNode(dst)) {
        return result;
    }
```

### 1.8 — `astar()` body
Signature takes `Heuristic h` by value. Bails out if `src` or `dst` are missing.

```cpp
    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();

    std::unordered_map<NodeType, WeightType, Hash> gScore;
    std::unordered_map<NodeType, NodeType, Hash> prev;

    for (const auto& node : g) {
        gScore[node] = INF;
    }
    gScore[src] = 0;
```

Initialize `gScore` (equal to `dist` in Dijkstra) of all nodes to `INF`, except `src` which is `0`.

```cpp
    // Min-heap storing pair of {fScore, node_id}
    using PQElement = std::pair<double, NodeType>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    pq.push({static_cast<double>(h(src, dst)), src});
```

Min-heap tracks f-score as a `double`. Seeded with `{h(src, dst), src}` since `g(src) = 0`.

```cpp
    std::unordered_set<NodeType, Hash> closedSet;

    while (!pq.empty()) {
        auto [f, u] = pq.top();
        pq.pop();

        if (closedSet.find(u) != closedSet.end()) {
            continue;
        }
```

FIFO pop and structured unpacking. If `u` is already in `closedSet`, skip it.

```cpp
        closedSet.insert(u);
        result.nodesVisited++;
```

Add to `closedSet` and increment the unique visit counter.

```cpp
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
```

Early exit: when we close `dst`, reconstruct the path backward inline, reverse it,
and return immediately.

```cpp
        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            WeightType weight = edge.weight;

            if (closedSet.find(v) != closedSet.end()) {
                continue;
            }
```

Examine neighbors. Skip if the neighbor is already in `closedSet`.

```cpp
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
```

If `tentativeG < gScore[v]`, update `gScore[v]`, update `prev[v]`, compute `fScore = g + h`,
and push `{fScore, v}` to `pq`. Returns empty result if loop terminates without finding `dst`.

---

# Part 2 — Tracing It By Hand

Consider this grid graph with edge weight 1:
```
S=(0,0) --1--> A=(1,0) --1--> Goal=(1,1)
S=(0,0) --1--> B=(0,1) --1--> Goal=(1,1)
A=(1,0) --1--> C=(2,0)   ← decoy branch (away from Goal)
C=(2,0) --1--> D=(2,1)   ← decoy dead end
```

Heuristics to `Goal=(1,1)`:
`h(S)=2, h(A)=1, h(B)=1, h(Goal)=0, h(C)=2`

### Initialization
```
gScore = {S:0, A:INF, B:INF, Goal:INF, C:INF, D:INF}
pq = [(f=2, S)]
closedSet = {}
```

### Pop (2, S)
Add S to `closedSet`. `nodesVisited = 1`.
Relax neighbors:
- A: `gScore[A]=1, prev[A]=S, fScore = 1 + h(A) = 2` → push `(2, A)`.
- B: `gScore[B]=1, prev[B]=S, fScore = 1 + h(B) = 2` → push `(2, B)`.
`pq` = `[(2, B), (2, A)]` (B tie-breaks first).

### Pop (2, B)
Add B. `nodesVisited = 2`.
Relax neighbor:
- Goal: `gScore[Goal]=2, prev[Goal]=B, fScore = 2 + h(Goal) = 2` → push `(2, Goal)`.
`pq` = `[(2, A), (2, Goal)]`.

### Pop (2, A)
Add A. `nodesVisited = 3`.
Relax neighbors:
- Goal: `tentativeG = 2`, not `< gScore[Goal]=2` → no push.
- C: `gScore[C]=2, prev[C]=A, fScore = 2 + h(C) = 4` → push `(4, C)`.
`pq` = `[(2, Goal), (4, C)]`.

### Pop (2, Goal) — **destination reached!**
Add Goal. `nodesVisited = 4`. `u == dst` → reconstruct:
`Goal ← B ← S`. Reverse → `[S, B, Goal]`.
**Return immediately.**

`(4, C)` is left in the queue. **C is never closed, and D is never even discovered!**

### Final result
```
path = [S, B, Goal]
nodesVisited = 4   (S, B, A, Goal)
```
Dijkstra would have explored all 6 nodes. A* successfully pruned the decoy branches.

---

# Key Takeaways & Summary

### A* Search Algorithm Summary

| Component | Description / Value |
| :--- | :--- |
| **Target Query** | Point-to-point shortest path (single-source to single-destination). |
| **Evaluation Function** | $f(n) = g(n) + h(n)$ |
| **$g(n)$** | Exact cost from the source node to the current node $n$. |
| **$h(n)$** | Estimated remaining cost from the current node $n$ to the destination. |
| **Admissibility** | Heuristic $h(n)$ must never overestimate the true cost to the goal ($h(n) \le h^*(n)$). |
| **Consistency** | For every edge $u \to v$, $h(u) \le \text{cost}(u, v) + h(v)$ (prevents re-opening closed nodes). |
| **Time Complexity** | $O(E \log V)$ worst case (same as Dijkstra), but much faster in practice. |
| **Space Complexity** | $O(V)$ to maintain the open set (priority queue) and closed set. |

> [!TIP]
> A* is highly sensitive to the quality of the heuristic. A heuristic of $h(n) = 0$ turns A* into Dijkstra's algorithm, while an overestimating (inadmissible) heuristic turns it into a fast but sub-optimal greedy search. Always use **Manhattan distance** for 4-directional grids and **Euclidean distance** for straight-line spatial coordinates.
