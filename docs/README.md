# Graph Algorithm Engine - Master Documentation Guide

Welcome. This folder contains **line-by-line explanations** for every file in the Graph Algorithm Engine project. This guide tells you what to read, in what order, and what to look for.

---

## Who Is This For?

| Your Background | Start Here |
|---|---|
| New to C++ — know Python or Java | Read `01_foundations/03_CPP_Syntax_Reference.md` first, then follow the Reading Path below |
| Know C++ basics, new to graph algorithms | Jump straight to the Reading Path |
| Preparing for a coding interview | Go directly to `04_reference/Interview_QandA.md` + `04_reference/Algorithm_Cheat_Sheet.md` |
| Debugging the project | Go to the specific file's explanation |
| Just want a quick overview | Read `04_reference/Algorithm_Cheat_Sheet.md` (2 pages, covers everything) |

---

## The Big Picture — How All Files Connect

```
┌─────────────────────────────────────────────────────────┐
│                     Your Code Entry Points               │
│                                                         │
│   src/main.cpp          benchmarks/benchmark.cpp        │
│   (demo program)        (performance tests)             │
└──────────┬──────────────────────────┬───────────────────┘
           │ #includes                │ #includes
           ▼                          ▼
 ┌─────────────────────────────────────────────────────────┐
 │                  Algorithm Headers (include/algorithms/) │
 │                                                         │
 │  BFS.hpp    DFS.hpp    Dijkstra.hpp    AStar.hpp        │
 │  BellmanFord.hpp    Kruskal.hpp    TarjanSCC.hpp        │
 │  TopologicalSort.hpp                                    │
 └──────────┬──────────────────────────────────────────────┘
            │ all #include
            ▼
 ┌─────────────────────────────────────────────────────────┐
 │                     include/Graph.hpp                   │
 │           (the Graph<NodeType, WeightType> class)       │
 │         ALL other files depend on this one              │
 └─────────────────────────────────────────────────────────┘
            │
            │ also used by
            ▼
 ┌─────────────────────────────────────────────────────────┐
 │                  Utility Headers (include/utils/)        │
 │                                                         │
 │  GraphLoader.hpp    GraphExporter.hpp    Timer.hpp      │
 └─────────────────────────────────────────────────────────┘
            │ data flows to
            ▼
 ┌─────────────────────────────────────────────────────────┐
 │              Tests (tests/) & Data (data/)              │
 │                                                         │
 │  test_graph.cpp   test_bfs_dfs.cpp   test_dijkstra.cpp │
 │  test_astar.cpp   test_mst.cpp       test_scc.cpp      │
 │  data/sample_graph.csv   road_network_sample.csv       │
 └─────────────────────────────────────────────────────────┘
```

**Rule of thumb**: If file A `#includes` file B, understand B before A.

---

## Recommended Reading Path

Follow this order. Each step builds on the previous one.

### 🟢 Phase 1 — C++ Foundations (Before Any Algorithm)

**Read this if you're new to C++ or need a refresher.**

| Step | File | What You'll Learn |
|---|---|---|
| 1 | `01_foundations/03_CPP_Syntax_Reference.md` | Every C++ syntax feature used in this project, explained in plain English |
| 2 | `01_foundations/01_Graph_Class.md` | The core `Graph<NodeType, WeightType>` class — this is the foundation everything else builds on |
| 3 | `01_foundations/02_Graph_Advanced_Concepts.md` | Templates, hash maps, iterators — the C++ machinery behind the Graph class |

**Checkpoint**: After step 3, you should understand what `Graph<int, double> g(true)` means and be able to call `g.addEdge(1, 2, 4.5)` and `g.neighbors(1)`.

---

### 🔵 Phase 2 — Basic Traversal Algorithms

| Step | File | What You'll Learn |
|---|---|---|
| 4 | `02_algorithms/01_BFS_DFS.md` | BFS (queue, level-order, shortest unweighted path) and DFS (stack/recursion, cycle detection) |

**Checkpoint**: You should be able to trace BFS and DFS by hand on a small graph (5-6 nodes) and predict the visit order.

---

### 🟡 Phase 3 — Shortest Path Algorithms

| Step | File | What You'll Learn |
|---|---|---|
| 5 | `02_algorithms/02_Dijkstra.md` | Priority queue, min-heap, stale entries, edge relaxation — the most important single algorithm here |
| 6 | `02_algorithms/03_AStar.md` | Heuristics, f(n)=g(n)+h(n), admissibility — Dijkstra with a guide |
| 7 | `02_algorithms/04_BellmanFord.md` | Negative weights, V-1 iterations, negative cycle detection |

**Checkpoint**: Given the graph `A--4--B--1--C--5--D` and `A--2--C`, you should be able to trace Dijkstra's algorithm step-by-step and know why A* would finish faster.

---

### 🟠 Phase 4 — Advanced Graph Algorithms

| Step | File | What You'll Learn |
|---|---|---|
| 8 | `02_algorithms/05_Kruskal_MST.md` | Disjoint Set Union (Union-Find), path compression, Minimum Spanning Trees |
| 9 | `02_algorithms/06_TarjanSCC.md` | The hardest algorithm — disc/low arrays, SCC root condition, stack-based extraction |
| 10 | `02_algorithms/07_TopologicalSort.md` | Kahn's BFS-based approach, DFS post-order, cycle detection |

---

### 🔴 Phase 5 — Infrastructure & Project Structure

| Step | File | What You'll Learn |
|---|---|---|
| 11 | `03_project_files/04_Utils.md` | RAII timers, CSV file parsing, JSON export |
| 12 | `03_project_files/01_Main.md` | How the demo program ties all algorithms together |
| 13 | `03_project_files/02_Benchmark.md` | Performance measurement, random graph generation, table formatting |
| 14 | `03_project_files/03_Tests.md` | Google Test macros, how unit tests verify correctness |
| 15 | `03_project_files/05_CMake_And_Data.md` | CMake build system, FetchContent, data file formats |

---

### 🏁 Phase 6 — Review & Interview Prep

| Step | File | What You'll Learn |
|---|---|---|
| 16 | `04_reference/Algorithm_Cheat_Sheet.md` | Side-by-side comparison of all 7 algorithms — great for review |
| 17 | `04_reference/Visual_Walkthroughs.md` | ASCII-art step-by-step traces — use these to verify your mental model |
| 18 | `04_reference/Interview_QandA.md` | 40+ interview questions with full answers — use this to practice |

---

## Quick Navigation — "Where Is X Explained?"

| Topic | File |
|---|---|
| `template<typename T>` | `01_foundations/03_CPP_Syntax_Reference.md` → Templates section |
| `#pragma once` | `01_foundations/03_CPP_Syntax_Reference.md` → Header Guards section |
| `std::unordered_map` | `01_foundations/02_Graph_Advanced_Concepts.md` |
| `std::priority_queue` with `greater<>` | `02_algorithms/02_Dijkstra.md` → Part 0 |
| Why BFS finds shortest path | `02_algorithms/01_BFS_DFS.md` → BFS section |
| Stale entry optimization | `02_algorithms/02_Dijkstra.md` → Part 2 (manual trace) |
| `f(n) = g(n) + h(n)` formula | `02_algorithms/03_AStar.md` |
| What "admissible heuristic" means | `02_algorithms/03_AStar.md` |
| Why V-1 iterations in Bellman-Ford | `02_algorithms/04_BellmanFord.md` |
| Path compression in DSU | `02_algorithms/05_Kruskal_MST.md` |
| `disc[]` and `low[]` in Tarjan's | `02_algorithms/06_TarjanSCC.md` |
| `low[u] == disc[u]` SCC root condition | `02_algorithms/06_TarjanSCC.md` |
| Kahn's algorithm | `02_algorithms/07_TopologicalSort.md` |
| RAII timer | `03_project_files/04_Utils.md` → Timer section |
| CSV parsing with `stringstream` | `03_project_files/04_Utils.md` → Loader section |
| `EXPECT_EQ` vs `ASSERT_EQ` | `03_project_files/03_Tests.md` |
| `FetchContent` for GoogleTest | `03_project_files/03_Tests.md` → CMakeLists section |
| `std::mt19937` random numbers | `03_project_files/02_Benchmark.md` |
| Structured binding `auto [a, b]` | `03_project_files/01_Main.md` |
| `try/catch` exceptions | `03_project_files/01_Main.md` |
| `add_executable` in CMake | `03_project_files/05_CMake_And_Data.md` |

---

## How Each Theory File Is Structured

Every explanation file follows this pattern:

```
1. WHAT IS THIS FILE? — What problem does this file solve?
2. BACKGROUND CONCEPTS — Any theory or C++ concept you need first
3. LINE-BY-LINE WALKTHROUGH — Each code block explained
4. WORKED EXAMPLE — A manual trace / diagram showing it running
5. KEY TAKEAWAYS — Summary table of the important ideas
```

If you get confused reading a theory file, look at the **Worked Example** section first — seeing the algorithm execute step-by-step often makes the code click.

---

## Common "I'm Stuck" Situations

### "I don't understand what `template<typename NodeType>` means"
→ Read `01_foundations/03_CPP_Syntax_Reference.md` → **Templates** section first

### "I understand the algorithm conceptually but can't follow the C++ code"
→ Read `01_foundations/03_CPP_Syntax_Reference.md` end-to-end. It covers every C++ construct used in this project.

### "The algorithm makes sense but I don't know where it's actually USED"
→ Read `03_project_files/01_Main.md` — it shows how every algorithm is called in the demo program

### "I'm getting a compiler error I don't understand"
→ Most errors come from template type mismatches. The `01_foundations/03_CPP_Syntax_Reference.md` has a **Common Compiler Errors** section

### "How does the visualizer work?"
→ The visualizer is a self-contained web app. `main.cpp` exports JSON files, and `visualizer/graph.js` reads them. `03_project_files/01_Main.md` explains the export step; the visualizer JS is self-documented in `visualizer/graph.js`.

---

## Estimated Reading Times

| Phase | Files | Time |
|---|---|---|
| Phase 1 — C++ Foundations | 3 files | 1–2 hours |
| Phase 2 — Basic Traversal | 1 file | 45 minutes |
| Phase 3 — Shortest Path | 3 files | 2–3 hours |
| Phase 4 — Advanced Algorithms | 3 files | 2–3 hours |
| Phase 5 — Infrastructure | 5 files | 1–2 hours |
| Phase 6 — Review & Interview Prep | 3 files | 1–2 hours |
| **Total** | **18 files** | **8–12 hours** |

Don't try to read everything in one sitting. The recommended approach:
- **Day 1**: Phase 1 + Phase 2 (Graph class + BFS/DFS)
- **Day 2**: Phase 3 (Dijkstra + A* + Bellman-Ford)
- **Day 3**: Phase 4 (Kruskal + Tarjan + Topo Sort)
- **Day 4**: Phase 5 + Phase 6 (Infrastructure + Interview Prep)
