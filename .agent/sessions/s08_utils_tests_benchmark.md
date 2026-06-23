## Session 08 — Utilities, Tests, Benchmarks, Visualizer

Before starting:
- Read `.agent/prompt.md` — sections for utils/, tests/, benchmarks/,
  visualizer/
- Read `.agent/spec.md` Step 8.1 for test requirements
- All 7 algorithm files from Sessions 03-07 are complete

Task:
Implement ALL remaining files:

Utilities:
  - include/utils/GraphLoader.hpp (CSV + adjacency list loader)
  - include/utils/GraphExporter.hpp (JSON exporter, D3.js compatible)
  - include/utils/Timer.hpp (high-resolution RAII timer)

Tests (Google Test):
  - tests/test_graph.cpp
  - tests/test_bfs_dfs.cpp
  - tests/test_dijkstra.cpp
  - tests/test_astar.cpp
  - tests/test_mst.cpp
  - tests/test_scc.cpp
  Each file must have: empty graph, single node, disconnected graph,
  known output verification, and performance test cases.

Benchmarks:
  - benchmarks/benchmark.cpp
  - Compare: Dijkstra vs A* (nodes visited + time)
  - Compare: BFS vs DFS (memory + time)
  - Test sizes: 100, 1000, 10000, 100000 nodes
  - Output as formatted table

Visualizer:
  - visualizer/index.html
  - visualizer/graph.js
  - visualizer/style.css
  D3.js force-directed layout, algorithm animation, shortest path highlight

Final src/main.cpp:
  - Demonstrate ALL algorithms on sample_graph.csv
  - Export results to JSON for visualizer
  - Show benchmark comparison table

Update README.md with complete build instructions and algorithm complexity table.
