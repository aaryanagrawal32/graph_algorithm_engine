## Session 06 — A* and Bellman-Ford

Before starting:
- Read `.agent/prompt.md` — sections for AStar.hpp and BellmanFord.hpp
- Read `.agent/spec.md` Steps 3.1 and 3.2 for concepts and acceptance criteria
- Dijkstra from Session 05 is complete

Task:
Implement BOTH `include/algorithms/AStar.hpp` AND `include/algorithms/BellmanFord.hpp`.

A* Requirements:
  - Heuristic as std::function<double(NodeType, NodeType)> parameter
  - astar(Graph&, src, dst, heuristic) → vector<NodeType>
  - Built-in heuristics: manhattanHeuristic, euclideanHeuristic, zeroHeuristic
  - Track nodesVisited count alongside path
  - WHY comments on f(n)=g(n)+h(n), admissibility, why Manhattan is admissible

Bellman-Ford Requirements:
  - bellmanFord(Graph&, src) → unordered_map<NodeType, WeightType>
  - Detect negative cycles → throw std::runtime_error
  - V-1 relaxation passes + V-th iteration check
  - WHY comment: why V-1 iterations, why V-th detects cycle

Update src/main.cpp to compare A* vs Dijkstra nodes visited count.

Do not implement Kruskal yet.
