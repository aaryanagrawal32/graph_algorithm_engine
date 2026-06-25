# Complete Line-by-Line Explanation of TopologicalSort.hpp
## Topological Sorting - From Absolute Beginner to Expert

---

## File Header & Documentation

```cpp
/**
 * @file TopologicalSort.hpp
 * @brief Topological Sort algorithms for Directed Acyclic Graphs (DAGs).
 *
 * Implements Kahn's BFS-based topological sort and DFS-based topological sort,
 * both including cycle detection mechanisms.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */
```

Documentation using Doxygen format.

**What is Topological Sorting?**
Ordering nodes in a directed graph so that if there's an edge from A → B, then A comes before B in the ordering.

**Real-world examples:**
- **Task scheduling**: Buy ingredients → Cook → Eat
- **Software dependency**: Library A depends on B, B on C. Install order: C → B → A
- **Course prerequisites**: CS 101 → CS 201 → CS 301

---

## Include Guards & Headers

```cpp
#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include "Graph.hpp"
```

- `<vector>`: For storing sorted results.
- `<queue>`: For Kahn's BFS-based algorithm.
- `<unordered_map>`: For tracking in-degrees and states.
- `<algorithm>`: For `std::reverse`.
- `<stdexcept>`: For error handling exceptions.
- `"Graph.hpp"`: Our custom Graph class.

---

## The TopologicalSortInternal Namespace

```cpp
namespace TopologicalSortInternal {
```

A "hidden" namespace for internal helper functions that users shouldn't call directly. Hides implementation details from the public API.

---

## The DFS Helper Function - Signature

```cpp
template<typename NodeType, typename WeightType, typename Hash>
bool topoDFSHelper(const Graph<NodeType, WeightType, Hash>& g, NodeType curr,
                   std::unordered_map<NodeType, int, Hash>& state,
                   std::vector<NodeType>& result) {
```

- `bool`: Returns `true` if cycle found, `false` otherwise.
- `g`: Read-only reference to the graph.
- `curr`: Current node being visited.
- `state`: Maps each node to its state: `0` (unvisited), `1` (visiting/in recursion stack), `2` (visited/fully processed).
- `result`: Accumulates sorted nodes.

---

## DFS Helper Function - Body Part 1

```cpp
    state[curr] = 1; // Mark as VISITING (active in stack)
```

Mark the current node as `1` (VISITING) the moment we start exploring it.
- **State 0 (UNVISITED)**: Not seen yet.
- **State 1 (VISITING)**: DFS in-progress, currently in call stack. A back-edge to this state indicates a cycle.
- **State 2 (VISITED)**: DFS returned, fully done.

---

## DFS Helper Function - Body Part 2

```cpp
    for (const auto& edge : g.neighbors(curr)) {
        if (state[edge.to] == 1) {
            return true; // Cycle detected: back-edge found
```

Loop through all neighbors. If a neighbor has state `1`, we found an edge pointing back to an active ancestor in the recursion stack — a cycle. Return `true` immediately.

---

## DFS Helper Function - Body Part 3

```cpp
        } else if (state[edge.to] == 0) {
            if (topoDFSHelper(g, edge.to, state, result)) {
                return true;
            }
        }
```

If a neighbor is unvisited (state `0`), recursively explore it. If the recursion finds a cycle, propagate `true` upward.

---

## DFS Helper Function - Body Part 4

```cpp
    state[curr] = 2; // Mark as VISITED (backtracked)
    result.push_back(curr); // Add node when all its descendants are fully processed
    return false;
```

After exploring all neighbors, mark `curr` as `2` (VISITED). Push it to the `result` vector (post-order). Return `false` (no cycle).

**Why push_back AFTER recursion?**
In DFS post-order traversal, the first node to be fully processed is a leaf node with no outgoing dependencies. By pushing after recursion, we get the reverse topological order: dependents come before dependencies. We reverse it at the end to get the correct order.

---

## Closing the DFS Helper Namespace

```cpp
}
// End of TopologicalSortInternal namespace
```

---

## Kahn's Algorithm Function - Signature & Documentation

```cpp
/**
 * @brief Performs topological sorting using Kahn's BFS-based algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @return std::vector<NodeType> Topologically sorted list of nodes.
 * @throws std::runtime_error If the graph is not a DAG (contains a cycle).
 *
 * WHY In-Degree:
 * The in-degree of a node is the number of incoming edges targeting it. A node with an in-degree
 * of 0 has no active dependencies and can be safely placed first. Kahn's algorithm works by
 * greedily extracting 0-in-degree nodes and removing their outgoing edges, updating neighbors.
 *
 * WHY cycle check works in Kahn's:
 * If the graph has a cycle, the nodes involved in the cycle will never have their in-degrees
 * reduced to 0 (since each node in the loop depends on another). Consequently, they will never
 * be pushed onto the queue. If the final sorted list size is less than the total node count V,
 * it indicates a cycle exists.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> topologicalSort(const Graph<NodeType, WeightType, Hash>& g) {
```

Kahn's is a BFS-based approach that tracks **in-degrees** (incoming edges). Nodes with in-degree `0` have no dependencies and can be safely processed first.

---

## Kahn's Algorithm - Input Validation

```cpp
    if (!g.isDirected()) {
        throw std::invalid_argument("Topological sort requires a directed graph.");
    }
```

Topological sort requires a directed graph since undirected graphs have no dependency direction.

---

## Kahn's Algorithm - Initialize In-Degrees

```cpp
    std::vector<NodeType> result;
    std::unordered_map<NodeType, int, Hash> inDegree;

    // Initialize all in-degrees to 0
    for (const auto& node : g) {
        inDegree[node] = 0;
    }
```

Sets up maps and initializes every node's in-degree to `0`.

---

## Kahn's Algorithm - Compute In-Degrees

```cpp
    // Compute in-degrees
    for (const auto& u : g) {
        for (const auto& edge : g.neighbors(u)) {
            inDegree[edge.to]++;
        }
    }
```

For every outgoing edge `u → v`, increment `inDegree[v]`.

---

## Kahn's Algorithm - Enqueue Zero In-Degree Nodes

```cpp
    // Queue nodes with 0 dependencies
    std::queue<NodeType> q;
    for (const auto& node : g) {
        if (inDegree[node] == 0) {
            q.push(node);
        }
    }
```

Finds all nodes with in-degree `0` and adds them to a FIFO queue to seed the BFS.

---

## Kahn's Algorithm - Main Loop

```cpp
    while (!q.empty()) {
        NodeType u = q.front();
        q.pop();
        result.push_back(u);

        // Remove edge u->v
        for (const auto& edge : g.neighbors(u)) {
            NodeType v = edge.to;
            inDegree[v]--;
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }
```

- Pop node `u` from `q` and append to `result`.
- For every neighbor `v` of `u`, decrement `inDegree[v]` (simulating removing edge `u → v`).
- If `inDegree[v]` becomes `0`, enqueue `v`.

---

## Kahn's Algorithm - Cycle Detection

```cpp
    // Cycle detection check
    if (static_cast<int>(result.size()) != g.nodeCount()) {
        throw std::runtime_error("Graph contains a cycle (not a DAG)");
    }

    return result;
}
```

If `result.size() != g.nodeCount()`, some nodes were never processed. This occurs because nodes involved in a cycle depend on each other, so their in-degrees can never be reduced to `0` — they are never enqueued. Throw `std::runtime_error`. Otherwise, return `result`.

---

## DFS-Based Topological Sort - Signature & Documentation

```cpp
/**
 * @brief Performs topological sorting using recursive DFS.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @return std::vector<NodeType> Topologically sorted list of nodes.
 * @throws std::runtime_error If the graph contains a cycle.
 *
 * HOW cycle detection works in DFS:
 * Uses 3-state coloring. Nodes currently undergoing search are marked VISITING. If DFS traverses
 * an edge pointing to a node marked VISITING, it represents a back-edge (looping back to a parent
 * still active in the call stack), indicating a cycle.
 *
 * DIFFERENCE between Kahn's and DFS-based output:
 * Both produce valid topological orderings, but Kahn's BFS-based traversal processes nodes level-by-level
 * (placing nodes with fewer dependencies as early as possible). DFS explores paths fully to the bottom
 * first. If there are multiple valid topological sorts, Kahn's and DFS may return different valid permutations.
 */
template<typename NodeType, typename WeightType, typename Hash>
std::vector<NodeType> topologicalSortDFS(const Graph<NodeType, WeightType, Hash>& g) {
```

DFS-based topological sort uses recursion and back-edge detection. It can produce different (but equally valid) permutations compared to Kahn's.

---

## DFS-Based Sort - Input Validation

```cpp
    if (!g.isDirected()) {
        throw std::invalid_argument("Topological sort requires a directed graph.");
    }
```

---

## DFS-Based Sort - Initialize State Map

```cpp
    std::vector<NodeType> result;
    std::unordered_map<NodeType, int, Hash> state; // 0 = unvisited, 1 = visiting, 2 = visited

    for (const auto& node : g) {
        state[node] = 0;
    }
```

All nodes start as `0` (UNVISITED).

---

## DFS-Based Sort - Main DFS Loop

```cpp
    for (const auto& node : g.getAllNodes()) {
        if (state[node] == 0) {
            if (TopologicalSortInternal::topoDFSHelper(g, node, state, result)) {
                throw std::runtime_error("Graph contains a cycle (not a DAG)");
            }
        }
    }
```

Loops over all nodes. If unvisited, call `topoDFSHelper` to explore. If a cycle is detected, throw `std::runtime_error`. This loop structure correctly handles disconnected components.

---

## DFS-Based Sort - Reverse Result

```cpp
    // The DFS helper pushes nodes to the result list when backtracking (bottom-up).
    // Reversing the list gives the correct top-down dependency order.
    std::reverse(result.begin(), result.end());
    return result;
}
```

Reverses the post-order list in-place to get correct dependency-first ordering, then returns it.

---

## Summary Table: Kahn's vs DFS

| Aspect | Kahn's (BFS) | DFS |
|--------|--------------|-----|
| **Approach** | In-degree based, queue | Recursive, stack (call stack) |
| **Cycle Detection** | Check if all nodes processed | Check for back-edge (state=1) |
| **Data Structure** | Queue of 0-in-degree nodes | Implicit call stack, state map |
| **Output Order** | Level-by-level | Depth-first |
| **Time Complexity** | O(V + E) | O(V + E) |
| **Space Complexity** | O(V) | O(V) |

---

## Real-World Topological Sort Examples

### Example 1: Build System Dependencies
Source files: `main.cpp`, `utils.cpp`, `lib.cpp`
Dependencies: `main` depends on `lib` and `utils`; `lib` depends on `utils`.
Graph: `main → lib`, `main → utils`, `lib → utils`.
Topological sort: `[utils, lib, main]`. (Compile `utils`, then `lib`, then `main`).

### Example 2: Course Prerequisites
CS 101 → CS 201 → CS 301 → CS 401, and CS 201 → CS 401.
Topological sort: `[CS101, CS201, CS301, CS401]`.

### Example 3: Detecting Circular Dependencies
Module A → B → C → A.
In-degrees all start at 1. No node has in-degree 0. Queue starts empty. Loop never runs.
`result.size() = 0 != g.nodeCount() = 3` → throws "Circular dependency detected!".

---

## Key Concepts Summary

### 3-State DFS Coloring
- **State 0 (White/Unvisited)**: Unseen.
- **State 1 (Gray/Visiting)**: DFS active. Back-edge detected if seen again.
- **State 2 (Black/Visited)**: DFS returned. Fully done.

### In-Degree Concept
Number of incoming edges. Nodes with in-degree 0 are ready to process.

### Post-Order DFS Traversal
Pushing nodes to the list during backtracking (bottom-up), then reversing it at the end to get top-down ordering.

---

## Common Interview Questions

**Q1: When would you use Kahn's vs DFS topological sort?**
A: Kahn's BFS is more intuitive and easy to trace. DFS is more elegant, uses recursion, and fits naturally with DFS-based algorithms.

**Q2: What if there are multiple valid topological sorts?**
A: Any ordering where prerequisites come first is valid. Different orderings of 0-in-degree nodes yield different valid sorts.

**Q3: How would you detect cycles in a directed graph?**
A: Kahn's: if `result.size() != V`. DFS: if a state 1 node is visited again. Both are O(V+E).

**Q4: Why can't topological sort work on undirected graphs?**
A: Undirected edges have no direction, meaning there are no ordering constraints or prerequisites.

---

# Key Takeaways & Summary

### Topological Sort: Kahn's vs. DFS Comparison

| Feature / Metric | Kahn's Algorithm (BFS-based) | DFS-based Algorithm |
| :--- | :--- | :--- |
| **Core Idea** | Repeatedly removes nodes with an in-degree of 0. | Post-order DFS traversal, reversing the final order. |
| **Cycle Detection** | Natural: if the sorted list size $< V$, a cycle exists. | Explicit: requires a 3-state coloring tracker (`visiting`). |
| **Data Structures** | `std::queue` (stores 0-in-degree nodes), in-degree map. | Implicit call stack, state map, result vector. |
| **Time Complexity** | $O(V + E)$ | $O(V + E)$ |
| **Space Complexity** | $O(V)$ | $O(V)$ |
| **Implementation Style** | Iterative. | Recursive helper. |

> [!TIP]
> Use **Kahn's algorithm** when you need an iterative approach and a straightforward cycle detection check. Kahn's is also intuitive because it processes nodes from source to sink in the natural dependency order. Use the **DFS-based approach** if you already have a DFS framework in place, but remember that you must explicitly reverse the result vector at the end of the search.
