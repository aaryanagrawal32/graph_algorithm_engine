# BellmanFord.hpp — Complete Line-by-Line Explanation

This file is actually simpler than `Dijkstra.hpp`/`AStar.hpp` in terms of
C++ machinery — no priority queue, no lambdas, no structured bindings.
What's new here is almost entirely **exceptions** (throwing errors) and
some genuinely subtle **graph theory reasoning** (why exactly V-1
iterations, and why a V-th iteration can detect a negative cycle). Part 0
covers the new mechanics. Part 1 walks the file top to bottom, spending
real time unpacking the two mathematical "WHY" proofs. Part 2 traces the
algorithm twice by hand — once on a graph that converges early and exits
cleanly, and once on a graph with an actual negative cycle, to watch the
detection fire.

---

# Part 0 — New Building Blocks

### The big conceptual shift from Dijkstra/A\*
Dijkstra and A\* are both **greedy**: they always process the
closest/most-promising node next (via a priority queue), and they commit
to a node's distance the moment it's processed, never revisiting it. That
greedy commitment is exactly what breaks when negative weights are
allowed (covered in the Dijkstra explanation). Bellman-Ford abandons
greediness entirely and uses a completely different strategy: **brute
force, repeated, full-graph relaxation**. Instead of cleverly picking
which node to process next, it just relaxes EVERY edge in the graph,
over and over, a precise number of times that's mathematically guaranteed
to be enough — no heap, no "process the best candidate first," nothing
clever. This is slower (`O(V·E)` instead of `O(E log V)`), but it
correctly handles negative weights, which Dijkstra/A\* simply cannot.

### NEW: The classic indexed `for` loop
Every previous file in this series used either a `while` loop or a
range-based `for (const auto& x : collection)` loop. This file
introduces the older, more manual loop style:
```cpp
for (int i = 0; i < V - 1; ++i) {
    ...
}
```
This has three parts, separated by semicolons, all on one line:
1. `int i = 0` — runs ONCE, before the loop starts: declare a counter
   variable `i`, starting at `0`.
2. `i < V - 1` — checked BEFORE every iteration (including the very
   first): the loop keeps running as long as this stays true.
3. `++i` — runs AFTER every iteration's body finishes, right before the
   condition gets checked again: increase `i` by 1.

So this particular loop runs its body with `i = 0, 1, 2, ..., V-2` — that
is, exactly `V - 1` times total. This pattern — "do
something a specific, known number of times" — is what indexed loops are
for, as opposed to range-based for loops (which are for "do something
once per element of a collection, whatever that collection happens to
be").

**Prefix (`++i`) vs postfix (`i++`):** both increase `i` by 1. The only
difference is what the increment EXPRESSION itself evaluates to:
`++i` evaluates to the NEW value (after incrementing), `i++` evaluates to
the OLD value (before incrementing). When the increment is its own
standalone statement (as it is here, and in nearly every for-loop you'll
ever see), nobody uses that evaluated value for anything — so `++i` and
`i++` behave IDENTICALLY in this position. `++i` is simply the more
traditional convention for for-loops in C++.

### NEW: Exceptions — `throw`, and exception types
An **exception** is C++'s mechanism for signaling "something has gone
wrong, and I cannot proceed normally" — used instead of returning a
special error value. The keyword `throw` immediately abandons whatever
function you're in (no `return` happens, nothing after the `throw` on
that path ever executes) and starts searching UP the chain of function
calls that led here for some code prepared to handle it.

```cpp
throw std::invalid_argument("Source node does not exist in the graph.");
```
This line does two things at once:
1. **Constructs** a new exception object — here, of type
   `std::invalid_argument` (from `<stdexcept>`) — passing it a
   human-readable message string explaining what went wrong.
2. **Throws** it — hands that object off to C++'s exception-handling
   machinery.

What happens next depends entirely on the CALLER:
- If whoever called `bellmanFord()` wrapped that call in a
  `try { ... } catch (const std::invalid_argument& e) { ... }` block
  (or `catch (const std::exception& e)`, since `std::invalid_argument`
  is-a `std::exception` through inheritance), execution jumps straight
  into that `catch` block, skipping everything else that would have run
  normally.
- If NO enclosing code anywhere up the call chain catches it, the
  exception keeps propagating upward, function after function, until it
  either finds a `catch` somewhere, or reaches the very top of the
  program with nothing left to unwind through — at which point the
  program terminates immediately (via `std::terminate`), typically
  printing something like `terminate called after throwing an instance of 'std::invalid_argument'` to the console.

**`std::invalid_argument` vs `std::runtime_error`** — both are standard
exception types, but they signal conceptually different kinds of
problems:
- `std::invalid_argument` (used for the missing-source-node case) — the
  CALLER gave bad input. This is knowable instantly, just by looking at
  what was passed in, before any real work happens — "you asked me to
  start from a node that isn't even in this graph."
- `std::runtime_error` (used for the negative-cycle case, further down)
  — something is wrong that could only be discovered by actually
  RUNNING the algorithm. There's no way to know in advance, just by
  glancing at the function's arguments, whether the GRAPH'S DATA happens
  to contain a negative cycle — you only find out partway through doing
  the real work.

**Contrast with every earlier file in this series:** `DFS.hpp`,
`BFS.hpp`, `Dijkstra.hpp`, and `AStar.hpp` all handled "the requested
node doesn't exist" the same way — quietly returning an empty/default
result and letting the caller notice on their own. This file makes a
different design choice for that exact same situation: it throws instead.
Both approaches are valid C++ API designs; this just illustrates that
the author of this particular file decided a missing source node is
serious enough to force the caller to explicitly notice, rather than risk
the caller silently misinterpreting a quietly-returned empty map.

### NEW: `@throws` (Doxygen tag)
One more documentation tag, alongside the `@file`/`@brief`/`@tparam`/
`@param`/`@return` ones from earlier files: `@throws TYPE description`
documents which exception type a function might throw, and under what
condition.

### NEW (conceptual): Signed integer overflow — why `dist[u] == INF` gets
### checked before adding to it
```cpp
if (dist[u] == INF) {
    continue;
}
```
This guard exists to prevent a subtle but serious bug. If `WeightType`
is a floating-point type (`double`/`float`), `INF` is genuine
mathematical infinity, and `infinity + (any finite number) = infinity`
still — so arithmetic involving it behaves sensibly even without this
guard. But if `WeightType` is a plain integer type like `int`, `INF` is
actually just `std::numeric_limits<int>::max()` — a very large but
ORDINARY finite number (around 2.1 billion). Adding ANY positive edge
weight `w` to that would push the result PAST the largest value an `int`
can hold — this is **signed integer overflow**, and in C++ it is
**undefined behavior**. The `if (dist[u] == INF) continue;` guard sidesteps
the entire danger by simply never attempting arithmetic on an unreached
node's distance in the first place.

---

# Part 1 — The File, Line by Line

```cpp
/**
 * @file BellmanFord.hpp
 * @brief Bellman-Ford shortest path algorithm.
 *
 * Finds the shortest paths from a single source vertex to all other
 * vertices in a weighted graph, supporting negative edge weights.
 * Detects negative cycles and throws if one is reachable from the source.
 *
 * Time Complexity: O(V * E)
 * Space Complexity: O(V)
 */
```

### 1.1 — Header comment
Pure documentation. "Supporting negative edge weights" is the headline
feature distinguishing this from Dijkstra. `O(V * E)` time — much worse
than Dijkstra's `O(E log V)`, which is the direct cost of giving up the
greedy/priority-queue approach. `O(V)` space.

```cpp
#pragma once
 
#include <unordered_map>
#include <vector>
#include <limits>
#include <stdexcept>
#include "Graph.hpp"
```

### 1.2 — Header guard and includes
`#pragma once` — the same double-inclusion guard from every file so far.
Includes: `<unordered_map>` for `dist`, `<vector>` for `allNodes`,
`<limits>` for `std::numeric_limits`, `<stdexcept>` for the exceptions,
and `"Graph.hpp"` for the `Graph` class. Notably absent: no `<queue>`,
`<functional>`, or `<algorithm>` — this algorithm needs none of a priority
queue, comparator functors, or `std::reverse`.

```cpp
/**
 * @brief Computes shortest paths from a single source node using the Bellman-Ford algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @param src The starting source node.
 * @return std::unordered_map<NodeType, WeightType, Hash> Map of shortest distances from src to all nodes.
 * @throws std::runtime_error If a negative weight cycle is detected reachable from src.
```

### 1.3 — Function documentation, part 1
Standard tags. Notice the `@return` type is a PLAIN `std::unordered_map`.
This function only ever computes distances; it has no `prev` map and
therefore no built-in way to reconstruct an actual path.

```cpp
 * WHY V-1 iterations of edge relaxation:
 * A shortest path in a graph with V vertices (without negative cycles) will visit at most
 * V - 1 edges. Each iteration of relaxing all edges propagates the correct distance estimates
 * by at least one edge hop. Thus, V - 1 iterations are mathematically guaranteed to find
 * the optimal shortest paths to all reachable vertices.
```

### 1.4 — The "why V-1 iterations" proof
**Claim 1: A shortest path (when no negative cycle exists) never visits
the same vertex twice.** If a path revisited some vertex, that means the path contains
a loop. If that loop's total weight were positive, simply deleting the loop
would make the path strictly cheaper. If the loop's weight were exactly
zero, deleting it wouldn't change the cost. The only way a loop could be
necessary would be if it had negative total weight — but we assumed no negative cycles
exist. So, the shortest path never needs to revisit any vertex.

**Claim 2: Therefore, a shortest path visits at most V distinct
vertices, meaning at most V−1 edges.** A sequence of at most V vertices has exactly V−1
connecting edges.

**Claim 3: One full pass of relaxing every edge extends "correctly-known
distances" by at least one extra hop.** After one full pass over all edges, any node
that's truly reachable in 1 hop from `src` is guaranteed to have its correct shortest
distance recorded. By induction, after `k` passes, every node whose true shortest path
uses `k` or fewer hops is guaranteed correct.

**Putting it together:** since Claim 2 guarantees every node's shortest
path uses at most V−1 hops, and Claim 3 guarantees `k` passes correctly
resolves every node within `k` hops — running exactly **V−1 full passes**
is mathematically guaranteed to correctly resolve every single node in
the graph.

```cpp
 * WHY the V-th iteration detects negative cycles:
 * If there are no negative weight cycles, all shortest paths have been finalized during the
 * V-1 passes, and a V-th pass will make zero modifications to the distance values. If any
 * distance decreases during the V-th relaxation pass:
 * `dist[u] + w < dist[v]`
 * it means there exists a path whose cost keeps shrinking infinitely by traversing more edges.
 * This indicates a reachable cycle with a net negative cost (a negative cycle).
```

### 1.5 — The negative-cycle detection proof
"IF there's no negative cycle, THEN V−1 passes are enough to finalize everything (so a
V-th pass changes nothing)." The contrapositive is: "IF a V-th pass DOES still change
something, THEN there must be a negative cycle." If you can go around some loop and have
your total cost go DOWN each time, then there's no such thing as a single best, finite
"shortest path" anymore at all.

```cpp
 * DIJKSTRA vs BELLMAN-FORD:
 * - Dijkstra's algorithm runs in O(E log V) time, which is much faster than Bellman-Ford's O(VE).
 *   Use Dijkstra when all edge weights are guaranteed to be non-negative.
 * - Bellman-Ford must be used when the graph contains negative weights or if you need to detect
 *   negative cycles.
 */
```

```cpp
template<typename NodeType, typename WeightType, typename Hash>
std::unordered_map<NodeType, WeightType, Hash> bellmanFord(
    const Graph<NodeType, WeightType, Hash>& g, NodeType src) {

    if (!g.hasNode(src)) {
        throw std::invalid_argument("Source node does not exist in the graph.");
    }
```

### 1.6 — Function signature and the entry guard
Standard three-template-parameter signature. Checks whether `src` actually exists,
and throws if it doesn't.

```cpp
    std::unordered_map<NodeType, WeightType, Hash> dist;
    const WeightType INF = std::numeric_limits<WeightType>::has_infinity 
                           ? std::numeric_limits<WeightType>::infinity() 
                           : std::numeric_limits<WeightType>::max();
 
    // Initialize distances
    for (const auto& node : g) {
        dist[node] = INF;
    }
    dist[src] = 0;
```

`dist` map initialization. Sets every node's distance to `INF`, except the source.

```cpp
    int V = g.nodeCount();
    std::vector<NodeType> allNodes = g.getAllNodes();
```

`V` stores the total number of vertices. `allNodes` caches every node in a vector to avoid
repeated query overhead.

```cpp
    // V - 1 iterations of edge relaxation
    for (int i = 0; i < V - 1; ++i) {
        bool anyChange = false;
```

### 1.7 — The main relaxation loop
The loop runs exactly `V - 1` times. `anyChange` is declared fresh on each pass
to track whether this pass made any improvements.

```cpp
        for (const auto& u : allNodes) {
            // Skip nodes that are currently unreachable to prevent underflow/overflow errors
            if (dist[u] == INF) {
                continue;
            }
 
            for (const auto& edge : g.neighbors(u)) {
                NodeType v = edge.to;
                WeightType w = edge.weight;
 
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    anyChange = true;
                }
            }
        }
```

Inside each pass, loop over all nodes `u`. If reached (`dist[u] != INF`), relax all
of `u`'s edges. If any edge is relaxed, update `dist[v]` and set `anyChange = true`.

Since updates are made in-place on `dist`, improvements can propagate further than one hop
per pass in a single iteration if nodes happen to be visited in topological order, leading
to faster practical convergence.

```cpp
        // Optimization: If no edge was relaxed in an iteration, we've already finalized all paths.
        if (!anyChange) {
            break;
        }
    }
```

If a full pass makes zero modifications, everything is already finalized. The `break` exits
the loop early, saving redundant passes.

```cpp
    // V-th iteration to check for negative cycles
    for (const auto& u : allNodes) {
        if (dist[u] == INF) {
            continue;
        }
 
        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            WeightType w = edge.weight;
 
            if (dist[u] + w < dist[v]) {
                throw std::runtime_error("Negative cycle detected");
            }
        }
    }
 
    return dist;
}
```

### 1.8 — The negative-cycle check and final return
One extra pass. If any edge can still be relaxed, a negative cycle exists. Immediately
throw `std::runtime_error`. If it completes with no cycle, return `dist`.

---

# Part 2 — Tracing It By Hand

### Trace A — Negative edge, no negative cycle (early break + clean exit)
Graph with V=4. `allNodes` = `[S, A, B, C]`:
```
S --1--> A
S --5--> B
A --(-3)--> B
B --2--> C
```
**Init:** `dist = {S:0, A:INF, B:INF, C:INF}`

**Pass i=0** (`anyChange = false`):
- `u=S` (dist=0): relax S→A: `dist[A]=1`, `anyChange=true`. Relax S→B: `dist[B]=5`, `anyChange=true`.
- `u=A` (dist=1): relax A→B: `1 + (-3) = -2 < 5` → `dist[B]=-2`, `anyChange=true`.
- `u=B` (dist=-2): relax B→C: `-2 + 2 = 0 < INF` → `dist[C]=0`, `anyChange=true`.
- `u=C` (dist=0): no edges.
- State: `dist = {S:0, A:1, B:-2, C:0}`

**Pass i=1** (`anyChange = false`):
- `u=S`: S→A (1 < 1? False), S→B (5 < -2? False).
- `u=A`: A→B (-2 < -2? False).
- `u=B`: B→C (0 < 0? False).
- `anyChange` is still `false` → `break` fires.

**V-th check pass:**
No improvements are found. No throw.
Returns `{S:0, A:1, B:-2, C:0}`.

---

### Trace B — An actual negative cycle (detection fires)
Graph with V=3:
```
S --1--> A
A --1--> B
B --(-3)--> A
```
A → B → A has a cycle of weight -2.

**Init:** `dist = {S:0, A:INF, B:INF}`

**Pass i=0:**
- `u=S`: S→A: `dist[A]=1`.
- `u=A` (dist=1): A→B: `dist[B]=2`.
- `u=B` (dist=2): B→A: `2 + (-3) = -1 < 1` → `dist[A]=-1`. `anyChange=true`.
- State: `dist = {S:0, A:-1, B:2}`

**Pass i=1:**
- `u=S`: S→A (1 < -1? False).
- `u=A` (dist=-1): A→B: `-1 + 1 = 0 < 2` → `dist[B]=0`. `anyChange=true`.
- `u=B` (dist=0): B→A: `0 + (-3) = -3 < -1` → `dist[A]=-3`. `anyChange=true`.
- Loop ends naturally. `anyChange` was `true`.
- State: `dist = {S:0, A:-3, B:0}`

**V-th check pass:**
- `u=S`: S→A (1 < -3? False).
- `u=A` (dist=-3): A→B: `-3 + 1 = -2 < 0` → **True!** → `throw std::runtime_error("Negative cycle detected")` fires.
Exits via exception.

---

# Key Takeaways & Summary

### Bellman-Ford Algorithm Summary

| Parameter | Details |
| :--- | :--- |
| **Core Strategy** | Brute-force relaxation of all edges $V-1$ times. |
| **Negative Weights** | Fully supported. |
| **Cycle Detection** | Detects negative-weight cycles reachable from the source on the $V$-th pass. |
| **Data Structures** | `std::unordered_map` for distances, `std::vector` to cache nodes. |
| **Time Complexity** | $O(V \cdot E)$ |
| **Space Complexity** | $O(V)$ |
| **API Action on Failure** | Throws `std::runtime_error` if a negative cycle is detected. |

> [!WARNING]
> While Bellman-Ford handles negative weights, it does so at the cost of a higher time complexity: $O(V \cdot E)$ compared to Dijkstra's $O(E \log V)$. Only choose Bellman-Ford when negative weights are possible or when cycle detection is explicitly required.
