## Session 07 — Kruskal, Tarjan SCC, Topological Sort

Before starting:
- Read `.agent/prompt.md` — sections for Kruskal.hpp, TarjanSCC.hpp,
  TopologicalSort.hpp
- Read `.agent/spec.md` Steps 4.1, 4.2, 5.1, 5.2 for acceptance criteria
- All shortest path algorithms from Sessions 05-06 are complete

Task:
Implement ALL THREE remaining algorithm files:
  - `include/algorithms/Kruskal.hpp`
  - `include/algorithms/TarjanSCC.hpp`
  - `include/algorithms/TopologicalSort.hpp`

Kruskal Requirements:
  - DSU struct with path compression + union by rank
  - kruskal(Graph&) → vector<tuple<WeightType, NodeType, NodeType>>
  - WHY comments on path compression, union by rank, cut property

Tarjan SCC Requirements:
  - Class TarjanSCC<NodeType> with findSCC(Graph&) → vector<vector<NodeType>>
  - Private: disc, low, onStack, stack, timer
  - WHY comments on disc[] vs low[], SCC root condition, O(V+E) proof

Topological Sort Requirements:
  - Kahn's BFS-based: topologicalSort(Graph&) → vector<NodeType>
  - DFS-based: topologicalSortDFS(Graph&) → vector<NodeType>
  - Both detect cycles and throw for non-DAG input

Update src/main.cpp to demonstrate all three on appropriate test graphs.
