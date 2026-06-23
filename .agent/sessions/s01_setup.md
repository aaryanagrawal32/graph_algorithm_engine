## Session 01 — Project Setup

Before starting:
- Read `.agent/prompt.md` fully for the complete project specification
- Read `.agent/spec.md` Step 1.1 for acceptance criteria and context

Task:
Create the complete folder structure for the Graph Algorithm Engine.
Generate ONLY these files:
  - CMakeLists.txt (root)
  - README.md (placeholder with project title only)
  - .gitignore
  - Empty placeholder .hpp files in include/ (no implementation yet)
  - src/main.cpp (empty main returning 0)
  - data/sample_graph.csv (with the sample data specified in prompt)

Verify: `cmake -B build && cmake --build build` succeeds with zero errors.

Stop after these files. Do not implement Graph.hpp or any algorithm yet.
