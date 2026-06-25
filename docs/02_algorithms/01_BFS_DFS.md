# DFS.hpp and BFS.hpp — Complete Line-by-Line Explanation

This walks through **every single line** of both files. Part 0 covers the
C++ building blocks you'll see over and over, so you're not re-explained
the same thing five times. Parts 1 and 2 go top to bottom through each file.
Part 3 traces both algorithms by hand on a tiny graph so the abstract code
turns into something you can picture.

---

# Part 0 — C++ Building Blocks Used Everywhere in This Code

Read this once before Part 1. Every concept here will reappear constantly.

### `#pragma once`
A "header guard." If this file gets `#include`d twice (which happens
easily in a large project — File A includes it, File B includes it, and
both A and B get included by File C), the compiler would otherwise paste
the same class/function definitions in twice and fail with "redefinition"
errors. `#pragma once` tells the compiler: "if you've already pasted this
file in once during this compilation, skip it the second time."

### `#include`
Copies the contents of another file in, at that exact point, before
compiling. `#include <vector>` pulls in a *standard library* file (angle
brackets = "search the system's installed C++ libraries").
`#include "Graph.hpp"` pulls in *your own* file (quotes = "look in my own project
folder first").

### `template<typename NodeType, typename WeightType, typename Hash>`
This is C++'s way of writing one function/class that works for many
types, without copy-pasting it. Read it as: "Whatever follows below
should work no matter what concrete type someone plugs in for `NodeType`,
`WeightType`, and `Hash`."

Concretely: your `Graph` is generic. Someone might create a
`Graph<int, double>` (nodes are integers, edge weights are doubles) or
a `Graph<std::string, int>` (nodes are city names, weights are integers).
Rather than writing a separate `dfs()` function for every possible
combination, you write it ONE time with placeholder type names
(`NodeType`, `WeightType`, `Hash`), and the compiler generates the real
version automatically the first time you actually call `dfs()` with
specific types. This is called **template instantiation** and it happens
at compile time — there's no runtime cost for this flexibility.

`Hash` specifically is the *hashing function* used internally by
`unordered_set`/`unordered_map` to decide where to store a given
`NodeType` value in memory. For simple types like `int`, the default hash
works fine. For custom types, you sometimes need to supply your own —
which is why `Graph` takes `Hash` as a template parameter instead of
hardcoding one.

### `namespace DFSInternal { ... }`
A namespace is a named "box" you put names (functions, in this case) into,
so they don't collide with identically-named things elsewhere in a big
project. Anything inside `namespace DFSInternal { }` must be referred to
as `DFSInternal::dfsHelper(...)` from outside that box (you'll see this
exact syntax used later, e.g. `DFSInternal::dfsHelper(g, start, visited, visitOrder);`).

The *purpose* here is signaling "these are implementation details, not
part of the public API." `dfsHelper`, `hasCycleDirectedHelper`, and
`hasCycleUndirectedHelper` are helper functions that `dfs()`, `hasCycle()`
etc. use internally — a user of this header is never meant to call
`DFSInternal::dfsHelper` directly.

### `const Graph<NodeType, WeightType, Hash>& g`
Three things stacked together — read right to left:
- `g` is the parameter name.
- `&` means **pass by reference**: instead of copying the entire `Graph`
  object (which could be huge — millions of nodes/edges) into the
  function, `g` is just an alias for the original object sitting
  somewhere else in memory. No copy happens. This is both a memory and
  speed optimization.
- `const` in front means "through this alias, you promise not to modify
  the original object." The compiler will give you an error if your
  function body tries to call a non-const method on `g` (like adding a
  node). This is a safety contract: anyone reading the function signature
  instantly knows "this function only reads the graph, it never changes it."

### `std::unordered_set<NodeType, Hash>& visited`
`std::unordered_set` is a collection that stores unique values (no
duplicates) and lets you check "is X in this collection?" extremely fast
— on average, O(1) (constant time), regardless of how many items are in
it. It does this using **hashing**: it converts each value into a number
(the "hash"), and uses that number to jump almost directly to where the
value would be stored, instead of scanning the whole collection.

This is exactly what you want for a "have I visited this node before?"
`visited` here is the SAME set the caller has, not a fresh empty one.
This matters a lot: it lets `dfsHelper` accumulate visited nodes that
*persist* across recursive calls into itself.

### `std::vector<NodeType>& visitOrder`
`std::vector` is a dynamically-resizable array — like a list that grows
as you add to it. `push_back(x)` appends `x` to the end. Same reference
logic as above: this is the caller's vector, being filled in by the
function.

### `for (const auto& edge : g.neighbors(curr))`
This is a **range-based for loop**, introduced in C++11. It means: "for
each element in `g.neighbors(curr)`, call it `edge`, and run the loop
body." `g.neighbors(curr)` returns a collection of edges leaving node
`curr`. `auto` tells the compiler "figure out the type yourself".
`const auto&` means "give me a read-only reference to each edge, don't copy it."

### `visited.find(edge.to) == visited.end()`
This is the idiomatic way to ask "is `edge.to` NOT in the `visited` set?"
`.find(x)` searches for `x` and returns an **iterator** pointing to it if
found, or a special "not found" iterator if `x` isn't there. That special
"not found" iterator is exactly what `.end()` returns. So:
- `visited.find(x) == visited.end()` → x is NOT in the set
- `visited.find(x) != visited.end()` → x IS in the set

(Newer C++ has a simpler `visited.contains(x)`, but this codebase uses the
older, more universally-compatible `.find() == .end()` idiom.)

### `bool`, `true`, `false`
`bool` is a type that holds exactly one of two values: `true` or `false`.
Functions like `hasCycle` return `bool` because the answer to "does this
graph have a cycle?" is inherently yes/no.

### Recursion
A function that calls itself. `dfsHelper` calls `dfsHelper` again (with a
different `curr` argument) inside its own body. Each call gets its own
private copy of local variables and waits for the call it made to finish
before continuing — this waiting chain is managed automatically by the
**call stack**. This matters a lot for DFS.

---

# Part 1 — `DFS.hpp`, Line by Line

```cpp
/**
 * @file DFS.hpp
 * @brief Depth-First Search (DFS) algorithms.
 *
 * Implements recursive and iterative DFS traversals, cycle detection,
 * and connectivity checks for the generic Graph class template.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 *
 * WHY USE THIS: DFS is fundamental for pathfinding, topological sorting,
 * cycle detection, and connected components.
 */
```

### 1.1 — The header comment block

This entire block is a **comment** — text the compiler completely ignores,
written for humans. It's wrapped in `/** ... */` (a "Doxygen-style" block
comment).

- `@file DFS.hpp` — documents which file this comment describes.
- `@brief Depth-First Search (DFS) algorithms.` — a one-line summary.
- The paragraph after that explains what's inside this file in plain
  English: recursive DFS, iterative DFS, cycle detection, connectivity
  checks, and that all of it works on the generic `Graph` template.
- `Time Complexity: O(V + E)` — `V` = number of vertices (nodes),
  `E` = number of edges. `O(V + E)` means: the total work is roughly
  proportional to (number of nodes) + (number of edges) — every node gets
  visited once, and every edge gets examined once. This is the best
  possible complexity for any algorithm that has to look at the whole
  graph at least once.
- `Space Complexity: O(V)` — how much *extra memory* the algorithm needs.
  Here it's proportional to the number of vertices — that's the `visited`
  set and the recursion stack (or explicit stack), each of which can hold
  at most one entry per node.
- `WHY USE THIS: ...` — practical motivation.

```cpp
#pragma once
```

### 1.2 — Header guard
Explained in Part 0. Prevents double-inclusion errors.

```cpp
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "Graph.hpp"
```

### 1.3 — Includes
Each line pulls in one piece of standard library functionality this file
needs:
- `<vector>` → `std::vector`, used for `visitOrder` and the path/result vectors.
- `<stack>` → `std::stack`, used by `dfsIterative` to manually manage the
  exploration order, since that function avoids recursion.
- `<unordered_set>` → `std::unordered_set`, used everywhere as the `visited` tracker.
- `<unordered_map>` → `std::unordered_map`, used in `hasCycleDirectedHelper`
  to track each node's 3-state "color" (unvisited / visiting / visited).
- `<queue>` → `std::queue`, used in `isConnected` (which does a BFS-style
  traversal internally, even inside a file mostly about DFS).
- `"Graph.hpp"` → your own Graph class definition.

```cpp
namespace DFSInternal {
```

### 1.4 — Open the internal namespace
Explained in Part 0. Helper functions not meant to be called directly by users.

---

### 1.5 — `dfsHelper`

```cpp
    /**
     * @brief Helper function for recursive DFS traversal.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    void dfsHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr,
                   std::unordered_set<NodeType, Hash>& visited,
                   std::vector<NodeType>& visitOrder) {
```

This is the function **signature** (its name + parameters + return type).
- `void` — this function returns nothing. It does its work entirely by
  *modifying* the `visited` set and `visitOrder` vector it was given by reference.
- `dfsHelper(...)` — the name, and four parameters:
  1. `g` — the graph, read-only, by reference.
  2. `curr` — the node we're currently visiting. Not a reference because it's cheap to copy.
  3. `visited` — the shared, accumulating set of nodes seen so far.
  4. `visitOrder` — the shared, accumulating list of nodes in visit order.

```cpp
        visited.insert(curr);
        visitOrder.push_back(curr);
```

The first thing this function does upon being called with some `curr`:
mark `curr` as visited, and record it in the output order.

```cpp
        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                // Recursion stack implicitly tracks traversal path and handles backtracking.
                dfsHelper(g, edge.to, visited, visitOrder);
            }
        }
    }
```

Now loop over every edge leaving `curr`:
- `edge.to` is the node on the other end of this edge.
- The `if` checks: "have we NOT visited this neighbor yet?"
- If unvisited, call `dfsHelper` again — but this time with `edge.to` as the new `curr`.

**Why this produces a depth-first order:** Think of standing at node A
with neighbors B and C. The loop reaches B first. Instead of finishing the
loop and then going to C, it immediately dives into `dfsHelper(B)` — which
itself will fully explore everything reachable from B BEFORE returning control.

**Why no separate "stack" variable appears:**
"Recursion stack implicitly tracks traversal path and handles backtracking."
When a function calls itself, the *call stack* remembers exactly which
calls are waiting, managing backtracking automatically.

---

### 1.6 — `hasCycleDirectedHelper`

```cpp
    /**
     * @brief Helper function to detect cycles in a directed graph using 3-state coloring.
     * State values: 0 = UNVISITED, 1 = VISITING (currently in recursion stack), 2 = VISITED.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    bool hasCycleDirectedHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr,
                                std::unordered_map<NodeType, int, Hash>& state) {
```

Same signature pattern as before, but:
- Returns `bool` — "was a cycle found starting from this exploration?"
- Takes a `state` map instead of a `visited` set. This map stores for
  each node: `0` (never seen), `1` (currently being explored), `2` (fully explored and safe).

**Why three states?** In a *directed* graph, A→B doesn't imply B→A. A cycle
means: starting from some node, you eventually get back to a node you're
*still in the middle of exploring*. Two states cannot distinguish this.

```cpp
        state[curr] = 1; // Mark as VISITING
```

The moment we start exploring `curr`, mark it `1` (visiting).

```cpp
        for (const auto& edge : g.neighbors(curr)) {
            if (state[edge.to] == 1) {
                // Found a back-edge pointing to an ancestor in the active call stack
                return true;
            } else if (state[edge.to] == 0) {
                if (hasCycleDirectedHelper(g, edge.to, state)) {
                    return true;
                }
            }
        }
```

For each neighbor of `curr`:
- If that neighbor's state is `1` (VISITING), we've found a back-edge. Return `true`.
- Else if that neighbor's state is `0` (UNVISITED), recurse into it. If that
  finds a cycle, propagate `true` upward immediately.

```cpp
        state[curr] = 2; // Mark as VISITED (and backtrack)
        return false;
    }
```

If the loop finishes without finding any cycle, mark `curr` as fully done (`2`)
and return `false`.

---

### 1.7 — `hasCycleUndirectedHelper`

```cpp
    /**
     * @brief Helper function to detect cycles in an undirected graph using parent tracking.
     */
    template<typename NodeType, typename WeightType, typename Hash>
    bool hasCycleUndirectedHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr, NodeType parent,
                                  std::unordered_set<NodeType, Hash>& visited) {
```

Different strategy for **undirected** graphs (edges go both ways). Here we use
a simple `visited` set, but we add a new parameter: `parent` — the node we
just came FROM on our way to `curr`.

**Why undirected graphs need parent tracking:** In an undirected graph,
every edge traversed forward (A→B) also exists backward (B→A). If you used
directed logic, the edge you just walked across would always look like a cycle.
You need to specifically ignore "the edge I just came from".

```cpp
        visited.insert(curr);

        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                if (hasCycleUndirectedHelper(g, edge.to, curr, visited)) {
                    return true;
                }
            } else if (edge.to != parent) {
                // Reached an already visited node that is NOT the direct parent of this node.
                // This means there is another path to it (a cycle).
                return true;
            }
        }
        return false;
    }
```

- Mark `curr` visited.
- For each neighbor:
  - If unvisited: recurse, passing `curr` as the new `parent`.
  - Else if it IS visited, but is NOT our `parent`: this is a cycle. Return `true`.
- If the loop finishes with no cycle found, return `false`.

```cpp
}
```

### 1.8 — Closing brace of `namespace DFSInternal`

---

### 1.9 — `dfs()` — public entry point for recursive DFS

```cpp
/**
 * @brief Performs a recursive DFS traversal starting from a given node.
 ...
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> dfs(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    std::vector<NodeType> visitOrder;
    if (!g.hasNode(start)) {
        return visitOrder;
    }
    std::unordered_set<NodeType, Hash> visited;
    DFSInternal::dfsHelper(g, start, visited, visitOrder);
    return visitOrder;
}
```

This is the function a user actually calls, outside `namespace DFSInternal`.
- Returns `std::vector<NodeType>` by value.
- `if (!g.hasNode(start)) { return visitOrder; }` — **edge case guard**.
- `std::unordered_set<NodeType, Hash> visited;` — create a fresh, empty visited tracker.
- `DFSInternal::dfsHelper(g, start, visited, visitOrder);` — hand off work.
- `return visitOrder;` — return the completed vector.

**Why split into a thin public `dfs()` and an internal `dfsHelper`?**
Because the recursive helper needs extra state (`visited` and `visitOrder`)
threaded through every recursive call. The public `dfs()` hides this setup.

---

### 1.10 — `dfsIterative()` — DFS without recursion

```cpp
/**
 * @brief Performs an iterative DFS traversal starting from a given node using an explicit stack.
 ...
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> dfsIterative(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    std::vector<NodeType> visitOrder;
    if (!g.hasNode(start)) {
        return visitOrder;
    }

    std::unordered_set<NodeType, Hash> visited;
    std::stack<NodeType> s;

    s.push(start);
```

Manages its own explicit `std::stack` object instead of the C++ call stack.
This version can visit nodes in a slightly different sibling order due to push
order and visited mark timing.

```cpp
    while (!s.empty()) {
        NodeType curr = s.top();
        s.pop();
```

The main loop: keep going as long as the stack is not empty.

```cpp
        if (visited.find(curr) == visited.end()) {
            visited.insert(curr);
            visitOrder.push_back(curr);
```

**Important subtlety:** here we check visited-ness only AFTER popping, not
before pushing. A node can be pushed multiple times before it is popped,
so this check at pop-time prevents redundant processing.

```cpp
            const auto& neighbors = g.neighbors(curr);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (visited.find(it->to) == visited.end()) {
                    s.push(it->to);
                }
            }
        }
    }
    return visitOrder;
}
```

- `neighbors.rbegin()` and `neighbors.rend()` iterate **back to front** (reverse iterator).
  Since a stack is LIFO (Last-In-First-Out), pushing in reverse order means
  they come off the stack in their original left-to-right order, mimicking recursion.
- `it->to` gets the target node.
- Once the stack is empty, return `visitOrder`.

---

### 1.11 — `hasCycle()` — public cycle detection

```cpp
/**
 * @brief Detects if the graph contains any cycles.
 * Supports both directed and undirected graphs.
 ...
 */
template<typename NodeType, typename WeightType, typename Hash>
bool hasCycle(const Graph<NodeType, WeightType, Hash>& g) {
```

Takes only the graph, because a cycle could exist anywhere, possibly
unreachable from a single start node. It scans all nodes as starting points.

```cpp
    if (g.isDirected()) {
        std::unordered_map<NodeType, int, Hash> state; // 0 = unvisited, 1 = visiting, 2 = visited
        for (const auto& node : g.getAllNodes()) {
            if (state[node] == 0) {
                if (DFSInternal::hasCycleDirectedHelper(g, node, state)) {
                    return true;
                }
            }
        }
```

- For directed graphs, create a shared `state` map.
- Loop over all nodes from `g.getAllNodes()`.
- `state[node] == 0` — only explore if unvisited (accessing map with `[]` defaults to 0).
- If any starting node leads to a cycle, return `true`.

```cpp
    } else {
        std::unordered_set<NodeType, Hash> visited;
        for (const auto& node : g.getAllNodes()) {
            if (visited.find(node) == visited.end()) {
                // For undirected cycle detection, we set the initial parent to itself
                if (DFSInternal::hasCycleUndirectedHelper(g, node, node, visited)) {
                    return true;
                }
            }
        }
    }
    return false;
}
```

- If undirected: use a shared `visited` set.
- Pass `node` as both `curr` and `parent` to start the search.
- If no cycles are found, return `false`.

---

### 1.12 — `isConnected()` — weak/strong connectivity

```cpp
/**
 * @brief Checks if the graph is connected.
 * For directed graphs, it checks for weak connectivity (ignoring edge direction).
 ...
 */
template<typename NodeType, typename WeightType, typename Hash>
bool isConnected(const Graph<NodeType, WeightType, Hash>& g) {
    if (g.nodeCount() <= 1) {
        return true;
    }
```

A graph with 0 or 1 nodes is connected.

```cpp
    std::vector<NodeType> allNodes = g.getAllNodes();
    NodeType start = allNodes[0];

    std::unordered_set<NodeType, Hash> visited;
    std::queue<NodeType> q;

    q.push(start);
    visited.insert(start);
```

Pick the first node as a starting point. Setup a `visited` set and a `queue`
for BFS. Seed them with the start node.

```cpp
    while (!q.empty()) {
        NodeType curr = q.front();
        q.pop();
```

Standard BFS loop: get oldest item (`q.front()`) and pop it.

```cpp
        // Standard outgoing neighbors
        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                q.push(edge.to);
            }
        }
```

Expand outgoing neighbors.

```cpp
        // If directed, we must check incoming edges as well to ensure weak connectivity
        if (g.isDirected()) {
            for (const auto& otherNode : allNodes) {
                if (visited.find(otherNode) == visited.end()) {
                    // Check if otherNode has an edge targeting curr
                    if (g.hasEdge(otherNode, curr)) {
                        visited.insert(otherNode);
                        q.push(otherNode);
                    }
                }
            }
        }
    }
```

This implements **weak connectivity** for directed graphs:
- Walk through all nodes.
- If a node is not yet visited, check if there is an incoming edge to `curr` (`g.hasEdge(otherNode, curr)`).
- If so, treat it as connected, mark visited, and enqueue.
- (Runs in O(V) per node, O(V²) overall for directed graphs — a brute-force approach).

```cpp
    return static_cast<int>(visited.size()) == g.nodeCount();
}
```

After the BFS exhausts everything, check if we visited every node in the graph.
`static_cast<int>(visited.size())` avoids signed/unsigned comparison warnings.
If the counts match, return `true`; else `false`.

---

# Part 2 — `BFS.hpp`, Line by Line

```cpp
/**
 * @file BFS.hpp
 * @brief Breadth-First Search (BFS) algorithms.
 *
 * Implements basic BFS traversal and BFS shortest path algorithms for
 * the generic Graph class template.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 *
 * WHY USE THIS: Useful for level-order traversals and finding shortest
 * paths in UNWEIGHTED graphs.
 */
```

### 2.1 — Header comment
Same structure and purpose as DFS.hpp. BFS visits nodes layer by layer
outward, making it ideal for shortest paths when every edge has equal weight.

```cpp
#pragma once

#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "Graph.hpp"
```

### 2.2 — Header guard and includes
Standard includes. `<algorithm>` brings in `std::reverse`.

---

### 2.3 — `bfs()`

```cpp
/**
 * @brief Performs a Breadth-First Search (BFS) traversal starting from a given node.
 ...
 * WHY queue (FIFO): A queue ensures level-order traversal.
 *
 * WHY mark visited on PUSH, not POP: Marking visited when pushing to the queue prevents the
 * same node from being pushed multiple times if it has multiple incoming edges from the current level.
 * This keeps the queue size bounded by O(V) and avoids redundant work.
 */
```

- **Why a queue:** FIFO ensures level-order traversal.
- **Why mark visited at push-time:** Prevents duplicate entries when a node is reachable
  from multiple nodes at the same level.

```cpp
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> bfs(const Graph<NodeType, WeightType, Hash>& g, NodeType start) {
    std::vector<NodeType> visitOrder;

    // Edge Case: If the start node is not present in the graph, return an empty traversal
    if (!g.hasNode(start)) {
        return visitOrder;
    }

    std::unordered_set<NodeType, Hash> visited;
    std::queue<NodeType> q;

    // Initialize search
    q.push(start);
    visited.insert(start);
```

Standard BFS setup, marking `start` visited immediately on push.

```cpp
    while (!q.empty()) {
        NodeType curr = q.front();
        q.pop();
        visitOrder.push_back(curr);
```

FIFO pop, recording node immediately without re-checking.

```cpp
        // Traverse all neighbors of the current node
        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to); // Mark visited immediately on push
                q.push(edge.to);
            }
        }
    }

    return visitOrder;
}
```

Enqueues unvisited neighbors and marks them visited immediately.

---

### 2.4 — `bfsShortestPath()`

```cpp
/**
 * @brief Computes the shortest path from a source node to a destination node using BFS.
 ...
 * WHY BFS finds shortest path only in UNWEIGHTED graphs:
 * BFS finds the path with the fewest edges. In an unweighted graph, all edge weights are equal,
 * so minimum hop count is equivalent to minimum path cost. For weighted graphs, use Dijkstra.
 *
 * WHY prev map helps reconstruct path:
 * Stores prev[neighbor] = current to trace back parents, allowing O(path_length) path recovery.
 */
```

Computes the actual sequence of nodes forming the shortest path.
- **Why unweighted only:** BFS minimizes edges crossed. In weighted graphs, a path with more
  edges can have a smaller total weight.
- **Why prev map:** Reconstructs the path by walking backward from `dst` to `src` using parent pointers.

```cpp
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> bfsShortestPath(const Graph<NodeType, WeightType, Hash>& g, NodeType src, NodeType dst) {
    std::vector<NodeType> path;

    // Edge Case: Ensure both source and destination exist in the graph
    if (!g.hasNode(src) || !g.hasNode(dst)) {
        return path;
    }
```

Guard against missing endpoints.

```cpp
    // Edge Case: Source is the destination
    if (src == dst) {
        path.push_back(src);
        return path;
    }
```

Trivial path of length 0.

```cpp
    std::unordered_set<NodeType, Hash> visited;
    std::unordered_map<NodeType, NodeType, Hash> prev;
    std::queue<NodeType> q;

    q.push(src);
    visited.insert(src);

    bool reachedDst = false;
```

Setup search variables and `reachedDst` flag.

```cpp
    while (!q.empty()) {
        NodeType curr = q.front();
        q.pop();

        if (curr == dst) {
            reachedDst = true;
            break;
        }
```

Early exit check: if we pop `dst`, we've found the shortest path.

```cpp
        for (const auto& edge : g.neighbors(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                prev[edge.to] = curr;
                q.push(edge.to);
            }
        }
    }
```

Record `prev[edge.to] = curr` on discovery.

```cpp
    // If destination was not reached, return empty vector
    if (!reachedDst) {
        return path;
    }
```

Check if destination was reached.

```cpp
    // Reconstruct the shortest path by walking backward from dst to src
    NodeType curr = dst;
    while (curr != src) {
        path.push_back(curr);
        curr = prev[curr];
    }
    path.push_back(src);
```

Walk backward from `dst` to `src` using `prev` pointers.

```cpp
    // Reverse path to change order from [dst -> ... -> src] to [src -> ... -> dst]
    std::reverse(path.begin(), path.end());

    return path;
}
```

`std::reverse` flips it in-place to get `[src, ..., dst]`.

---

# Part 3 — Tracing Both Algorithms by Hand

Picture this tiny undirected graph:

```
        A
       / \
      B   C
      |   |
      D   E
       \ /
        F
```

Edges: A-B, A-C, B-D, C-E, D-F, E-F.

### Tracing `dfs(g, A)`

```
dfsHelper(A): visit A. visitOrder = [A]
  neighbor B unvisited → dfsHelper(B)
    dfsHelper(B): visit B. visitOrder = [A, B]
      neighbor A already visited → skip
      neighbor D unvisited → dfsHelper(D)
        dfsHelper(D): visit D. visitOrder = [A, B, D]
          neighbor B already visited → skip
          neighbor F unvisited → dfsHelper(F)
            dfsHelper(F): visit F. visitOrder = [A, B, D, F]
              neighbor D already visited → skip
              neighbor E unvisited → dfsHelper(E)
                dfsHelper(E): visit E. visitOrder = [A, B, D, F, E]
                  neighbor C unvisited → dfsHelper(C)
                    dfsHelper(C): visit C. visitOrder = [A, B, D, F, E, C]
                      neighbor A already visited → skip
                      neighbor E already visited → skip
```

Final result: `[A, B, D, F, E, C]`. Plunges to F before exploring C (depth-first). Automatic backtracking unwinds the call stack.

### Tracing `bfs(g, A)`

```
Start: q = [A], visited = {A}
Pop A. visitOrder = [A] → enqueue B, C. q=[B,C], visited={A,B,C}
Pop B. visitOrder = [A,B] → enqueue D. q=[C,D], visited={A,B,C,D}
Pop C. visitOrder = [A,B,C] → enqueue E. q=[D,E], visited={A,B,C,D,E}
Pop D. visitOrder = [A,B,C,D] → enqueue F. q=[E,F], visited={A,B,C,D,E,F}
Pop E. visitOrder = [A,B,C,D,E] → F already visited, skip. q=[F]
Pop F. visitOrder = [A,B,C,D,E,F] → neighbors visited, skip. q=[]
Done.
```

Final result: `[A, B, C, D, E, F]` — layer by layer.

### Tracing `bfsShortestPath(g, A, F)`

```
prev[B]=A, prev[C]=A, prev[D]=B, prev[E]=C, prev[F]=D
F is popped → reachedDst=true, break.
Reconstruction: F ← D ← B ← A
Reverse: [A, B, D, F]
```

Shortest path: `A → B → D → F` (3 edges).

---

# Key Takeaways & Summary

### DFS vs. BFS Comparison

| Metric / Feature | Depth-First Search (DFS) | Breadth-First Search (BFS) |
| :--- | :--- | :--- |
| **Core Strategy** | Explores as deep as possible before backtracking. | Explores neighbors layer-by-layer (level-order). |
| **Data Structure** | Stack (implicit call stack or `std::stack`). | Queue (`std::queue`). |
| **Time Complexity** | $O(V + E)$ | $O(V + E)$ |
| **Space Complexity** | $O(V)$ (worst-case call stack depth). | $O(V)$ (worst-case queue width). |
| **Shortest Path?** | No (only finds *a* path). | Yes (only for **unweighted** graphs). |
| **Primary Use Cases** | Cycle detection, topological sorting, maze solving. | Unweighted shortest path, connected components. |

> [!TIP]
> Use **DFS** when memory is a constraint on wide trees, or when you need to explore deep paths. Use **BFS** when you want to find the shortest path in terms of hop count or need to explore nodes close to the source first.
