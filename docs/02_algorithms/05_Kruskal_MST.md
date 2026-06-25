# Kruskal.hpp — Complete Line-by-Line Explanation

This file introduces a brand new data structure — **Disjoint Set Union
(DSU)**, also called Union-Find — plus `std::tuple` (an N-element
sibling of `std::pair`) and `std::sort` with a custom comparator. Part 0
covers all of that. Part 1 walks both the `DSU` struct and the
`kruskal()` function top to bottom. Part 2 traces DSU operations by
hand (specifically constructing a moment where path compression
visibly flattens a chain), then traces the full `kruskal()` algorithm
on a small graph — including a genuinely tricky cycle-skip that isn't
just a literal duplicate edge.

---

# Part 0 — New Building Blocks

### NEW: `std::tuple` and `std::get<N>(...)`
`std::pair` holds exactly two values, accessed as `.first`/`.second`.
`std::tuple` is the generalization to **any** number of values:
```cpp
std::tuple<WeightType, NodeType, NodeType>
```
holds exactly three values — here, a weight and two nodes, representing
one edge as `(weight, u, v)`. Since a tuple can have any number of
slots, there's no fixed naming scheme like `.first`/`.second`.
Instead, you access slot `N` (counting from `0`) using the free function
`std::get<N>(theTuple)`. So for a tuple `t`:
`std::get<0>(t)` is the weight, `std::get<1>(t)` is `u`, `std::get<2>(t)` is `v`.

### NEW: Disjoint Set Union (DSU / Union-Find) — the core idea
This is a data structure for one specific, very common job: efficiently
tracking which "group" each item currently belongs to, while groups
keep merging over time, and quickly answering "are these two items in
the same group right now?"

Every item stores a single `parent` pointer to another item. If you follow
an item's parent, then that item's parent, you eventually reach an item
whose parent points to itself — that item is the **root**, and it's used
as that whole group's unique representative ID. Two items are in the same
group exactly when they lead to the same root.

This file's `DSU` struct keeps two parallel maps:
- `parent` — for every item, who its parent is.
- `rank` — for every item that's currently a root, roughly "how tall is
  the tree hanging below me."

### NEW: `find()`'s single most important line — path compression
```cpp
return parent[x] = find(parent[x]);
```
This single line does two things in sequence:
1. First, `find(parent[x])` runs recursively to find the ultimate root.
2. Once the root is found, `parent[x] = ...` overwrites `x`'s parent to point
   directly at the root, bypassing intermediate nodes.
3. The expression evaluates to that same root value, which is returned.

**Why this matters ("path compression"):** without this trick, walking a long
chain of parent pointers would take O(V) time. With path compression, the first
`find()` traverses it once, and then permanently rewrites all parent pointers
along the path to point directly to the root, making subsequent lookups O(1).

### NEW: Union by rank
```cpp
if (rank[rootX] < rank[rootY]) {
    parent[rootX] = rootY;
} else if (rank[rootX] > rank[rootY]) {
    parent[rootY] = rootX;
} else {
    parent[rootY] = rootX;
    rank[rootX]++;
}
```
Always attach the tree with the smaller rank underneath the tree with the
larger rank. This keeps tree heights growing logarithmically as merges happen,
keeping `find()` fast even before path compression flattens it.
- `rank[rootX] < rank[rootY]` — X's tree is shorter → attach X under Y.
- `rank[rootX] > rank[rootY]` — Y's tree is shorter → attach Y under X.
- Equal ranks (`else`) — attach Y under X, and bump X's rank by 1.

### NEW: Amortized complexity, and the inverse Ackermann function
**Amortized** means: averaged out over a long sequence of operations.
`α(N)` (the inverse Ackermann function) grows so absurdly slowly that it never
exceeds 4 or 5 for any practical universe-sized `N`.
Combining path compression and union by rank guarantees that DSU operations run
in essentially constant time, O(α(N)) ~ O(1) per operation.

### NEW: `std::sort` with a custom comparator lambda
```cpp
std::sort(allEdges.begin(), allEdges.end(), [](const auto& a, const auto& b) {
    return std::get<0>(a) < std::get<0>(b);
});
```
`std::sort` rearranges a range of elements in-place. The custom comparator lambda
says: "compare these two edges using only their weight (`std::get<0>`), ignoring
which nodes they connect."
Whenever two edges share the exact same weight, their final sorted relative order
is unspecified since `std::sort` is not guaranteed stable.

### Recap: structured bindings, now with three elements
`auto [w, u, v] = edge;` works on a `std::tuple` exactly the same way it
worked on a `std::pair` in earlier files.

### Recap: why a spanning tree has exactly V−1 edges
Connecting `V` vertices into a single tree without cycles always requires
exactly `V − 1` edges. Every successful merge reduces the number of separate DSU
components by exactly 1. Starting from `V` groups, reaching 1 group requires
exactly `V − 1` merges, allowing us to stop the algorithm early.

---

# Part 1 — The File, Line by Line

```cpp
/**
 * @file Kruskal.hpp
 * @brief Kruskal's Minimum Spanning Tree (MST) algorithm.
 *
 * Finds the minimum spanning tree of a weighted undirected graph
 * using the greedy approach combined with Disjoint Set Union (DSU).
 *
 * Time Complexity: O(E log E) or O(E log V)
 * Space Complexity: O(V + E)
 */
```

### 1.1 — Header comment
An MST is the cheapest set of edges connecting all vertices into one tree.
This file finds one for a weighted undirected graph.

```cpp
#pragma once
 
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <stdexcept>
#include "Graph.hpp"
```

### 1.2 — Header guard and includes
Standard includes, adding `<tuple>` for edge tuples.

```cpp
/**
 * @struct DSU
 * @brief Disjoint Set Union (Union-Find) data structure.
 * Supports path compression and union by rank.
 */
template<typename NodeType, typename Hash = GraphUtils::SafeHash>
struct DSU {
    std::unordered_map<NodeType, NodeType, Hash> parent;
    std::unordered_map<NodeType, int, Hash> rank;
```

### 1.3 — The `DSU` struct declaration
No `WeightType` here. DSU only tracks grouping and connectivity.

```cpp
    /**
     * @brief Creates a new set containing only element x.
     * Time Complexity: O(1) average.
     */
    void makeSet(NodeType x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
            rank[x] = 0;
        }
    }
```

### 1.4 — `makeSet`
Creates a brand-new, singleton group containing just `x`. The `find` guard
ensures we don't overwrite an existing element that has already been merged.

```cpp
    /**
     * @brief Finds the representative (root) of the set containing x.
     * Applies path compression to flatten the set tree.
     * Time Complexity: Amortized O(alpha(N)) ~ O(1).
     */
    NodeType find(NodeType x) {
        if (parent[x] == x) {
            return x;
        }
        // Path compression step
        return parent[x] = find(parent[x]);
    }
```

### 1.5 — `find`
If `parent[x] == x`, `x` is the root. Otherwise, recursively find the root
and update `parent[x]` in-place (path compression).

```cpp
    /**
     * @brief Unites the sets containing x and y.
     * Applies Union by Rank to attach the shorter tree under the taller tree.
     * @return true if united successfully, false if they were already in the same set.
     * Time Complexity: Amortized O(alpha(N)) ~ O(1).
     */
    bool unite(NodeType x, NodeType y) {
        NodeType rootX = find(x);
        NodeType rootY = find(y);
 
        if (rootX == rootY) {
            return false; // Cycle detected/Already in same component
        }
```

### 1.6 — `unite`, part 1
Find roots of both `x` and `y`. If they share the same root, they are already
connected — adding an edge between them would create a cycle. Return `false`.

```cpp
        // Union by rank
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        return true;
    }
};
```

### 1.7 — `unite`, part 2
If roots differ, merge groups using union by rank and return `true`.

---

```cpp
/**
 * @brief Computes the Minimum Spanning Tree (MST) of a weighted undirected graph using Kruskal's algorithm.
 * @tparam NodeType Type of the graph nodes.
 * @tparam WeightType Type of the edge weights.
 * @tparam Hash Hash functor for NodeType.
 * @param g Reference to the Graph.
 * @return std::vector<std::tuple<WeightType, NodeType, NodeType>> Edges in the MST as {weight, u, v}.
 *
 * Time Complexity Derivation: O(E log E)
 * - Collecting all edges takes O(E).
 * - Sorting edges by weight takes O(E log E) time.
 * - Processing edges and running DSU find/unite takes O(E * alpha(V)) ~ O(E) time.
 * - Total time complexity is O(E log E). Since E <= V^2, log E <= 2 log V, which is also O(E log V).
```

### 1.8 — `kruskal()` documentation, part 1
Returns list of chosen MST edges. Since `E <= V^2`, `O(E log E)` is equivalent
to `O(E log V)`. Sorting is the dominant operation.

```cpp
 * WHY sorting edges is the greedy choice (Cut Property):
 * For any cut of the graph (partitioning vertices into two sets S and V - S), the minimum weight edge
 * that crosses the cut must belong to some MST of the graph. By sorting edges globally and processing
 * them in ascending order of weight, Kruskal's algorithm always greedily selects the cheapest edge
 * crossing the current components cut, guaranteeing a minimum total weight.
```

### 1.9 — The Cut Property
For any cut (partition of vertices into S and V-S), the cheapest edge crossing
it belongs to some MST. By sorting edges and picking the cheapest that doesn't
form a cycle, we greedily select the optimal edge crossing the cuts between
our growing forest components.

```cpp
 * WHY DSU detects cycles efficiently:
 * If an edge connects u and v, and `find(u) == find(v)`, they are already connected by some path
 * in the current forest. Adding this edge would create a cycle. DSU lets us check this in O(1) time.
 */
```

```cpp
template<typename NodeType, typename WeightType, typename Hash>
std::vector<std::tuple<WeightType, NodeType, NodeType>> kruskal(const Graph<NodeType, WeightType, Hash>& g) {
    std::vector<std::tuple<WeightType, NodeType, NodeType>> mstEdges;
 
    if (g.isDirected()) {
        throw std::invalid_argument("Kruskal's algorithm requires an undirected graph.");
    }
```

### 1.11 — Function signature and directedness guard
Throws `std::invalid_argument` if the graph is directed. Kruskal's requires
undirected graphs.

```cpp
    // Collect all edges from the adjacency list
    std::vector<std::tuple<WeightType, NodeType, NodeType>> allEdges;
    for (const auto& u : g.getAllNodes()) {
        for (const auto& edge : g.neighbors(u)) {
            // In an undirected graph, u->v and v->u both exist. We collect all,
            // and the DSU cycle check will naturally skip the reverse duplicate edge.
            allEdges.push_back({edge.weight, u, edge.to});
        }
    }
```

### 1.12 — Collecting edges
Collects every edge twice (once as `u→v` and once as `v→u`). The DSU cycle check
will naturally skip the reverse duplicate since they will already be connected.

```cpp
    // Sort edges by weight
    std::sort(allEdges.begin(), allEdges.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });
```

Sorts the edge list in-place by weight.

```cpp
    // Initialize DSU
    DSU<NodeType, Hash> dsu;
    for (const auto& node : g) {
        dsu.makeSet(node);
    }
```

Creates a singleton set for every vertex in the graph.

```cpp
    // Process sorted edges
    int edgesAdded = 0;
    int targetEdges = g.nodeCount() - 1; // An MST has exactly V - 1 edges
```

Tracks MST progress. Stops early once `V - 1` edges have been added.

```cpp
    for (const auto& edge : allEdges) {
        if (edgesAdded >= targetEdges) {
            break; // Found all edges of the MST
        }
 
        auto [w, u, v] = edge;
        if (dsu.unite(u, v)) {
            mstEdges.push_back(edge);
            edgesAdded++;
        }
    }
 
    return mstEdges;
}
```

Iterates over sorted edges. If `dsu.unite(u, v)` succeeds, the edge connects two
disconnected trees without cycles — add to `mstEdges` and increment count.
Once target is reached or edges exhausted, return `mstEdges`.

---

# Part 2 — Tracing It By Hand

## Trace A — DSU operations in isolation (watch path compression flatten a chain)
Items: `1, 2, 3, 4, 5`.
`makeSet` all: `parent` points to self, `rank` = 0.

**`unite(1, 2)`:** roots 1 and 2. Tied ranks → `parent[2]=1`, `rank[1]=1`.
`parent = {1:1, 2:1, 3:3, 4:4, 5:5}`

**`unite(3, 4)`:** roots 3 and 4. Tied ranks → `parent[4]=3`, `rank[3]=1`.
`parent = {1:1, 2:1, 3:3, 4:3, 5:5}`

**`unite(2, 4)`:**
- `find(2)`: base case 1.
- `find(4)`: base case 3.
- Roots 1 and 3. Tied ranks → `parent[3]=1`, `rank[1]=2`.
`parent = {1:1, 2:1, 3:1, 4:3, 5:5}`. (Chain `4 → 3 → 1` formed).

**`unite(4, 5)`:**
- `find(4)`: `parent[4]=3` → calls `find(3)`. `find(3)` returns 1.
  Path compression: `parent[3]=1`, and `parent[4]=1` (rewritten direct to root!).
- `find(5)`: returns 5.
- Roots 1 and 5. `rank[1]=2 > rank[5]=0` → `parent[5]=1`.
`parent = {1:1, 2:1, 3:1, 4:1, 5:1}`. All nodes flattened to root 1 in one shot!

---

## Trace B — Full `kruskal()` on a small weighted graph
Undirected graph with V=4 (`targetEdges = 3`):
```
A-B  weight 1
A-C  weight 2
B-C  weight 2   (tied)
C-D  weight 3
A-D  weight 4
```

Sorted collected edges (including duplicates):
```
(1,A,B), (1,B,A), (2,A,C), (2,C,A), (2,B,C), (2,C,B), (3,C,D), (3,D,C), (4,A,D), (4,D,A)
```

**Edge `(1,A,B)`:** `unite(A,B)`: roots A, B. Merged → `parent[B]=A`, `rank[A]=1`. **Added.**
`mstEdges=[(1,A,B)]`, `edgesAdded=1`.

**Edge `(1,B,A)`** (reverse duplicate): `find(B)=A`, `find(A)=A`. Same root → **Skipped.**

**Edge `(2,A,C)`:** `unite(A,C)`: roots A, C. `rank[A]=1 > rank[C]=0` → `parent[C]=A`. **Added.**
`mstEdges=[(1,A,B),(2,A,C)]`, `edgesAdded=2`.

**Edge `(2,C,A)`** (reverse duplicate): roots match → **Skipped.**

**Edge `(2,B,C)`:** `unite(B,C)`: `find(B)=A`, `find(C)=A`. Same root → **Skipped!**
(Avoids the A-B-C cycle! DSU cycle detection in action).

**Edge `(2,C,B)`**: skipped.

**Edge `(3,C,D)`:** `unite(C,D)`: `find(C)=A`, `find(D)=D`. `rank[A]=1 > rank[D]=0` → `parent[D]=A`. **Added.**
`mstEdges=[(1,A,B),(2,A,C),(3,C,D)]`, `edgesAdded=3`.

**Next check:** `edgesAdded(3) >= targetEdges(3)` → **break!**

Returns: `[(1,A,B), (2,A,C), (3,C,D)]` with total weight **6**.
Ties between weight-2 edges don't affect the guaranteed-minimum total weight.

---

# Key Takeaways & Summary

### Kruskal's MST & DSU Summary

| Component / Feature | Kruskal's Algorithm | Disjoint Set Union (DSU) |
| :--- | :--- | :--- |
| **Purpose** | Finds the Minimum Spanning Tree of a weighted graph. | Tracks partition of elements into disjoint, merging sets. |
| **Core Paradigm** | Greedy (processes sorted edges, adding if no cycle forms). | Forest of trees representing set hierarchies with parent pointers. |
| **Key Optimizations** | Sorts edges first; DSU is used for rapid cycle checks. | **Path Compression** (flattening) & **Union by Rank** (height bounding). |
| **Time Complexity** | $O(E \log E)$ (sorting edges) or $O(E \log V)$ (since $E \le V^2$). | $O(\alpha(V))$ per operation (essentially $O(1)$ amortized). |
| **Space Complexity** | $O(E + V)$ to store edge tuples and DSU maps. | $O(V)$ for parent and rank maps. |

> [!NOTE]
> Kruskal's algorithm is ideal for sparse graphs. For extremely dense graphs where $E \approx V^2$, Prim's algorithm (using a priority queue) can sometimes be preferred, but Kruskal's is highly favored in practice due to the sheer simplicity and efficiency of the DSU structure.
