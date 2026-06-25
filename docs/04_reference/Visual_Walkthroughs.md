# Visual Algorithm Walkthroughs — Step-by-Step ASCII Traces

This file traces every algorithm **by hand** on concrete small graphs. Reading these should make the code in each `.hpp` file immediately obvious.

**How to use this file**: Before reading the code, read the trace here. Then read the code and try to match each code block to a step in the trace.

---

## The Sample Graph Used Throughout

```
Directed version (used by BFS, DFS, Dijkstra, A*, Bellman-Ford, Tarjan):

         4          5
    0 ──────► 1 ──────► 3
    │          │          │
  2 │        1 │          │ 3
    ▼          ▼          ▼
    2 ──────► 3 ◄──────── 4
         2          2

Wait — let's use a cleaner, unambiguous example:

    0 ──4──► 1 ──5──► 3
    │                   ▲
    │2                  │3
    ▼                   │
    2 ──1──► 3 ──────────┘
         
Simpler. Clean 5-node graph:
    A ──4──► B
    │         │
    2│        │1
    ▼         ▼
    C ──1──► D ──5──► E

(Nodes: A, B, C, D, E. Source: A. Destination: E)
```

Let's use this graph for all traces:

```
Edges (directed, weighted):
A → B  (weight 4)
A → C  (weight 2)
B → D  (weight 1)
C → D  (weight 1)
D → E  (weight 5)
B → E  (weight 8)     ← longer path for contrast

Unweighted version (for BFS/DFS): same edges, ignore weights
```

---

## BFS Trace

**Goal**: Find the shortest HOP-COUNT path from A to E.

```
Initial state:
Queue: [A]
Visited: {A}
Prev: {}

──────────────────────────────────────
Step 1: Pop A
──────────────────────────────────────
Queue before pop: [A]
Current node: A
Neighbors of A: B, C

  → B not visited: mark visited, push to queue, prev[B] = A
  → C not visited: mark visited, push to queue, prev[C] = A

Queue: [B, C]
Visited: {A, B, C}
Prev: {B:A, C:A}

──────────────────────────────────────
Step 2: Pop B  (B was pushed first → comes out first: FIFO)
──────────────────────────────────────
Current node: B
Neighbors of B: D, E

  → D not visited: mark visited, push, prev[D] = B
  → E not visited: mark visited, push, prev[E] = B

Queue: [C, D, E]
Visited: {A, B, C, D, E}
Prev: {B:A, C:A, D:B, E:B}

──────────────────────────────────────
Step 3: Pop C
──────────────────────────────────────
Current node: C
Neighbors of C: D

  → D already visited → SKIP

Queue: [D, E]

──────────────────────────────────────
Step 4: Pop D
──────────────────────────────────────
Current node: D
Neighbors of D: E

  → E already visited → SKIP

Queue: [E]

──────────────────────────────────────
Step 5: Pop E (destination found!)
──────────────────────────────────────

DONE. Reconstruct path using prev:
E ← prev[E] = B
B ← prev[B] = A
A ← no prev (it's the source)

Path (reversed): A → B → E
Hop count: 2 hops
```

**What BFS tells us**: The shortest HOP path to E is A→B→E (2 hops).
**What BFS does NOT tell us**: This is NOT the cheapest path. A→C→D→E costs 2+1+5=8, while A→B→D→E costs 4+1+5=10, and A→B→E costs 4+8=12. BFS ignores weights!

---

## DFS Trace (Recursive)

**Goal**: Explore as deep as possible before backtracking.

```
Call dfs(A):
  visited = {A}
  result = [A]
  Neighbors of A: B, C (in order)
  
  → Process neighbor B:
    Call dfs(B):
      visited = {A, B}
      result = [A, B]
      Neighbors of B: D, E
      
      → Process neighbor D:
        Call dfs(D):
          visited = {A, B, D}
          result = [A, B, D]
          Neighbors of D: E
          
          → Process neighbor E:
            Call dfs(E):
              visited = {A, B, D, E}
              result = [A, B, D, E]
              Neighbors of E: (none)
              return
            ← Back to dfs(D)
          return
        ← Back to dfs(B)
      
      → Process neighbor E:
        E already visited → SKIP
      return
    ← Back to dfs(A)
  
  → Process neighbor C:
    Call dfs(C):
      visited = {A, B, D, E, C}
      result = [A, B, D, E, C]
      Neighbors of C: D
      D already visited → SKIP
      return
    ← Back to dfs(A)

Final result: [A, B, D, E, C]
```

**Key observation**: DFS goes A→B→D→E all the way to the end BEFORE it processes C. BFS would have visited C at step 2 (same level as B). This is the fundamental difference.

---

## Dijkstra Trace

**Goal**: Find the shortest WEIGHTED path from A to all nodes.

```
Initial state:
dist = {A:0, B:∞, C:∞, D:∞, E:∞}
prev = {}
PQ (min-heap by dist): [(0, A)]

Legend: PQ entries are (distance, node). Min distance is always at top.

──────────────────────────────────────
Step 1: Pop (0, A)   ← smallest distance = A
──────────────────────────────────────
d=0, node=A
Is d > dist[A]? 0 > 0? NO → process

Relax neighbors:
  Edge A→B (weight 4): dist[A]+4 = 4 < dist[B]=∞ → Update! dist[B]=4, prev[B]=A, push (4,B)
  Edge A→C (weight 2): dist[A]+2 = 2 < dist[C]=∞ → Update! dist[C]=2, prev[C]=A, push (2,C)

dist = {A:0, B:4, C:2, D:∞, E:∞}
PQ: [(2,C), (4,B)]   ← C is on top (dist=2 is smallest)

──────────────────────────────────────
Step 2: Pop (2, C)   ← smallest distance = C
──────────────────────────────────────
d=2, node=C
Is d > dist[C]? 2 > 2? NO → process

Relax neighbors:
  Edge C→D (weight 1): dist[C]+1 = 3 < dist[D]=∞ → Update! dist[D]=3, prev[D]=C, push (3,D)

dist = {A:0, B:4, C:2, D:3, E:∞}
PQ: [(3,D), (4,B)]

──────────────────────────────────────
Step 3: Pop (3, D)   ← smallest distance = D
──────────────────────────────────────
d=3, node=D
Is d > dist[D]? 3 > 3? NO → process

Relax neighbors:
  Edge D→E (weight 5): dist[D]+5 = 8 < dist[E]=∞ → Update! dist[E]=8, prev[E]=D, push (8,E)

dist = {A:0, B:4, C:2, D:3, E:8}
PQ: [(4,B), (8,E)]

──────────────────────────────────────
Step 4: Pop (4, B)   ← smallest distance = B
──────────────────────────────────────
d=4, node=B
Is d > dist[B]? 4 > 4? NO → process

Relax neighbors:
  Edge B→D (weight 1): dist[B]+1 = 5 < dist[D]=3? NO → Skip
  Edge B→E (weight 8): dist[B]+8 = 12 < dist[E]=8? NO → Skip

PQ: [(8,E)]

──────────────────────────────────────
Step 5: Pop (8, E)
──────────────────────────────────────
d=8, node=E
No outgoing edges.
PQ: empty

DONE.

Final distances: {A:0, B:4, C:2, D:3, E:8}
Final prev:     {B:A, C:A, D:C, E:D}

Reconstruct path A→E:
E ← prev[E] = D
D ← prev[D] = C
C ← prev[C] = A
Path: A → C → D → E   Cost: 2 + 1 + 5 = 8 ✓
```

**What the stale entry would look like**: If we had updated D's distance and pushed a duplicate, when we'd pop the old (higher-distance) entry, `d > dist[D]` would be true and we'd skip it. This is exactly the "stale entry" optimization.

---

## A\* Trace

**Goal**: Find cheapest path from A to E using a heuristic.

Assume a simple heuristic `h` that estimates remaining distance:
```
h(A, E) = 10
h(B, E) = 7
h(C, E) = 8
h(D, E) = 5
h(E, E) = 0  ← at goal, no remaining distance
```

```
Initial state:
g = {A:0, B:∞, C:∞, D:∞, E:∞}    (actual cost so far)
PQ: [(f=0+10=10, A)]               (f = g + h)

──────────────────────────────────────
Step 1: Pop (f=10, A)
──────────────────────────────────────
g[A] = 0. Process neighbors:

  A→B (cost 4): g[B] = 0+4 = 4, f[B] = 4+7 = 11. Push (11, B)
  A→C (cost 2): g[C] = 0+2 = 2, f[C] = 2+8 = 10. Push (10, C)

PQ: [(10, C), (11, B)]   ← C's f=10 is smaller!

──────────────────────────────────────
Step 2: Pop (f=10, C)
──────────────────────────────────────
g[C] = 2. Process neighbors:

  C→D (cost 1): g[D] = 2+1 = 3, f[D] = 3+5 = 8. Push (8, D)

PQ: [(8, D), (11, B)]

──────────────────────────────────────
Step 3: Pop (f=8, D)
──────────────────────────────────────
g[D] = 3. Process neighbors:

  D→E (cost 5): g[E] = 3+5 = 8, f[E] = 8+0 = 8. Push (8, E)

PQ: [(8, E), (11, B)]

──────────────────────────────────────
Step 4: Pop (f=8, E)  ← E is the goal!
──────────────────────────────────────
DONE. Path cost = g[E] = 8
Path: A → C → D → E   (same as Dijkstra)

Nodes visited: A, C, D, E = 4 nodes
Dijkstra visited: A, C, D, B, E = 5 nodes

A* skipped B entirely because its f=11 was higher than E's f=8!
```

---

## Bellman-Ford Trace

**Graph**: Same but add a negative edge: A→B (weight -1) instead of 4.

```
Edges: A→B(-1), A→C(2), B→D(1), C→D(1), D→E(5), B→E(8)

Initial: dist = {A:0, B:∞, C:∞, D:∞, E:∞}
V = 5 nodes, so run V-1 = 4 rounds.

──────────────────────────────────────
Round 1: Relax ALL edges
──────────────────────────────────────
A→B(-1): dist[A]+(-1) = -1 < ∞ → dist[B] = -1
A→C(2):  dist[A]+2 = 2 < ∞    → dist[C] = 2
B→D(1):  dist[B]+1 = 0 < ∞    → dist[D] = 0
B→E(8):  dist[B]+8 = 7 < ∞    → dist[E] = 7
C→D(1):  dist[C]+1 = 3 < dist[D]=0? NO → skip
D→E(5):  dist[D]+5 = 5 < dist[E]=7? YES → dist[E] = 5

After Round 1: dist = {A:0, B:-1, C:2, D:0, E:5}

──────────────────────────────────────
Round 2: Relax ALL edges again
──────────────────────────────────────
(All distances stay the same — no further relaxation possible)

──────────────────────────────────────
Rounds 3, 4: No changes
──────────────────────────────────────

V-th check (Round 5): Try to relax all edges one more time
  If ANY distance decreases → NEGATIVE CYCLE!
  In this case: no changes → SAFE, no negative cycle.

Final: dist = {A:0, B:-1, C:2, D:0, E:5}
Shortest path A→E = A→B→D→E = -1+1+5 = 5
```

---

## Kruskal MST Trace

**Graph** (undirected): A-B(4), A-C(2), B-D(1), C-D(1), D-E(5), B-E(8)

```
Step 1: Sort all edges by weight:
  B-D(1), C-D(1), A-C(2), A-B(4), D-E(5), B-E(8)

Step 2: Initialize DSU — each node is its own component:
  {A}, {B}, {C}, {D}, {E}

──────────────────────────────────────
Process edge B-D(1):
──────────────────────────────────────
find(B) = B, find(D) = D → different components!
Unite B and D: {A}, {B,D}, {C}, {E}
Add edge B-D to MST. MST edges: [B-D(1)]

──────────────────────────────────────
Process edge C-D(1):
──────────────────────────────────────
find(C) = C, find(D) = B (B is root of {B,D})
Different components → Unite!
Components: {A}, {B,D,C}, {E}
MST edges: [B-D(1), C-D(1)]

──────────────────────────────────────
Process edge A-C(2):
──────────────────────────────────────
find(A) = A, find(C) = B (root of {B,D,C})
Different → Unite!
Components: {A,B,D,C}, {E}
MST edges: [B-D(1), C-D(1), A-C(2)]

──────────────────────────────────────
Process edge A-B(4):
──────────────────────────────────────
find(A) = B (root of {A,B,D,C}), find(B) = B
SAME component! → Adding this edge would create a cycle → SKIP!

──────────────────────────────────────
Process edge D-E(5):
──────────────────────────────────────
find(D) = B (root of {A,B,D,C}), find(E) = E
Different → Unite!
Components: {A,B,D,C,E}   ← All connected!
MST edges: [B-D(1), C-D(1), A-C(2), D-E(5)]

DONE! V-1 = 4 edges. Total weight = 1+1+2+5 = 9
```

---

## Tarjan SCC Trace

**Graph**: 0→1, 1→2, 2→0 (SCC #1: {0,1,2}), 1→3, 3→4, 4→3 (SCC #2: {3,4})

```
disc[] initially empty (unvisited)
low[] initialized as disc when visited
timer = 0
stack = []

──────────────────────────────────────
dfs(0):
──────────────────────────────────────
disc[0]=1, low[0]=1, timer=1
stack = [0], onStack = {0}

  → neighbor 1 (unvisited) → recurse:

  ──────────────────────────────────────
  dfs(1):
  ──────────────────────────────────────
  disc[1]=2, low[1]=2, timer=2
  stack = [0,1], onStack = {0,1}

    → neighbor 2 (unvisited) → recurse:

    ──────────────────────────────────────
    dfs(2):
    ──────────────────────────────────────
    disc[2]=3, low[2]=3, timer=3
    stack = [0,1,2], onStack = {0,1,2}

      → neighbor 0: VISITED and ON STACK!
        low[2] = min(low[2]=3, disc[0]=1) = 1

    CHECK: low[2]=1 == disc[2]=3? NO → not an SCC root

    ← return from dfs(2)

    Back in dfs(1):
    low[1] = min(low[1]=2, low[2]=1) = 1

    → neighbor 3 (unvisited) → recurse:

    ──────────────────────────────────────
    dfs(3):
    ──────────────────────────────────────
    disc[3]=4, low[3]=4, timer=4
    stack = [0,1,2,3], onStack = {0,1,2,3}

      → neighbor 4 (unvisited) → recurse:

      ──────────────────────────────────────
      dfs(4):
      ──────────────────────────────────────
      disc[4]=5, low[4]=5, timer=5
      stack = [0,1,2,3,4], onStack = {0,1,2,3,4}

        → neighbor 3: VISITED and ON STACK!
          low[4] = min(low[4]=5, disc[3]=4) = 4

      CHECK: low[4]=4 == disc[4]=5? NO → not an SCC root

      ← return from dfs(4)

      Back in dfs(3):
      low[3] = min(low[3]=4, low[4]=4) = 4

    CHECK: low[3]=4 == disc[3]=4? YES! → SCC ROOT FOUND!
    Pop stack until we reach node 3:
      Pop 4 → not 3, add to SCC
      Pop 3 → IS 3, add to SCC, stop
    SCC #1 = {4, 3}  (reversed during pop)
    onStack = {0,1,2}

    ← return from dfs(3)

  CHECK: low[1]=1 == disc[1]=2? NO → not an SCC root

  ← return from dfs(1)

Back in dfs(0):
low[0] = min(low[0]=1, low[1]=1) = 1

CHECK: low[0]=1 == disc[0]=1? YES! → SCC ROOT FOUND!
Pop stack until we reach node 0:
  Pop 2 → not 0, add to SCC
  Pop 1 → not 0, add to SCC
  Pop 0 → IS 0, add to SCC, stop
SCC #2 = {2, 1, 0}

Final SCCs: [{4,3}, {2,1,0}]
```

---

## Topological Sort Trace (Kahn's Algorithm)

**Graph**: CS101→CS201, CS101→CS202, CS201→CS301, CS202→CS301

```
Step 1: Compute in-degrees:
CS101: in-degree = 0  (no prerequisites)
CS201: in-degree = 1  (CS101)
CS202: in-degree = 1  (CS101)
CS301: in-degree = 2  (CS201 AND CS202)

Step 2: Queue all nodes with in-degree 0:
Queue: [CS101]
Result: []

──────────────────────────────────────
Iteration 1: Pop CS101
──────────────────────────────────────
Result: [CS101]
Decrement in-degrees of CS101's neighbors:
  CS201: 1-1 = 0 → in-degree 0! Push to queue.
  CS202: 1-1 = 0 → in-degree 0! Push to queue.

Queue: [CS201, CS202]

──────────────────────────────────────
Iteration 2: Pop CS201
──────────────────────────────────────
Result: [CS101, CS201]
Decrement CS201's neighbors:
  CS301: 2-1 = 1 → still has prerequisites, don't queue yet

Queue: [CS202]

──────────────────────────────────────
Iteration 3: Pop CS202
──────────────────────────────────────
Result: [CS101, CS201, CS202]
Decrement CS202's neighbors:
  CS301: 1-1 = 0 → in-degree 0! Push to queue.

Queue: [CS301]

──────────────────────────────────────
Iteration 4: Pop CS301
──────────────────────────────────────
Result: [CS101, CS201, CS202, CS301]
No neighbors.

Queue: []

DONE! result.size()=4 == nodeCount()=4 → No cycle!
Topological order: CS101, CS201, CS202, CS301 ✓

Interpretation: Take CS101 first, then EITHER CS201 or CS202 (in any relative order), then CS301.
```

**Cycle detection trace**: If we had added CS301→CS101:
- CS101 would get in-degree 1, never reaching 0
- Queue would empty before processing all 4 nodes
- result.size()=3 < nodeCount()=4 → CYCLE DETECTED! Throw exception.

---

## Summary: What the Manual Traces Teach You

| Trace | Key Insight |
|---|---|
| BFS | FIFO ensures level-by-level = shortest hops |
| DFS | Goes deep immediately — visits C last because B is explored fully first |
| Dijkstra | Stale entries appear when a better path is found; skip them with `if (d > dist[u])` |
| A* | Heuristic guides exploration toward goal; B was never even processed! |
| Bellman-Ford | Every edge relaxed V-1 times; V-th round = cycle check |
| Kruskal | DSU efficiently detects "would this edge create a cycle?" in near O(1) |
| Tarjan | `low[u] == disc[u]` is the SCC root condition — understood by tracing disc/low values |
| Topo Sort | In-degree 0 = "all prerequisites satisfied" → safe to take this course |
