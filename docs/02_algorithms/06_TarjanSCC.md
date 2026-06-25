# TarjanSCC.hpp — Line-by-Line Deep Explanation

## What Is This File?

`TarjanSCC.hpp` implements **Tarjan's Strongly Connected Components (SCC) algorithm** — one of the most elegant and important algorithms in graph theory. It partitions a **directed graph** into groups of nodes where every node can reach every other node through directed edges.

---

## What Is a Strongly Connected Component?

Imagine a city road map where some roads are one-way. A **Strongly Connected Component (SCC)** is a group of intersections where you can travel from any intersection to any other intersection using only the available one-way roads.

```
Example directed graph:

    0 ──► 1 ──► 3 ──► 4
    ▲     │           │
    │     ▼           ▼
    └──── 2     5 ◄── 6
                │
                └──► 7
```

SCCs here:
- `{0, 1, 2}` — each can reach the others via the cycle 0→1→2→0
- `{3, 4, 6, 5}` — 3→4→6→5→3 cycle forms one component
- `{7}` — no outgoing edges, so it is alone

---

## File Header

```cpp
/**
 * @file TarjanSCC.hpp
 * @brief Tarjan's Strongly Connected Components (SCC) algorithm.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */
```

**Every file starts with a documentation block.** This tells you:
- What the file is
- Why it exists
- How fast it runs — O(V + E) means it only looks at every vertex and every edge once.

---

## `#pragma once`

```cpp
#pragma once
```

This is a **header guard**. It tells the compiler: "Only include this file one time per compilation unit."

---

## Include Statements

```cpp
#include <vector>          // std::vector — dynamic arrays
#include <stack>           // std::stack — LIFO stack for DFS traversal
#include <unordered_map>   // std::unordered_map — hash map for disc/low values
#include <unordered_set>   // std::unordered_set — fast "is node on the stack?" lookup
#include <algorithm>       // std::min — used to update low[] values
#include <stdexcept>       // std::invalid_argument — for error handling
#include "Graph.hpp"       // Our own Graph class
```

**Why `unordered_set` instead of `set`?**
`unordered_set` uses a hash table internally: O(1) average lookup.
`set` uses a sorted tree: O(log N) lookup.
For Tarjan's, we need to check "is node v currently on the DFS stack?" thousands of times, so O(1) is critical.

---

## Class Template Declaration

```cpp
template<typename NodeType, typename WeightType = int, typename Hash = GraphUtils::SafeHash>
class TarjanSCC {
```

- **`template<...>`** — This class works for ANY type of node.
- **`typename NodeType`** — The type of the graph's nodes (e.g., `int`, `std::string`).
- **`typename WeightType = int`** — Edge weight type, defaults to `int`.
- **`typename Hash = GraphUtils::SafeHash`** — The hash function.

**Why a class instead of a free function?**
Tarjan's algorithm is **stateful** — it needs to maintain `disc[]`, `low[]`, the stack, and a timer across recursive calls. A class is the natural way to group all this state together.

---

## Private Member Variables

```cpp
private:
    std::unordered_map<NodeType, int, Hash> disc;    // Discovery time of each node
    std::unordered_map<NodeType, int, Hash> low;     // Lowest discovery time reachable
    std::unordered_set<NodeType, Hash>      onStack; // Is this node currently on the DFS stack?
    std::stack<NodeType>                    s;       // The DFS traversal stack itself
    int                                     timer;   // Global counter — increments on each visit
    std::vector<std::vector<NodeType>>      sccs;    // Accumulator for completed SCCs
```

### `disc[u]` — The Discovery Timestamp
When DFS first arrives at node `u`, it stamps `disc[u] = ++timer`. It permanently records when we first saw a node.

### `low[u]` — The Lowest Reachable Discovery Time
This is the **key insight** of Tarjan's algorithm. `low[u]` answers the question:
> "What is the earliest-visited node that I (or any node in my DFS subtree) can reach?"

If `low[u] == disc[u]`, it means node `u` cannot "reach back" to any ancestor. This makes `u` the **root of its own SCC**.
`low[]` starts equal to `disc[]` but gets pulled down when we find back-edges to ancestors.

### `onStack` — Active DFS Stack Membership
Tracks which nodes are currently in the DFS recursion stack. We use it to distinguish between back-edges (pointing to an ancestor in the active stack; same SCC candidate) and cross-edges (pointing to already processed nodes in a completed SCC; ignore).

### `s` — The Actual Traversal Stack
Accumulates nodes in DFS order. When we detect an SCC root (`low[u] == disc[u]`), we pop nodes from `s` until we reach `u` — those popped nodes form one SCC.

### `timer` — The Global Clock
A simple integer that increments with each new node visit.

### `sccs` — The Result Accumulator
A `vector<vector<NodeType>>` where each inner vector is one SCC.

---

## The Private `dfs()` Helper

```cpp
void dfs(const Graph<NodeType, WeightType, Hash>& g, NodeType u) {
```

Called recursively.

### Step 1: Stamp Discovery Time and Push to Stack

```cpp
disc[u] = low[u] = ++timer;
```

Both `disc[u]` and `low[u]` start equal since we haven't checked neighbors yet.

```cpp
s.push(u);
onStack.insert(u);
```

Push node `u` onto both the stack `s` and the `onStack` set.

### Step 2: Visit Each Neighbour

```cpp
for (const auto& edge : g.neighbors(u)) {
    NodeType v = edge.to;
```

We look at every directed edge `u → v`.

#### Case 1: `v` has NOT been visited yet

```cpp
if (disc.find(v) == disc.end()) {
    dfs(g, v);                         // Recurse into v
    low[u] = std::min(low[u], low[v]); // Pull up v's low value
}
```

Recurse into `v`. After the call returns, update `low[u]` with `low[v]`, bubbling up the earliest reachable ancestor.

#### Case 2: `v` IS visited AND is still on the stack (back-edge)

```cpp
} else if (onStack.find(v) != onStack.end()) {
    low[u] = std::min(low[u], disc[v]);
}
```

If `v` is already visited and still on the active stack, `u → v` is a back-edge. We update `low[u] = min(low[u], disc[v])`.
Using `disc[v]` (not `low[v]`) is correct because we only want to track how far back in the DFS tree we can reach without crossing completed SCC boundaries.

#### Case 3: `v` is visited but NOT on the stack (cross/forward edge)
We skip it completely — it belongs to an already-completed SCC.

### Step 3: SCC Root Detection and Extraction

```cpp
if (low[u] == disc[u]) {
```

If `low[u] == disc[u]`, `u` is the **root** of a complete SCC.

```cpp
    std::vector<NodeType> scc;
    while (true) {
        NodeType v = s.top();
        s.pop();
        onStack.erase(v);  // Remove from active stack tracking
        scc.push_back(v);
        if (v == u) {
            break;         // Stop when we reach the root node u
        }
    }
    sccs.push_back(scc);
```

Pop nodes from `s` one by one until we hit `u`. These form one SCC.

---

## The Public `findSCC()` Method

```cpp
std::vector<std::vector<NodeType>> findSCC(const Graph<NodeType, WeightType, Hash>& g) {
```

### Guard: Only for Directed Graphs

```cpp
if (!g.isDirected()) {
    throw std::invalid_argument("Tarjan's SCC algorithm requires a directed graph.");
}
```

SCCs are only meaningful for directed graphs.

### Reset State

```cpp
disc.clear();
low.clear();
onStack.clear();
while (!s.empty()) { s.pop(); }
sccs.clear();
timer = 0;
```

Resets all state before each new run to allow reuse.

### Main DFS Loop

```cpp
for (const auto& node : g.getAllNodes()) {
    if (disc.find(node) == disc.end()) {
        dfs(g, node);
    }
}
```

Call `dfs` on every unvisited node to handle disconnected graphs.

### Return Results

```cpp
return sccs;
```

---

## Complete Mental Model: Worked Example

```
Graph: 0→1, 1→2, 2→0, 1→3, 3→4, 4→5, 5→3, 4→6, 6→7, 7→6

Step-by-step:
dfs(0): disc[0]=1, low[0]=1, stack=[0]
  dfs(1): disc[1]=2, low[1]=2, stack=[0,1]
    dfs(2): disc[2]=3, low[2]=3, stack=[0,1,2]
      neighbor 0: on stack! low[2] = min(3, disc[0]=1) = 1
    ← back to 2: low[2]=1. Check: low[2]=1 != disc[2]=3 (not root)
    ← back to 1: low[1] = min(2, low[2]=1) = 1
    dfs(3): disc[3]=4, low[3]=4, stack=[0,1,2,3]
      dfs(4): disc[4]=5, low[4]=5, stack=[0,1,2,3,4]
        dfs(5): disc[5]=6, low[5]=6, stack=[0,1,2,3,4,5]
          neighbor 3: on stack! low[5] = min(6, disc[3]=4) = 4
        ← back to 5: low[5]=4. Check: low[5]=4 != disc[5]=6 (not root)
        ← back to 4: low[4] = min(5, low[5]=4) = 4
        dfs(6): disc[6]=7, low[6]=7, stack=[0,1,2,3,4,5,6]
          dfs(7): disc[7]=8, low[7]=8, stack=[0,1,2,3,4,5,6,7]
            neighbor 6: on stack! low[7] = min(8, disc[6]=7) = 7
          ← back to 7: low[7]=7. Check: low[7]=7 == disc[7]=8? NO (7 != 8, not root)
          ← back to 6: low[6] = min(7, low[7]=7) = 7
          Check: low[6]=7 == disc[6]=7? YES → SCC root!
          Pop until 6: {7, 6} → SCC #1 = {7, 6}
        ← back to 4: low[4] = min(4, low[6]=7) = 4
        Check low[4]=4 == disc[4]=5? NO
      ← back to 3: low[3] = min(4, low[4]=4) = 4
      Check low[3]=4 == disc[3]=4? YES → SCC root!
      Pop until 3: {5, 4, 3} → SCC #2 = {5, 4, 3}
    ← back to 1: Check low[1]=1 == disc[1]=2? NO
  ← back to 0: low[0] = min(1, low[1]=1) = 1
  Check low[0]=1 == disc[0]=1? YES → SCC root!
  Pop until 0: {2, 1, 0} → SCC #3 = {2, 1, 0}

Final SCCs: [{7,6}, {5,4,3}, {2,1,0}]
```

---

# Key Takeaways & Summary

### Tarjan's SCC Algorithm Summary

| Metric / Concept | Details |
| :--- | :--- |
| **Problem Solved** | Finds all Strongly Connected Components (SCCs) in a directed graph. |
| **Core Strategy** | Single-pass Depth-First Search (DFS) with parent/ancestor tracking. |
| **Discovery Time (`disc[u]`)** | Monotonically increasing counter assigned when a node is first visited. |
| **Low Link Value (`low[u]`)** | Smallest discovery time reachable from $u$ via a sequence of tree edges or back-edges. |
| **SCC Identification** | A node $u$ is the root of an SCC if and only if $\text{disc}[u] == \text{low}[u]$. |
| **Data Structures** | `std::stack` (tracks active DFS path), `std::unordered_set` (O(1) stack membership check), maps for `disc` and `low`. |
| **Time Complexity** | $O(V + E)$ |
| **Space Complexity** | $O(V)$ |

> [!IMPORTANT]
> The auxiliary `std::unordered_set` for tracking stack membership is crucial. In Tarjan's algorithm, we only update low-link values using nodes that are currently *on* the DFS stack (i.e., part of the current recursive branch). Checking membership in a raw `std::stack` takes $O(V)$ time, which would degrade the algorithm to $O(V^2)$ without the $O(1)$ set lookup.
