## Session 04 — DFS Implementation

Before starting:
- Read `.agent/prompt.md` — section "include/algorithms/DFS.hpp"
- Read `.agent/spec.md` Step 1.4 for acceptance criteria
- BFS from Session 03 is complete

Task:
Implement ONLY `include/algorithms/DFS.hpp`.

Requirements:
  - Recursive DFS: dfs(Graph&, NodeType start) → vector<NodeType>
  - Iterative DFS: dfsIterative(Graph&, NodeType start) → vector<NodeType>
  - Cycle detection: hasCycle(Graph&) → bool
  - Connectivity check: isConnected(Graph&) → bool
  - WHY comment explaining why recursive and iterative may give different order
  - WHY comment explaining how back edges detect cycles

Update src/main.cpp to demonstrate both versions and show order difference.

Do not implement Dijkstra yet.