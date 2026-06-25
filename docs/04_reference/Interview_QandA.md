# Interview Q&A — All Algorithms

40+ questions with complete answers. These are real interview questions at SWE/quant companies. The goal is to answer each in **under 90 seconds** without notes.

**How to practice**: Cover the answer, read the question, answer it aloud, then check.

---

## Graph Fundamentals

---

**Q1: What is the difference between an adjacency list and an adjacency matrix? When would you choose each?**

**A**: An adjacency list stores, for each node, only the nodes it directly connects to. An adjacency matrix stores a V×V grid where entry [i][j] = weight of edge i→j (or 0 if no edge).

| | Adjacency List | Adjacency Matrix |
|---|---|---|
| Space | O(V + E) | O(V²) |
| Check edge (u,v) | O(degree(u)) | O(1) |
| Get all neighbors | O(degree(u)) | O(V) |

**Choose adjacency list when**: The graph is sparse (E << V²). Most real-world graphs (road networks, social networks) are sparse. This project uses adjacency lists.

**Choose adjacency matrix when**: The graph is dense (E ≈ V²) and you need frequent O(1) "does edge exist?" queries, or you're implementing Floyd-Warshall (which accesses [i][j] and [j][k]).

---

**Q2: What is the difference between directed and undirected graphs?**

**A**: In a directed graph (digraph), edge `u→v` only means you can go from u to v, not from v to u. In an undirected graph, edge `u-v` means you can go both ways.

Implementation: when adding an undirected edge `(u, v)`, we add to both `adjList[u]` and `adjList[v]`. For directed, we only add to `adjList[u]`.

Applications:
- **Directed**: Social media following (A follows B ≠ B follows A), web page links, dependency graphs
- **Undirected**: Friendship (mutual), road networks (bidirectional roads), electrical circuits

---

**Q3: What is a connected component vs a strongly connected component?**

**A**: 
- **Connected component** (undirected graphs): A maximal set of nodes where every pair can reach each other via any path.
- **Strongly Connected Component** (directed graphs): A maximal set of nodes where every node can reach every other node via directed edges.

In a directed graph, having a path from A to B does NOT guarantee a path from B to A. SCCs capture exactly the groups where you can navigate freely in both directions.

Example: `0→1→2→0` is one SCC. `1→3` means 3 is in 0's SCC only if there's a path back (3→...→1 or 3→...→0).

---

## BFS

---

**Q4: Why does BFS find the shortest path in unweighted graphs?**

**A**: BFS explores nodes in order of hop count. It processes all nodes at distance 1 before any at distance 2, all at distance 2 before any at distance 3, and so on. This is because it uses a FIFO queue — nodes added at level k are all processed before nodes added at level k+1.

The FIRST time BFS reaches a node, it has traveled via the fewest possible hops. Any later path to the same node would take MORE hops (since all shorter paths were already explored).

This reasoning breaks for weighted graphs: "fewer hops" ≠ "less total cost." A path with 3 hops of weight 1 each (cost 3) beats a 1-hop path of weight 100.

---

**Q5: Why do we mark nodes as visited when PUSHING to the queue, not when POPPING?**

**A**: If you mark visited when popping, the same node can be pushed multiple times. Consider:

```
0 → 1, 0 → 2, 1 → 3, 2 → 3
```

If we mark when popping:
- Push 1 and 2 (from node 0)
- Pop 1, push 3, (don't mark 1 yet)
- Pop 2, push 3 again! (3 isn't marked yet)
- Pop 3 twice (it appears twice in the queue)
- Pop 3 twice (it appears twice in the queue)

In a large graph this leads to exponential redundant work and potentially infinite loops (with cycles). Marking when pushing ensures a node only enters the queue once.

---

**Q6: BFS vs DFS — when would you prefer DFS?**

**A**: 
- **BFS**: Shortest path (hop count), level-order traversal, finding all nodes within distance k
- **DFS**: Cycle detection, topological sort, SCC algorithms (Tarjan), maze solving when you just need A path (not the shortest), checking if a path exists

DFS is also more memory efficient for deep graphs — its stack depth is O(max_depth), while BFS queue can hold O(max_width) nodes, which for a balanced tree is O(V).

---

## Dijkstra

---

**Q7: How does Dijkstra's algorithm work? (Explain the main loop)**

**A**: Dijkstra greedily builds shortest paths outward from the source.

1. Start with `dist[source] = 0`, all others `= ∞`
2. Push `(0, source)` to a min-heap priority queue
3. Loop: pop the node `u` with the current smallest distance `d`
   - If `d > dist[u]`: this is a stale entry (a shorter path was found later) — skip
   - Otherwise: for each neighbor `v` with edge weight `w`, if `dist[u] + w < dist[v]`, update `dist[v] = dist[u] + w` and push the new entry
4. When the queue is empty, all shortest distances are finalized

The key insight: once a node is popped with a non-stale entry, its shortest distance is **permanently settled** — no future path can be shorter (because all future distances in the min-heap are ≥ current, and all weights are ≥ 0).

---

**Q8: Why does Dijkstra fail with negative edge weights?**

**A**: Dijkstra assumes that once a node is settled (popped from the priority queue), its distance is final. This is valid for non-negative weights because you can only make distances larger by following more edges.

With negative weights: suppose we settle node B with distance 5, then later find edge C→B with weight -10. The path through C gives B a distance of (dist[C] + (-10)), which might be less than 5. But Dijkstra already considers B settled and won't reconsider it.

Example:
```
A→B (weight 3)
A→C (weight 4)
C→B (weight -2)

Dijkstra: pops B first with dist=3. Later finds A→C→B = 4-2=2, but B is already settled!
Correct answer: 2 (via A→C→B)
```

---

**Q9: What is the time complexity of Dijkstra and where does each part come from?**

**A**: O(E log V) with a binary heap.

- Each edge (E total) causes at most one priority queue `push` operation: O(log V) per push → O(E log V) total
- Each node (V total) is popped from the priority queue at most once (stale entries are skipped): O(log V) per pop → O(V log V) total
- Total: O((V + E) log V) = O(E log V) for connected graphs where E ≥ V-1

With a Fibonacci heap, it's O(E + V log V) — better for dense graphs, but complex to implement.

---

## A\*

---

**Q10: What is an admissible heuristic and why does it guarantee optimality?**

**A**: A heuristic `h(n)` is admissible if it NEVER OVERESTIMATES the true cost from node n to the goal: `h(n) ≤ actual_cost(n, goal)`.

**Why it guarantees optimality**: When A* pops the goal node, its `g` value is the actual cost of the path found. If the heuristic never overestimated, no other unexplored path could have a smaller actual cost — because those paths have `f = g + h ≥ g + 0 = g`, and since they haven't been expanded yet, their `g` values alone are ≥ the optimal cost. *(Proof by contradiction: if a shorter path existed, its f-value when enqueued would have been ≤ optimal, and A* would have expanded it first.)*

Examples:
- **Admissible**: Euclidean distance (straight line ≤ any actual path), Manhattan distance for 4-directional grid movement
- **NOT admissible**: Euclidean distance × 2 (overestimates) — A* might skip the optimal path

---

**Q11: When does A* degenerate to Dijkstra?**

**A**: When `h(n) = 0` for all n (the zero heuristic). Then `f(n) = g(n) + 0 = g(n)`, and A* prioritizes exactly by cost-so-far — exactly like Dijkstra. A* provides no guidance and explores in all directions equally.

When does `h(n) = 0` make sense? When you have no geometric information about the graph (e.g., abstract node IDs with no coordinates). Dijkstra is the correct choice then.

---

## Bellman-Ford

---

**Q12: Why does Bellman-Ford require exactly V-1 iterations?**

**A**: The longest possible SIMPLE path (no repeated nodes) in a V-node graph has V-1 edges. 

In each iteration, Bellman-Ford relaxes all edges. After iteration k, all shortest paths with AT MOST k edges are correctly computed. Therefore, after V-1 iterations, all shortest paths (which can have at most V-1 edges) are found.

If there were no negative cycles, the distances stabilize after V-1 iterations. A V-th iteration that still updates any distance proves there's a negative cycle (a cycle that can be traversed repeatedly to make paths shorter and shorter without bound).

---

**Q13: When would you use Bellman-Ford instead of Dijkstra?**

**A**:
- **Use Bellman-Ford**: negative edge weights, need to detect negative cycles (currency arbitrage, financial models)
- **Use Dijkstra**: non-negative weights, performance matters

Bellman-Ford is O(VE) — much slower than Dijkstra's O(E log V). For a graph with 1000 nodes and 5000 edges: Dijkstra ≈ 65,000 operations; Bellman-Ford ≈ 5,000,000 operations. Only use Bellman-Ford when Dijkstra is incorrect.

---

## Kruskal / Union-Find

---

**Q14: What is a Minimum Spanning Tree?**

**A**: Given a connected undirected weighted graph, an MST is a subgraph that:
1. Connects all V nodes
2. Has exactly V-1 edges (a tree has no cycles and connects all nodes with minimum edges)
3. Has the minimum possible total edge weight among all such spanning trees

**Why V-1 edges**: A tree with V nodes has exactly V-1 edges (provable by induction). Adding any more edge creates a cycle; removing any edge disconnects the graph.

**Applications**: Network infrastructure design (connecting cities with minimum cable length), cluster analysis, approximation algorithms for Traveling Salesman.

---

**Q15: Explain path compression in Union-Find. What does it do and why does it work?**

**A**: When `find(x)` traverses from x to its root, path compression makes all visited nodes point DIRECTLY to the root:

```
Before find(1):    After find(1):
    4               4
    │             / │ \
    3            1  2  3
    │
    2
    │
    1
```

This means all future `find()` calls for nodes 1, 2, 3 go directly to root 4 in O(1).

**Why it works**: The root doesn't change (we haven't modified the set membership), so making intermediate nodes point directly to the root is valid. It doesn't affect correctness, only speed.

Combined with union by rank: O(α(n)) amortized per operation, where α is the inverse Ackermann function — for any practical input size, α(n) ≤ 4. Effectively constant time.

---

**Q16: How does Kruskal's algorithm detect cycles?**

**A**: Using the Disjoint Set Union (DSU). Before adding an edge (u, v):
- `find(u)` returns u's component root
- `find(v)` returns v's component root
- If both roots are the same, u and v are ALREADY in the same component — adding the edge would create a cycle. Skip it.
- If roots differ, the edge is safe — add it and `unite(u, v)` to merge the components.

---

## Tarjan's SCC

---

**Q17: What do `disc[u]` and `low[u]` represent in Tarjan's algorithm?**

**A**:
- `disc[u]`: The DFS discovery time — a timestamp (1, 2, 3, ...) assigned when node u is first visited. It never changes.
- `low[u]`: The smallest `disc` value reachable from u's DFS subtree, including u itself and ancestors reachable via back-edges.

Think of `low[u]` as "how far back in the DFS tree can I reach from u's subtree?" If I can reach an ancestor with `disc = 1`, then `low[u] = 1`.

---

**Q18: Why is the condition `low[u] == disc[u]` the SCC root condition?**

**A**: `low[u] == disc[u]` means: the earliest ancestor reachable from u's subtree is u itself. In other words, no node in u's DFS subtree has a back-edge to any node that was discovered BEFORE u.

This means u is not "inside" any larger cycle that spans above it in the DFS tree. Everything in u's subtree that goes in a cycle loops back within u's subtree — making it a self-contained strongly connected component with u as its "oldest" member (root).

When this condition triggers, we pop the stack from the top down to u — those nodes form the SCC.

---

**Q19: Why does Tarjan's require a separate `onStack` set?**

**A**: To distinguish between two types of already-visited neighbors:

1. **On the stack (back-edge to ancestor)**: This neighbor is part of the current DFS path — there's a cycle going back to it. Update `low[u]` because we can reach this ancestor.

2. **Not on the stack (cross-edge to completed SCC)**: This neighbor has already been popped and belongs to a previously found SCC. We should NOT update `low[u]` with its `disc` value — doing so would incorrectly merge separate SCCs.

`visited.count(v) > 0` doesn't distinguish these two cases. `onStack.count(v) > 0` does.

---

## Topological Sort

---

**Q20: What is topological sort and when is it applicable?**

**A**: Topological sort is a linear ordering of nodes in a directed graph such that for every directed edge `u→v`, node u appears before node v in the ordering.

**Only applicable to DAGs** (Directed Acyclic Graphs). If there's a cycle (A→B→C→A), there's no valid ordering — you'd need A before B (because A→B) and C before A (because C→A) and B before C (because B→C), which is impossible.

**Applications**: Build systems (compile dependencies first), task scheduling (prerequisites before tasks), course planning (take CS101 before CS201).

---

**Q21: How does Kahn's algorithm detect cycles?**

**A**: Kahn's algorithm can only process nodes with in-degree 0 (all prerequisites satisfied). If there's a cycle, the nodes IN the cycle never reach in-degree 0 — each node in the cycle has an incoming edge from another node in the cycle.

So the result vector won't contain those nodes. **Cycle detected when**: `result.size() < nodeCount()`. The "missing" nodes are part of cycles.

---

**Q22: What is the difference between Kahn's and DFS-based topological sort?**

**A**:
- **Kahn's (BFS-based)**: Uses in-degree tracking. Outputs nodes as their prerequisites are completed. Natural for "which task can I start now?" reasoning. Easier cycle detection.
- **DFS-based**: Recursively explores dependencies first. Pushes a node to a result stack only AFTER all its outgoing paths are fully explored. Naturally gives "deepest dependency first" ordering.

Both produce valid topological orderings, but they may produce different orderings for the same graph (multiple valid orderings can exist).

---

## C++ and Implementation

---

**Q23: What is RAII? Give an example from this codebase.**

**A**: RAII = Resource Acquisition Is Initialization. The idea: tie a resource's lifetime to an object's lifetime. Acquire the resource in the constructor; release it in the destructor. Since destructors run automatically (even if exceptions are thrown), the resource is always cleaned up.

Example from this project — `Timer`:
```cpp
{
    Timer t("Dijkstra");  // Constructor: clock starts
    dijkstra(g, src);
}   // Destructor fires: prints elapsed time. Always. Even if exception thrown.
```

Other classic RAII uses: file handles (destructor closes the file), mutex locks (destructor releases the lock), memory (smart pointers' destructor frees memory).

---

**Q24: Why are template definitions in header files (no .cpp)?**

**A**: Templates are not compiled into actual code until they're instantiated (used). The compiler needs to see the full template definition at the point where it generates the concrete code for a specific type.

If you put `template<typename T> void bfs(...)` in a `.cpp` file, the compiler compiles that `.cpp` separately and only generates code for the types used in THAT file. When another `.cpp` file tries to use `bfs<int>`, the linker can't find the implementation.

Solution: Keep template definitions in `.hpp` files — every file that `#include`s them gets the full definition and can instantiate it.

---

**Q25: What is `std::numeric_limits<T>::max()` and why is it used instead of a hardcoded large number?**

**A**: `std::numeric_limits<T>::max()` gives the largest representable value for type `T`. For `int`, it's 2,147,483,647. For `double`, there's also `::infinity()`.

**Why not `99999999`?**: 
1. If your graph weights are large, 99999999 might not be "infinity"
2. `numeric_limits<int>::max() + 1` overflows (wraps to a negative number) — arithmetic with "infinity" becomes dangerous. Better to use `double` and `::infinity()` which has proper arithmetic behavior.
3. `numeric_limits<T>::max()` works for any numeric type — your template still works if T is `long long` or `float`.

---

**Q26: What is the difference between `std::map` and `std::unordered_map`?**

**A**:
| | `std::map` | `std::unordered_map` |
|---|---|---|
| Internal structure | Red-black tree (sorted) | Hash table |
| Lookup | O(log N) | O(1) average |
| Insertion | O(log N) | O(1) average |
| Iteration order | Sorted by key | Random |
| Requires | `operator<` on key type | Hash function for key type |

We use `unordered_map` everywhere in this project for O(1) lookups. The sorted order of `map` is rarely needed in graph algorithms. The `GraphUtils::SafeHash` provides the hash function for custom types like `std::pair<int,int>`.

---

**Q27: Why does the priority queue use `pair<double, NodeType>` instead of `pair<NodeType, double>`?**

**A**: `std::pair` compares lexicographically — first by `.first`, then by `.second` if `.first` is equal.

If we stored `(NodeType, double)`, the priority queue would first compare by NodeType. If NodeType is `int`, this means it would pick the lowest node ID first — which is not what we want.

By storing `(double, NodeType)`, the priority queue compares by distance first, which is exactly the behavior Dijkstra needs: always process the node with the smallest current distance.

---

## System Design Questions

---

**Q28: How would you modify this engine to handle a graph that doesn't fit in memory?**

**A**: For very large graphs (billions of nodes, like the web graph):

1. **External memory BFS**: Store the graph on disk in an adjacency list format. Only load the edges for the current "frontier" (nodes being processed at the current BFS level) into memory.

2. **Compressed representations**: Store edges as sorted arrays instead of hash maps to reduce overhead. Use delta-compression for node IDs (store differences between consecutive IDs, which are small for sorted lists).

3. **Distributed graph processing**: Frameworks like Apache Spark's GraphX or Google's Pregel partition the graph across machines. Each machine holds a subgraph; messages pass between machines for cross-partition edges.

4. **Streaming algorithms**: Process edges one at a time from a stream, maintaining compact summaries (sketches) of the graph structure.

---

**Q29: If you needed to run Dijkstra on a graph with 10 billion nodes, what would change?**

**A**:

1. **Memory**: 10^10 nodes × ~40 bytes per distance/prev entry = ~400GB for dist[] and prev[] alone. We can't store distances to all nodes.

2. **Bidirectional Dijkstra**: Run Dijkstra from both source and destination simultaneously. Stop when they meet. Expected to expand ~√(V) nodes instead of ~V nodes, which halves the memory and time.

3. **Landmarks/A\***: If we have coordinates (like a road network), A* with geographic heuristic dramatically reduces the number of nodes expanded (12× fewer, as shown in benchmarks).

4. **Highway hierarchies**: Preprocess the graph to identify "important" nodes (highways). Long-distance queries only use the highway network, short-distance queries use local edges. Used by Google Maps and similar.

---

**Q30: How would you test that your Dijkstra implementation is correct?**

**A**: Layers of testing (exactly what `test_dijkstra.cpp` does):

1. **Empty graph**: No crash, empty result
2. **Single node**: Distance to self is 0
3. **Simple known graph**: Hand-compute the shortest path, assert the code matches
4. **Disconnected graph**: Unreachable nodes have `dist = ∞`, path is empty
5. **Different weight types**: Works for both `int` and `double` weights
6. **Negative weight edge**: Verify it gives wrong answer (and that you'd use Bellman-Ford instead)
7. **Large random graph**: Property-based test — run Bellman-Ford on the same graph and assert both give the same shortest distances (since the graph has no negative weights, both are correct)
8. **Performance**: 10,000+ node graph runs in under 1 second

---

## Trick Questions

---

**Q31: Can you run BFS on a weighted graph?**

**A**: You CAN run BFS on a weighted graph — it will traverse correctly. But the result won't be the SHORTEST WEIGHTED path. BFS finds the shortest hop path. For weighted shortest path, use Dijkstra or A*.

Exception: if all weights are identical (e.g., all = 1), BFS and Dijkstra give the same result, and BFS is faster (O(V+E) vs O(E log V)).

---

**Q32: Is it possible for Dijkstra and A* to give different answers?**

**A**: No — if the heuristic is admissible, A* always finds the same optimal path as Dijkstra. The DIFFERENCE is which nodes they expand:

- Dijkstra: expands ALL nodes with `dist ≤ optimal_distance`
- A*: expands FEWER nodes, guided by the heuristic toward the goal

Same optimal path length, fewer nodes visited.

If the heuristic is NOT admissible (overestimates), A* might find a suboptimal path.

---

**Q33: What happens if you run Kruskal's on a directed graph?**

**A**: Kruskal's finds a Minimum SPANNING TREE — a concept defined for undirected graphs. For directed graphs, the equivalent is the Minimum Spanning ARBORESCENCE (directed MST), which requires a different algorithm (Edmonds'/Chu-Liu algorithm).

In this project, `kruskal()` explicitly throws `std::invalid_argument` if called on a directed graph. This is the correct defensive behavior.

---

**Q34: Can a graph have multiple valid topological orderings?**

**A**: Yes! If nodes have no dependency relationship between them, they can appear in any relative order.

For `CS101→CS301` and `CS102→CS301` (CS101 and CS102 are both prerequisites for CS301), both:
- `CS101, CS102, CS301`
- `CS102, CS101, CS301`

are valid orderings. There's no constraint on the relative order of CS101 and CS102.

Kahn's algorithm and DFS-based topological sort may produce different valid orderings of the same graph.

---

**Q35: How does this engine's `Graph` class handle duplicate edges?**

**A**: `addEdge(u, v, w)` simply appends to the adjacency list without checking for existing edges. Calling `addEdge(1, 2, 5)` twice results in two entries for the edge 1→2.

This means `edgeCount()` would return 2 for that edge pair. Algorithms like Dijkstra would process it twice (adding some redundant work but not incorrect results). Kruskal would sort and process both, but only one would ever be added to the MST (the second would create a cycle and be rejected by DSU).

For production use, you'd add a `hasEdge()` check before `addEdge`, or implement a `setEdge()` that updates existing edges. This is a potential future improvement.
