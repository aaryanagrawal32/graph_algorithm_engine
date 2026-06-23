# Graph Algorithm Engine — Step-by-Step Spec File

## Project Identity

| Field | Value |
|---|---|
| **Project Name** | Graph Algorithm Engine |
| **Language** | C++ (C++17) |
| **Build System** | CMake 3.15+ |
| **Target** | SWE/Quant internship portfolio |
| **Difficulty** | Intermediate-Advanced |
| **Total Timeline** | 7–8 weeks |

---

## Prerequisites Checklist

Before starting, confirm you have:

- [ ] C++ fundamentals — pointers, templates, STL containers
- [ ] Basic OOP — classes, inheritance, interfaces
- [ ] Familiar with `std::vector`, `std::queue`, `std::unordered_map`
- [ ] BFS/DFS conceptually understood (even if not coded)
- [ ] CMake installed (`cmake --version`)
- [ ] SFML installed (`sudo apt install libsfml-dev` on Linux)
- [ ] Git initialized (`git init` in project folder)

---

## Week-by-Week Roadmap

```
Week 1  →  Foundation (Graph class + BFS + DFS)
Week 2  →  Shortest Path (Dijkstra)
Week 3  →  Advanced Shortest Path (A* + Bellman-Ford)
Week 4  →  MST + Union-Find (Kruskal)
Week 5  →  SCC + Topological Sort (Tarjan)
Week 6  →  Real Data + File I/O
Week 7  →  Visualizer + Benchmarks
Week 8  →  Polish, Tests, README, CV prep
```

---

---

# PHASE 1 — Foundation
## Week 1: Graph Class + BFS + DFS

---

### Step 1.1 — Project Setup
**Time estimate: 2 hours**

#### Tasks
- [ ] Create folder structure manually (see below)
- [ ] Write root `CMakeLists.txt`
- [ ] Create empty placeholder `.hpp` files
- [ ] Verify CMake configures without errors
- [ ] Initialize git, create `.gitignore`

#### Folder Structure
```
graph-engine/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── Graph.hpp
│   └── algorithms/
│       ├── BFS.hpp
│       └── DFS.hpp
├── src/
│   └── main.cpp
└── data/
    └── sample_graph.csv
```

#### Acceptance Criteria
- `cmake -B build && cmake --build build` runs without errors
- Empty `main.cpp` with `int main() { return 0; }` compiles cleanly

---

### Step 1.2 — Graph Class
**Time estimate: 4–6 hours**

#### What to Build
A generic `Graph<NodeType, WeightType>` class in `include/Graph.hpp`

#### Concepts to Understand BEFORE Coding
- What is an adjacency list vs adjacency matrix?
- Why `unordered_map` for O(1) average node lookup?
- What does a template parameter mean — why `<NodeType>`?

#### Interface Contract
```cpp
template<typename NodeType, typename WeightType = int>
class Graph {
    struct Edge { NodeType to; WeightType weight; };

    // Core methods
    void addNode(NodeType u);
    void addEdge(NodeType u, NodeType v, WeightType w = 1);
    void removeEdge(NodeType u, NodeType v);
    bool hasNode(NodeType u) const;
    bool hasEdge(NodeType u, NodeType v) const;
    const std::vector<Edge>& neighbors(NodeType u) const;
    int nodeCount() const;
    int edgeCount() const;
    std::vector<NodeType> getAllNodes() const;
};
```

#### Questions to Answer in Code Comments
1. Why adjacency list over adjacency matrix for this use case?
2. What is the time complexity of `addEdge`? Of `neighbors()`?
3. Why is `neighbors()` marked `const`?
4. What happens in `addEdge` for an undirected graph?

#### Test in main.cpp
```cpp
Graph<int> g(false);  // undirected
g.addEdge(1, 2, 4);
g.addEdge(1, 3, 2);
g.addEdge(2, 3, 1);
// Print all neighbors of node 1
// Expected: [(2, w=4), (3, w=2)]
```

#### Acceptance Criteria
- [ ] Graph compiles as template class
- [ ] Can add directed and undirected edges
- [ ] `nodeCount()` and `edgeCount()` return correct values
- [ ] `hasNode()` and `hasEdge()` work correctly
- [ ] Handles duplicate edges gracefully

---

### Step 1.3 — BFS Implementation
**Time estimate: 3–4 hours**

#### Concepts to Understand BEFORE Coding
- Why does a queue give level-order traversal?
- Why does BFS find shortest path in unweighted graphs?
- What is the visited set for — what breaks without it?

#### Functions to Implement
```cpp
// Basic traversal
template<typename NodeType, typename WeightType>
std::vector<NodeType> bfs(Graph<NodeType, WeightType>& g, NodeType start);

// Shortest path
template<typename NodeType, typename WeightType>
std::vector<NodeType> bfsShortestPath(
    Graph<NodeType, WeightType>& g, NodeType src, NodeType dst);
```

#### Algorithm Steps (understand each step)
```
1. Create visited set, queue
2. Push start node, mark visited
3. While queue not empty:
   a. Pop front node u
   b. Add u to result
   c. For each neighbor v of u:
      - If v not visited: mark visited, push to queue
4. Return result
```

#### Questions to Answer in Comments
1. Why `queue` (FIFO) and not `stack` (LIFO)?
2. Why mark visited when PUSHING, not when POPPING?
3. How does `prev` map help reconstruct the path?

#### Manual Trace Exercise
Before coding, trace BFS by hand on this graph:
```
1 -- 2 -- 4
|         |
3 ------- 5
```
Starting from node 1. Write the queue state at each step.

#### Acceptance Criteria
- [ ] BFS visits all reachable nodes exactly once
- [ ] BFS returns correct level-order sequence
- [ ] `bfsShortestPath` returns empty vector for unreachable nodes
- [ ] `bfsShortestPath` returns correct shortest path

---

### Step 1.4 — DFS Implementation
**Time estimate: 3–4 hours**

#### Functions to Implement
```cpp
// Recursive DFS
template<typename NodeType, typename WeightType>
std::vector<NodeType> dfs(Graph<NodeType, WeightType>& g, NodeType start);

// Iterative DFS (using explicit stack)
template<typename NodeType, typename WeightType>
std::vector<NodeType> dfsIterative(Graph<NodeType, WeightType>& g, NodeType start);

// Cycle detection
template<typename NodeType, typename WeightType>
bool hasCycle(Graph<NodeType, WeightType>& g);
```

#### Questions to Answer in Comments
1. Why does recursive DFS use the call stack implicitly?
2. Does recursive and iterative DFS give same traversal order? Why/why not?
3. How do you detect a cycle using DFS? (hint: back edges)

#### Manual Trace Exercise
Trace both recursive and iterative DFS on the same graph from Step 1.3.
Note where the orders differ and why.

#### Acceptance Criteria
- [ ] Both versions visit all reachable nodes
- [ ] `hasCycle` returns true for graphs with cycles
- [ ] `hasCycle` returns false for trees and DAGs

---

### Phase 1 — End of Week 1 Checkpoint

**Before moving to Week 2, you must be able to:**
- [ ] Explain adjacency list vs matrix trade-offs without looking at code
- [ ] Draw BFS and DFS traversal order for any small graph on a whiteboard
- [ ] Explain why BFS finds shortest path but DFS doesn't
- [ ] Implement BFS from scratch in 15 minutes on a whiteboard

**LeetCode problems to solve this week (validate understanding):**
- #200 Number of Islands (BFS/DFS)
- #133 Clone Graph (BFS)
- #207 Course Schedule (cycle detection)
- #417 Pacific Atlantic Water Flow (BFS)

---

---

# PHASE 2 — Shortest Path
## Week 2: Dijkstra's Algorithm

---

### Step 2.1 — Understand Priority Queue
**Time estimate: 2 hours**

Before implementing Dijkstra, deeply understand `std::priority_queue`.

#### Experiment to Run
```cpp
#include <queue>
#include <iostream>

int main() {
    // Max heap (default)
    std::priority_queue<int> maxPQ;
    maxPQ.push(3); maxPQ.push(1); maxPQ.push(4); maxPQ.push(1); maxPQ.push(5);
    while (!maxPQ.empty()) {
        std::cout << maxPQ.top() << " ";  // What order?
        maxPQ.pop();
    }

    // Min heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> minPQ;
    minPQ.push(3); minPQ.push(1); minPQ.push(4);
    while (!minPQ.empty()) {
        std::cout << minPQ.top() << " ";  // What order?
        minPQ.pop();
    }
}
```

Answer before running: what will each print and why?

---

### Step 2.2 — Dijkstra Implementation
**Time estimate: 5–6 hours**

#### Concepts to Understand BEFORE Coding
- What is "edge relaxation"?
- Why does Dijkstra fail with negative weights?
- What is a "stale entry" in the priority queue?
- Why is the time complexity O(E log V)?

#### Data Structure
```cpp
struct DijkstraResult {
    std::unordered_map<NodeType, WeightType> dist;  // shortest distances
    std::unordered_map<NodeType, NodeType>   prev;  // path reconstruction
};
```

#### Algorithm Steps
```
1. Initialize dist[all] = INF, dist[src] = 0
2. Push (0, src) to min-heap priority queue
3. While PQ not empty:
   a. Pop (d, u) — node with current smallest distance
   b. If d > dist[u]: SKIP (stale entry)
   c. For each neighbor v with edge weight w:
      - If dist[u] + w < dist[v]:
        * Update dist[v] = dist[u] + w
        * Update prev[v] = u
        * Push (dist[v], v) to PQ
4. Return dist and prev maps
```

#### Path Reconstruction
```cpp
std::vector<NodeType> reconstructPath(
    DijkstraResult& result, NodeType src, NodeType dst) {
    // Walk backwards from dst to src using prev map
    // Reverse and return
}
```

#### Questions to Answer in Comments
1. Why do we skip stale entries instead of using a `decreaseKey` operation?
2. Why initialize all distances to `numeric_limits<WeightType>::max()`?
3. What does `prev[v] = u` mean semantically?
4. Why does the algorithm terminate correctly without a visited set?

#### Manual Trace
Trace Dijkstra on this graph (find shortest path from A to D):
```
A --4-- B
|       |
2       1
|       |
D --5-- C
```
Write dist[] and PQ state after each step.

#### Acceptance Criteria
- [ ] Finds correct shortest distances from source to all nodes
- [ ] Path reconstruction returns correct path
- [ ] Returns empty path for unreachable destination
- [ ] Handles disconnected graphs
- [ ] Works with `int` and `double` weight types

**LeetCode problems:**
- #743 Network Delay Time
- #787 Cheapest Flights Within K Stops
- #1631 Path With Minimum Effort

---

### Phase 2 — End of Week 2 Checkpoint

**Must be able to:**
- [ ] Explain why `greater<>` gives min-heap behavior
- [ ] Explain the stale entry optimization
- [ ] Explain why negative weights break Dijkstra (with example)
- [ ] Implement Dijkstra from scratch on whiteboard

---

---

# PHASE 3 — Advanced Shortest Path
## Week 3: A\* and Bellman-Ford

---

### Step 3.1 — A\* Algorithm
**Time estimate: 5–6 hours**

#### Core Formula
```
f(n) = g(n) + h(n)

g(n) = actual cost from source to n
h(n) = estimated cost from n to goal (heuristic)
f(n) = total estimated cost of path through n
```

#### Concepts to Understand BEFORE Coding
- What makes a heuristic "admissible"?
- Why does an admissible heuristic guarantee optimal path?
- When does A\* degenerate to Dijkstra? (when h(n) = 0)
- When does A\* degenerate to Greedy Best-First? (when g(n) ignored)

#### Heuristics to Implement
```cpp
// For grid graphs with pair<int,int> nodes
auto manhattan = [](std::pair<int,int> a, std::pair<int,int> b) {
    return std::abs(a.first-b.first) + std::abs(a.second-b.second);
};

// For geographic coordinates
auto euclidean = [](std::pair<double,double> a, std::pair<double,double> b) {
    double dx = a.first-b.first, dy = a.second-b.second;
    return std::sqrt(dx*dx + dy*dy);
};
```

#### Key Difference from Dijkstra
```
Dijkstra: priority = g(n)          (actual cost so far)
A*:       priority = g(n) + h(n)   (actual + estimated remaining)
```

#### Benchmark to Include
```cpp
// Count nodes visited by each algorithm
auto [dijkstraPath, dijkstraVisited] = dijkstraWithStats(g, src, dst);
auto [astarPath, astarVisited] = astar(g, src, dst, manhattan);

// On a 100x100 grid, A* should visit ~10x fewer nodes
```

---

### Step 3.2 — Bellman-Ford
**Time estimate: 3–4 hours**

#### When to Use Over Dijkstra
| Scenario | Algorithm |
|---|---|
| Non-negative weights, fast | Dijkstra |
| Negative weights | Bellman-Ford |
| Need to detect negative cycles | Bellman-Ford |
| Distributed systems (routing protocols) | Bellman-Ford |

#### Algorithm Steps
```
1. Initialize dist[all] = INF, dist[src] = 0
2. Repeat V-1 times:
   For every edge (u, v, w):
     If dist[u] + w < dist[v]:
       dist[v] = dist[u] + w
3. Check V-th iteration:
   If any edge still relaxes → NEGATIVE CYCLE detected
```

#### WHY V-1 iterations?
The longest possible shortest path (without cycles) visits V-1 edges.
So V-1 relaxation passes guarantee we've found all shortest paths.

#### Acceptance Criteria
- [ ] Finds correct shortest paths with negative weights
- [ ] Throws exception on negative cycle detection
- [ ] O(VE) complexity (much slower than Dijkstra — note this)

**LeetCode:** #743, #787 (compare your Dijkstra and Bellman-Ford solutions)

---

---

# PHASE 4 — Minimum Spanning Tree
## Week 4: Union-Find + Kruskal

---

### Step 4.1 — Union-Find (DSU)
**Time estimate: 4 hours**

This is a standalone data structure worth understanding deeply — used beyond just MST.

#### Interface
```cpp
struct DSU {
    void makeSet(int x);
    int find(int x);           // with path compression
    bool unite(int x, int y);  // with union by rank
    bool connected(int x, int y);
};
```

#### Two Optimizations (BOTH required)

**Path Compression** — during `find`, make every node point directly to root:
```
Before: 1 → 2 → 3 → 4 (root)
find(1) with path compression:
After:  1 → 4, 2 → 4, 3 → 4
```

**Union by Rank** — always attach smaller tree under larger:
```
rank[4] = 2, rank[7] = 1
unite(4, 7): attach 7 under 4 (not vice versa)
```

#### Why These Matter
Without optimizations: O(n) per operation
With both: O(α(n)) per operation — effectively O(1)
α is the inverse Ackermann function — grows slower than log(log(log(n)))

---

### Step 4.2 — Kruskal's MST
**Time estimate: 3–4 hours**

#### Algorithm Steps
```
1. Collect all edges → sort by weight (greedy choice)
2. Initialize DSU with all nodes
3. For each edge (u, v, w) in sorted order:
   If find(u) != find(v):     ← not in same component
     Add edge to MST
     unite(u, v)              ← merge components
4. Stop when MST has V-1 edges
```

#### Correctness Intuition (Cut Property)
For any cut of the graph into two sets S and V-S, the minimum weight edge crossing the cut is always in some MST. Kruskal's greedily picks these edges.

#### Acceptance Criteria
- [ ] Returns V-1 edges for a connected graph
- [ ] Minimum total weight (verify against brute force on small graph)
- [ ] Handles disconnected graphs (returns forest)
- [ ] Works correctly with DSU path compression

**LeetCode:** #1135 Connecting Cities With Minimum Cost, #1584 Min Cost to Connect All Points

---

---

# PHASE 5 — Connectivity
## Week 5: Tarjan SCC + Topological Sort

---

### Step 5.1 — Tarjan's SCC
**Time estimate: 6–7 hours**

This is the hardest algorithm in the project. Take your time.

#### What is an SCC?
A maximal set of nodes where every node is reachable from every other node via directed edges.

#### Two Arrays — Understand These Cold
- `disc[u]` — discovery time of node u in DFS (when first visited)
- `low[u]` — lowest discovery time reachable from subtree rooted at u

#### SCC Root Condition
```
u is the root of an SCC if and only if: low[u] == disc[u]
```
This means no node in u's subtree can reach any ancestor of u.

#### The Stack
Maintain a stack of nodes in current DFS path.
When SCC root is found, pop all nodes until u is popped — those form the SCC.

#### Step-by-Step Before Coding
1. Draw a directed graph with 6–7 nodes and 2–3 SCCs
2. Manually run Tarjan's, tracking disc[], low[], and stack at each step
3. Verify your SCC identification matches visual inspection

---

### Step 5.2 — Topological Sort
**Time estimate: 3–4 hours**

#### Two Implementations

**Kahn's Algorithm (BFS-based)**
```
1. Compute in-degree of all nodes
2. Queue all nodes with in-degree 0
3. While queue not empty:
   a. Pop node u, add to result
   b. For each neighbor v: decrement in-degree[v]
   c. If in-degree[v] == 0: push to queue
4. If result.size() != V: graph has cycle
```

**DFS-based**
```
1. DFS from each unvisited node
2. When DFS finishes a node (all neighbors processed): push to stack
3. Result is stack in reverse order
```

#### When to Use Each
- Kahn's: easier to detect cycles, natural BFS order
- DFS-based: simpler recursive code, natural for dependency resolution

**LeetCode:** #207 Course Schedule, #210 Course Schedule II

---

---

# PHASE 6 — Real Data
## Week 6: File I/O + OpenStreetMap Data

---

### Step 6.1 — CSV Loader
**Time estimate: 3 hours**

#### Format to Support
```csv
# Edge list format: source,destination,weight
1,2,4
1,3,2
2,3,1
3,4,5
```

#### Error Handling Required
- File not found → throw with filename
- Malformed line → throw with line number and content
- Negative weight on undirected graph → warn but allow

---

### Step 6.2 — OpenStreetMap Data
**Time estimate: 4–5 hours**

#### Getting Real Data
```python
# Run this Python script to download road network
import osmnx as ox

G = ox.graph_from_place("IIT Kharagpur, India", network_type="drive")

# Export as edge list CSV
edges = ox.graph_to_gdfs(G, nodes=False)
edges[['u', 'v', 'length']].to_csv('kharagpur_roads.csv', index=False)
```

#### What to Demonstrate with Real Data
- Shortest path between two campus buildings (use node IDs from OSM)
- A\* vs Dijkstra nodes visited on real map
- Visualize the road network graph

---

### Step 6.3 — JSON Exporter
**Time estimate: 2–3 hours**

#### D3.js Compatible Format
```json
{
  "nodes": [
    {"id": 1, "label": "Node 1"},
    {"id": 2, "label": "Node 2"}
  ],
  "links": [
    {"source": 1, "target": 2, "weight": 4},
    {"source": 1, "target": 3, "weight": 2}
  ],
  "algorithm_steps": [1, 3, 2, 4]
}
```

---

---

# PHASE 7 — Visualizer + Benchmarks
## Week 7

---

### Step 7.1 — D3.js Visualizer
**Time estimate: 4–5 hours**

#### Features Required
- Force-directed layout (nodes repel, edges attract)
- Color nodes by visit order (gradient from blue → red)
- Animate algorithm traversal step-by-step
- Highlight shortest path in red
- Controls: algorithm selector, speed slider, step button

#### D3.js Force Simulation Starter
```javascript
const simulation = d3.forceSimulation(data.nodes)
    .force("link", d3.forceLink(data.links).id(d => d.id))
    .force("charge", d3.forceManyBody().strength(-300))
    .force("center", d3.forceCenter(width/2, height/2));
```

---

### Step 7.2 — Benchmarks
**Time estimate: 3–4 hours**

#### Benchmark Matrix

| Test | Graph Size | What to Measure |
|---|---|---|
| BFS vs DFS | 1k, 10k, 100k nodes | Time + memory |
| Dijkstra vs A\* | 1k, 10k grid nodes | Time + nodes visited |
| Dijkstra vs Bellman-Ford | 100, 1k, 10k | Time (show Dijkstra wins) |
| Kruskal dense vs sparse | Same V, varying E | Time |

#### Expected Results to Verify Against
- A\* should visit 5–15x fewer nodes than Dijkstra on grid graphs
- Dijkstra should be 5–10x faster than Bellman-Ford on positive weights
- DSU with both optimizations should show near-constant time per operation

---

---

# PHASE 8 — Polish
## Week 8

---

### Step 8.1 — Unit Tests
**Time estimate: 4 hours**

For each algorithm, write at minimum:

| Test Case | Why |
|---|---|
| Empty graph | Edge case — don't crash |
| Single node | Edge case |
| Disconnected graph | Algorithm must handle gracefully |
| Known small graph | Verify correctness against manual trace |
| Large random graph | Performance regression test |

---

### Step 8.2 — README
**Time estimate: 2 hours**

#### Sections Required
1. What this project is + motivation
2. Algorithms implemented (table with complexity)
3. Prerequisites + build instructions
4. Usage examples with code
5. Benchmark results (with actual numbers from your machine)
6. What I learned
7. Future improvements

---

### Step 8.3 — Interview Prep
**Time estimate: 3 hours**

For each algorithm, prepare answers to:

| Question | Algorithm |
|---|---|
| Why adjacency list over matrix? | Graph fundamentals |
| Why BFS finds shortest path but DFS doesn't? | BFS |
| Why does Dijkstra fail with negative weights? | Dijkstra |
| What makes A\* heuristic admissible? | A\* |
| Why V-1 iterations in Bellman-Ford? | Bellman-Ford |
| What is the cut property of MST? | Kruskal |
| What do disc[] and low[] represent in Tarjan's? | SCC |
| How does Kahn's algorithm detect cycles? | Topological Sort |

Practice answering each in under 90 seconds without notes.

---

---

# Algorithm Complexity Reference Card

| Algorithm | Time | Space | When to Use |
|---|---|---|---|
| BFS | O(V+E) | O(V) | Shortest path (unweighted), level traversal |
| DFS | O(V+E) | O(V) | Cycle detection, topological sort, SCC |
| Dijkstra | O(E log V) | O(V) | Shortest path (non-negative weights) |
| A\* | O(E log V) | O(V) | Shortest path with good heuristic |
| Bellman-Ford | O(VE) | O(V) | Shortest path (negative weights) |
| Kruskal | O(E log E) | O(V) | Minimum spanning tree |
| Tarjan SCC | O(V+E) | O(V) | Strongly connected components |
| Topological Sort | O(V+E) | O(V) | DAG ordering |

---

# CV Framing Template

```
Graph Algorithm Engine | C++17, CMake, SFML, D3.js | [Month Year]

Built a generic templated graph engine in C++ implementing 7 algorithms
(BFS, DFS, Dijkstra, A*, Bellman-Ford, Kruskal MST, Tarjan SCC).
Applied to real OpenStreetMap road network data (50k+ nodes, KGP campus).
A* demonstrated 12x fewer node visits vs Dijkstra on geographic graphs.
Includes D3.js interactive visualizer with step-by-step algorithm animation.
```

---

# Common Mistakes to Avoid

| Mistake | Consequence | Fix |
|---|---|---|
| Not marking visited on push (BFS) | Infinite loop | Mark when pushing to queue |
| Using max-heap for Dijkstra | Wrong shortest path | Use `greater<>` for min-heap |
| Forgetting stale entry check | Slower but correct | Add `if (d > dist[u]) continue` |
| Ignoring negative cycles (Bellman-Ford) | Wrong results silently | Always run V-th iteration check |
| Not using path compression in DSU | O(n) instead of O(α(n)) | Always compress on find |
| Running Tarjan on undirected graph | Nonsensical results | SCC is for directed graphs only |
| Topological sort on cyclic graph | Partial result, no error | Check result.size() == V |
