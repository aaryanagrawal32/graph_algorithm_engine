# Algorithm Cheat Sheet — All 7 Algorithms Side-by-Side

This is your **one-page quick reference**. Use it to:
- Decide which algorithm to use for a given problem
- Review time/space complexity before an interview
- See what data structures each algorithm needs

---

## The Decision Tree — Which Algorithm Do I Use?

```
Is the graph WEIGHTED?
│
├── NO (all edges = 1 hop)
│   └── Use BFS for shortest path
│       Use DFS for cycle detection / traversal
│
└── YES (edges have different weights)
    │
    ├── Are there NEGATIVE WEIGHTS?
    │   ├── YES
    │   │   ├── Is there a NEGATIVE CYCLE? → Use Bellman-Ford (detects it)
    │   │   └── No negative cycle → Use Bellman-Ford
    │   │
    │   └── NO (all weights ≥ 0)
    │       ├── Do you know the DESTINATION? → Use A* (faster with heuristic)
    │       └── Need distances to ALL nodes → Use Dijkstra
    │
    └── Special cases:
        ├── Find minimum spanning tree (undirected) → Use Kruskal
        ├── Find strongly connected components (directed) → Use Tarjan SCC
        └── Order nodes by dependencies (DAG) → Use Topological Sort
```

---

## Complexity Reference Card

| Algorithm | Time | Space | Input | Output |
|---|---|---|---|---|
| **BFS** | O(V + E) | O(V) | Unweighted graph + source | Visit order / shortest hop-path |
| **DFS** | O(V + E) | O(V) | Any graph + source | Visit order / cycle detection |
| **Dijkstra** | O(E log V) | O(V) | Weighted graph (≥0) + source | Shortest distances + paths to all nodes |
| **A\*** | O(E log V) | O(V) | Weighted graph (≥0) + source + goal + heuristic | Shortest path to ONE destination |
| **Bellman-Ford** | O(V × E) | O(V) | Weighted graph (any) + source | Shortest distances / negative cycle detection |
| **Kruskal** | O(E log E) | O(V + E) | Undirected weighted graph | Minimum spanning tree edges |
| **Tarjan SCC** | O(V + E) | O(V) | Directed graph | All strongly connected components |
| **Topological Sort** | O(V + E) | O(V) | Directed Acyclic Graph (DAG) | Linear ordering of nodes |

**V** = number of vertices (nodes), **E** = number of edges

---

## Deep Dive — Each Algorithm

---

### BFS — Breadth-First Search

**Core idea**: Explore all neighbors at distance 1, then distance 2, then distance 3... like ripples in a pond.

**Data structure**: `std::queue` (FIFO) — ensures you process nodes level by level

**Key insight**: Because we process nodes in order of how many hops they are from the source, the FIRST time we reach a node is via the FEWEST hops. This is why BFS finds shortest paths in **unweighted** graphs.

**Does NOT work for weighted graphs** because 1 edge might have weight 1000 while another has weight 1. "Fewer hops" ≠ "shorter distance."

```
Graph: 0→1, 0→2, 1→3, 2→3

BFS from 0:
Level 0: [0]
Level 1: [1, 2]     (both reachable in 1 hop)
Level 2: [3]        (reachable in 2 hops)

Visit order: 0, 1, 2, 3
Shortest path 0→3: 0→1→3 or 0→2→3 (both 2 hops)
```

**When to use**: Finding shortest path in maps where all edges = 1 unit, social network distance, level-order traversal.

---

### DFS — Depth-First Search

**Core idea**: Go as deep as possible down one path before backtracking.

**Data structures**:
- Recursive: the call stack IS the DFS stack
- Iterative: explicit `std::stack`

**Key insight**: DFS naturally explores a full path to its end before considering alternatives. This makes it great for problems where you need to follow a chain of dependencies.

**Does NOT find shortest paths** — it finds A path, but not necessarily the shortest one.

**Cycle detection**: During DFS, if you encounter a node that's already in the current recursion path (a "back edge"), there's a cycle.

```
Graph: 0→1, 1→2, 2→3

DFS from 0:
Visit 0 → go deeper to 1 → go deeper to 2 → go deeper to 3 → backtrack

Visit order: 0, 1, 2, 3  (same as BFS on a chain, but different on branching graphs)
```

**When to use**: Cycle detection, topological sort, checking connectivity, solving mazes.

---

### Dijkstra's Algorithm

**Core idea**: Always expand the currently cheapest unvisited node. Greedily build shortest paths outward from the source.

**Data structure**: Min-heap `priority_queue` with `greater<>` — so the node with the smallest current distance is always processed next.

**Key insight — Stale Entries**: When we find a shorter path to a node, we push a new entry into the priority queue. The old (stale) entry is still there. When we pop it, we skip it with:
```cpp
if (d > dist[u]) continue;  // This is a stale entry — skip it
```

**Why it fails with negative weights**:
Once we pop a node from the priority queue, we consider its distance "settled." But with negative edges, a path found later (longer so far, but with a future negative edge) could actually be shorter. Dijkstra doesn't go back and reconsider.

```
Graph: A--4--B, A--2--C, C--1--B, B--5--D

Dijkstra from A:
dist = {A:0, B:∞, C:∞, D:∞}
Push (0,A)

Pop (0,A): relax neighbors
  dist[B] = min(∞, 0+4) = 4, push (4,B)
  dist[C] = min(∞, 0+2) = 2, push (2,C)

Pop (2,C): relax neighbors  
  dist[B] = min(4, 2+1) = 3, push (3,B)

Pop (3,B): relax neighbors
  dist[D] = min(∞, 3+5) = 8, push (8,D)

Pop (4,B): d=4 > dist[B]=3 → STALE, skip!

Pop (8,D): D settled, dist[D] = 8
```

**When to use**: GPS navigation, network routing, any "find the cheapest path" problem with non-negative weights.

---

### A\* (A-Star) Algorithm

**Core idea**: Dijkstra + a hint about where the goal is. Instead of just using the cost-so-far `g(n)`, prioritize nodes with the smallest `f(n) = g(n) + h(n)` where `h(n)` estimates the remaining distance.

**The formula**:
```
f(n) = g(n) + h(n)
       ↑         ↑
actual cost   estimated cost
from source   from n to goal
to n so far   (the heuristic)
```

**Key insight — Admissibility**: A heuristic is "admissible" if it NEVER overestimates the real remaining distance. An admissible heuristic guarantees A* finds the optimal path.

- **Manhattan distance** `|x1-x2| + |y1-y2|` is admissible for 4-directional grid movement (you can't travel diagonally, so straight-line distance ≤ actual distance)
- **Euclidean distance** `√(dx²+dy²)` is admissible for any movement type (straight line ≤ any path)
- **Zero heuristic** `h(n) = 0` → A* degenerates to Dijkstra

**Why A* visits fewer nodes**: By estimating the remaining distance, A* can ignore nodes that are clearly heading away from the goal. Dijkstra explores in all directions equally.

```
10×10 grid, source = (0,0), goal = (9,9):
Dijkstra: explores ~100 nodes (all of them)
A* with Manhattan: explores ~18-25 nodes (the corridor toward the goal)
```

**When to use**: Game pathfinding, GPS (geographic heuristic available), any problem where you can estimate remaining cost.

---

### Bellman-Ford

**Core idea**: Relax every single edge, V-1 times. After V-1 rounds, shortest paths are guaranteed to be found.

**Why V-1 rounds**: The longest possible simple path (without cycles) visits V nodes and has V-1 edges. So after V-1 rounds of relaxing all edges, every such path has been "discovered."

**Negative cycle detection**: After V-1 rounds, try one more round. If any distance still decreases, there's a negative cycle (a cycle whose total weight is negative — you can go around it forever to get shorter and shorter paths).

```
V-1 = 3 iterations for a 4-node graph:

Round 1: Finds all 1-hop shortest paths
Round 2: Finds all 2-hop shortest paths
Round 3: Finds all 3-hop shortest paths (V-1 = 3 hops = longest simple path)
Round 4 (check): If anything changes → NEGATIVE CYCLE!
```

**Slowness**: O(V × E) is much slower than Dijkstra's O(E log V). Use Bellman-Ford only when negative weights are present.

**When to use**: Financial modeling (negative edges = gains), graphs where Dijkstra gives wrong answers, detecting arbitrage in currency exchange.

---

### Kruskal's MST

**Core idea**: Sort all edges by weight. Greedily add the cheapest edge that doesn't create a cycle, until you have V-1 edges.

**Cycle detection**: Uses a **Disjoint Set Union (DSU)** data structure. Two nodes are in the same component if their "representative" (root) is the same. Adding an edge between two nodes in the same component would create a cycle.

**DSU optimizations**:
- **Path compression**: When finding a node's root, make all nodes on the path point directly to the root. Subsequent `find()` calls are O(1).
- **Union by rank**: When merging two trees, attach the smaller tree under the larger tree. Keeps trees shallow.
- Together: nearly O(1) per operation (O(α(n)) where α is the inverse Ackermann function)

```
Graph: A-B(4), A-H(8), B-C(8), C-I(2), F-G(2), G-H(1)

Sorted edges: G-H(1), C-I(2), F-G(2), A-B(4), ...

Step 1: Add G-H(1) → {G,H} one component
Step 2: Add C-I(2) → {C,I} one component  
Step 3: Add F-G(2) → {F,G,H} one component
Step 4: Add A-B(4) → {A,B} one component
...

Total MST weight = 37 (V-1 = 8 edges for 9 nodes)
```

**When to use**: Network infrastructure (cheapest way to connect all nodes), clustering, approximation algorithms.

---

### Tarjan's SCC

**Core idea**: Run DFS, tracking two values per node:
- `disc[u]`: when was `u` first discovered?
- `low[u]`: what's the earliest ancestor reachable from `u`'s subtree?

**SCC root condition**: `low[u] == disc[u]` means no path in `u`'s DFS subtree leads to any ancestor of `u` → `u` is the "root" of its own SCC.

**The stack**: Nodes are pushed when first visited, popped when an SCC root is found. All nodes from the top of the stack down to the root form one SCC.

```
0→1, 1→2, 2→0 (cycle), 1→3, 3→4, 4→5, 5→3 (cycle), 4→6, 6→7, 7→6 (cycle)

SCCs:
{0, 1, 2} — all can reach each other via the 0→1→2→0 cycle
{3, 4, 5} — all can reach each other via the 3→4→5→3 cycle
{6, 7}    — can reach each other via 6→7→6 cycle
```

**When to use**: Finding cycles in dependency graphs, detecting circular imports, strongly connected road networks, compiler dependency analysis.

---

### Topological Sort

**Core idea**: Linear ordering of nodes where every directed edge `u→v` has `u` appearing before `v`.

**Only works on DAGs** (Directed Acyclic Graphs). Cycles make topological sort impossible.

**Two implementations**:

**Kahn's Algorithm (BFS-based)**:
1. Compute in-degree (number of incoming edges) for all nodes
2. Queue nodes with in-degree 0 (no prerequisites)
3. Pop a node, add to result, decrement in-degree of its neighbors
4. If a neighbor's in-degree reaches 0, queue it
5. If result.size() < nodeCount → there's a cycle (some nodes never reached in-degree 0)

**DFS-based**:
1. DFS from every unvisited node
2. When DFS finishes processing a node (all its children done), push it to a stack
3. Reverse the stack → topological order

```
Dependencies: CS101→CS201, CS101→CS202, CS201→CS301, CS202→CS301

In-degrees: CS101=0, CS201=1, CS202=1, CS301=2

Kahn's:
Queue: [CS101]
Pop CS101 → result=[CS101], CS201.indegree=0, CS202.indegree=0
Queue: [CS201, CS202]
Pop CS201 → result=[CS101,CS201], CS301.indegree=1
Pop CS202 → result=[CS101,CS201,CS202], CS301.indegree=0
Queue: [CS301]
Pop CS301 → result=[CS101,CS201,CS202,CS301] ✓
```

**When to use**: Task scheduling, build systems (what to compile first), course prerequisites, package dependency resolution.

---

## Algorithm Property Comparison

| Property | BFS | DFS | Dijkstra | A* | Bellman-Ford | Kruskal | Tarjan | Topo Sort |
|---|---|---|---|---|---|---|---|---|
| Handles negative weights | ❌ | ❌ | ❌ | ❌ | ✅ | N/A | N/A | N/A |
| Detects negative cycles | ❌ | ❌ | ❌ | ❌ | ✅ | N/A | N/A | N/A |
| Finds shortest path | ✅(hops) | ❌ | ✅ | ✅ | ✅ | N/A | N/A | N/A |
| Requires directed graph | ❌ | ❌ | ❌ | ❌ | ❌ | ❌(undirected) | ✅(directed) | ✅(directed) |
| Detects cycles | ✅ | ✅ | ❌ | ❌ | ✅ | via DSU | implicit | ✅ |
| Works on disconnected graph | ✅ | ✅ | ✅ | ✅ | ✅ | ✅(forest) | ✅ | ✅ |
| Needs a goal node | ❌ | ❌ | ❌ | ✅ | ❌ | N/A | N/A | N/A |
| Needs a heuristic | ❌ | ❌ | ❌ | ✅ | ❌ | N/A | N/A | N/A |

---

## Data Structures Used — Quick Reference

| Data Structure | Algorithm(s) | Why |
|---|---|---|
| `queue` (FIFO) | BFS | Level-by-level expansion |
| `stack` (LIFO) | DFS, Tarjan | Depth-first, backtrack |
| `priority_queue` (min-heap) | Dijkstra, A* | Always expand cheapest node |
| `unordered_map` | All | O(1) lookup of distances, visited |
| `unordered_set` | BFS, DFS, Tarjan | O(1) membership check |
| Disjoint Set Union | Kruskal | Cycle detection in O(α(n)) |
| `vector<vector<T>>` | Tarjan | Store list of SCCs |
