## Session 05 — Dijkstra's Algorithm

Before starting:
- Read `.agent/prompt.md` — section "include/algorithms/Dijkstra.hpp"
- Read `.agent/spec.md` Step 2.2 for the manual trace and acceptance criteria
- BFS and DFS from Sessions 03-04 are complete

Task:
Implement ONLY `include/algorithms/Dijkstra.hpp`.

Requirements:
  - DijkstraResult struct: dist map + prev map
  - dijkstra(Graph&, NodeType src) → DijkstraResult
  - reconstructPath(DijkstraResult&, src, dst) → vector<NodeType>
  - priority_queue with greater<> comparator (min-heap)
  - Stale entry optimization: if (d > dist[u]) continue
  - WHY comments on: greater<> vs less<>, stale entries, why fails with
    negative weights, time complexity O(E log V) derivation

Update src/main.cpp to show Dijkstra on weighted graph with path reconstruction.

Do not implement A* yet.