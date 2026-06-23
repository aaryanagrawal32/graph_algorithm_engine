## Session 02 — Graph Class

Before starting:
- Read `.agent/prompt.md` — section "include/Graph.hpp"
- Read `.agent/spec.md` Step 1.2 for concepts and acceptance criteria
- Session 01 is complete and compiles cleanly

Task:
Implement ONLY `include/Graph.hpp` exactly as specified in prompt.md.

Requirements to satisfy from spec Step 1.2:
  - Template parameters: NodeType, WeightType = int
  - Inner Edge struct with to + weight fields
  - unordered_map adjacency list as core data structure
  - All methods listed in prompt: addNode, addEdge, removeEdge,
    hasNode, hasEdge, neighbors, nodeCount, edgeCount, getAllNodes
  - begin()/end() iterators for range-based for loops
  - Every method has WHY comment explaining time complexity

Also update src/main.cpp to demonstrate:
  - Creating directed and undirected graphs
  - Adding edges with different weight types
  - Printing all neighbors of a node

Stop after Graph.hpp and main.cpp update. Do not implement BFS or DFS yet.
