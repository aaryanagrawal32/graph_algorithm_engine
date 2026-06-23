## Session 03 — BFS Implementation

Before starting:
- Read `.agent/prompt.md` — section "include/algorithms/BFS.hpp"
- Read `.agent/spec.md` Step 1.3 for the manual trace and acceptance criteria
- Graph.hpp from Session 02 is complete and tested

Task:
Implement ONLY `include/algorithms/BFS.hpp`.

Requirements:
  - Free function template: bfs(Graph&, NodeType start) → vector<NodeType>
  - Free function template: bfsShortestPath(Graph&, src, dst) → vector<NodeType>
  - Uses queue<NodeType> and unordered_set<NodeType>
  - Returns empty vector for unreachable destination
  - WHY comments on: queue vs stack choice, marking visited on push not pop,
    why BFS finds shortest path in unweighted graphs only

Also update src/main.cpp to demonstrate BFS on the sample graph.

Do not implement DFS yet.
