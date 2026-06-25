# CMakeLists.txt & Data Files — How the Project Gets Built
### A Beginner-Friendly Guide

> **Who is this for?**
> This guide is for someone who has written C++ code before but has never seen a build system. We explain from scratch: what CMake is, why it exists, and what every line of both CMake files does.

---

## Part 1: What Is a Build System? Why Do We Need CMake?

Writing code is just the first step. Before you can run a C++ program, you need to **compile** it — translate the human-readable source code into machine instructions the CPU can execute.

For a single file, compiling is easy:
```bash
g++ main.cpp -o main.exe
```

But this project has **20+ files** spread across `src/`, `include/`, `benchmarks/`, and `tests/`. Compiling manually would look like:
```bash
g++ src/main.cpp include/Graph.hpp include/algorithms/BFS.hpp ... -o graph_engine.exe
```

...and you'd need to run different commands for the tests, benchmarks, etc. Get any flag wrong and you get cryptic errors.

**CMake solves this.** You write `CMakeLists.txt` once — a recipe that describes *what to build and how*. CMake reads it and generates the actual build commands for your system.

```
You write: CMakeLists.txt
              ↓
CMake reads it and generates: Makefiles (on Linux/Mac) or .sln (on Windows)
              ↓
make / cmake --build runs the actual compiler on every file
```

The big advantage: **the same `CMakeLists.txt` works everywhere** — on Windows, Linux, and macOS, with different compilers (GCC, Clang, MSVC). You don't write different build scripts for different machines.

---
---

# File 1: Root `CMakeLists.txt`

This is the main build recipe for the entire project.

---

## Line 1: Minimum Version

```cmake
cmake_minimum_required(VERSION 3.15)
```

> **What this means:** If someone has an older version of CMake (like 3.10), they get a clear error message: "This project requires CMake 3.15 or newer" instead of a confusing failure later.

Why 3.15? Because `FetchContent_MakeAvailable` (used in the test configuration) was added in CMake 3.14. We use 3.15 as a safe minimum.

---

## Line 2: Project Name and Version

```cmake
project(GraphEngine VERSION 1.0)
```

> **What this does:** Declares the project. CMake automatically creates variables you can use elsewhere:
> - `${PROJECT_NAME}` = `"GraphEngine"`
> - `${PROJECT_VERSION}` = `"1.0"`
> - `${PROJECT_VERSION_MAJOR}` = `1`
> - `${PROJECT_VERSION_MINOR}` = `0`

These variables can be embedded in documentation or used to auto-generate version headers.

---

## Lines 3-4: C++ Version

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
```

> **`set(VARIABLE value)`** assigns a value to a CMake variable.

`CMAKE_CXX_STANDARD 17` — compile all source files with the C++17 standard (adds the `-std=c++17` flag to every compiler call).

`CMAKE_CXX_STANDARD_REQUIRED True` — if the compiler doesn't support C++17, **stop immediately with an error** instead of silently falling back to C++14 or C++11. Our code uses C++17 features:
- Structured bindings (`auto [a, b] = pair`)
- `std::filesystem` (creating directories)
- `if constexpr` (compile-time conditionals)

These don't exist in C++14. Falling back silently would cause confusing compile errors.

---

## Line 5: Compiler Warning Flags

```cmake
add_compile_options(-Wall -Wextra -O2)
```

> **`add_compile_options`** adds flags that are passed to the compiler for every source file.

| Flag | What It Does |
|---|---|
| `-Wall` | Enable all standard warnings (unused variables, missing return statements, etc.) |
| `-Wextra` | Enable additional warnings beyond `-Wall` |
| `-O2` | Optimization level 2 — compiler reorganizes code, inlines functions, unrolls loops |

> **Why enforce warnings?** Warnings are the compiler telling you about likely bugs. By treating warnings seriously (and keeping zero warnings), any new warning after a code change stands out immediately as something to investigate.

> **Why `-O2` and not `-O3`?** `-O3` enables more aggressive optimizations that can occasionally produce subtly different results for floating-point arithmetic. `-O2` is the standard safe production optimization level.

> **Important:** These flags use the Linux/GCC syntax. On Windows with MSVC, the equivalent flags are `/W4` and `/O2`. CMake handles translating when you choose a different generator, but these flags work for MinGW (the GCC port for Windows).

---

## Line 6: Finding SFML

```cmake
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
```

> **`find_package`** searches your system for an installed library. CMake checks standard installation locations (like `C:\SFML\` on Windows or `/usr/lib/` on Linux).

Breaking down the arguments:
- `SFML` — the library name
- `2.5` — minimum version required
- `COMPONENTS graphics window system` — we need these three parts of SFML (not `audio` or `network`)
- `REQUIRED` — if SFML is not found, stop with an error immediately

> **What is SFML?** SFML (Simple and Fast Multimedia Library) is a C++ library for graphics, windows, and audio. This project uses it for rendering visualizations.

When SFML is found, CMake creates link targets like `sfml-graphics`, `sfml-window`, `sfml-system` that we can use in the next step.

---

## Lines 7-9: The Main Program

```cmake
add_executable(graph_engine src/main.cpp)
target_include_directories(graph_engine PRIVATE include)
target_link_libraries(graph_engine sfml-graphics sfml-window sfml-system)
```

**`add_executable(graph_engine src/main.cpp)`**
> Creates a build target called `graph_engine` compiled from `src/main.cpp`. After building, you'll have `build/graph_engine.exe`.

**`target_include_directories(graph_engine PRIVATE include)`**
> When compiling `graph_engine`, look in the `include/` folder to resolve `#include "Graph.hpp"`, `#include "algorithms/BFS.hpp"`, etc.
>
> The `PRIVATE` keyword means this include path applies only to `graph_engine` itself. If another target depended on `graph_engine`, it would NOT automatically get this include path. (The opposite keywords are `PUBLIC` and `INTERFACE`.)

**`target_link_libraries(graph_engine sfml-graphics sfml-window sfml-system)`**
> Links the SFML libraries into the executable. "Linking" means connecting the compiled code from SFML to our program so the SFML functions are available at runtime.

---

## Lines 10-11: Enabling Tests

```cmake
enable_testing()
add_subdirectory(tests)
```

**`enable_testing()`**
> Activates CTest — CMake's test runner. After this, you can run `ctest` from the build directory to execute all registered tests.

**`add_subdirectory(tests)`**
> Tells CMake: "also process the `tests/CMakeLists.txt` file." The tests have their own configuration file that handles downloading Google Test and building 6 test executables.

---

## Lines 12-13: The Benchmark Program

```cmake
add_executable(benchmark benchmarks/benchmark.cpp)
target_include_directories(benchmark PRIVATE include)
```

A second executable target. Notice: **no SFML linking**. Benchmarks only run algorithms — they don't display any graphics. So we don't need to link SFML here.

After building: `build/benchmark.exe`

---
---

# File 2: `tests/CMakeLists.txt`

This is the build recipe specifically for the test suite.

---

## Step 1: Load the FetchContent Module

```cmake
include(FetchContent)
```

> **`include(ModuleName)`** loads a built-in CMake module. `FetchContent` is a module that lets you declare external dependencies and have CMake download and compile them automatically during the configure step (when you run `cmake -B build`).

This is the modern CMake way to manage dependencies. Before FetchContent, you had to manually install libraries on your system.

---

## Step 2: Declare the Dependency

```cmake
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
```

> **`FetchContent_Declare`** registers a dependency but does NOT download it yet. It just records where to find it.

Arguments:
- `googletest` — the name we'll use to refer to this dependency in later commands
- `GIT_REPOSITORY` — the GitHub URL to clone from
- `GIT_TAG v1.14.0` — checkout this specific version tag

> **Why pin to `v1.14.0` instead of `main`?**
> If we used `GIT_TAG main`, a future commit by the Google Test team could change the API and break our tests. Pinning to a specific version tag ensures that everyone who clones this repository at any future point gets the exact same Google Test code.

---

## Step 3: Windows Compatibility Fix

```cmake
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
```

> This is a Windows-specific workaround. On Windows with MinGW, all compiled code must link against the same C Runtime Library (CRT). If Google Test uses a different CRT than our project code, the linker fails with obscure errors.
>
> This setting forces Google Test to use the same CRT as us.

Breaking down the syntax:
- `gtest_force_shared_crt ON` — set this Google Test option to ON
- `CACHE BOOL "" FORCE` — store it in CMake's cache and override any existing value (so even if someone previously set it to OFF, we force it ON)

---

## Step 4: Actually Download and Build

```cmake
FetchContent_MakeAvailable(googletest)
```

This is the line that **triggers the actual download and compilation**. After this:
- Google Test source code is in `build/_deps/googletest-src/`
- Compiled libraries are in `build/_deps/googletest-build/`
- The CMake targets `GTest::gtest` and `GTest::gtest_main` are available to use

The first time you run `cmake -B build`, this downloads from GitHub (needs internet). After that, it uses the cached download.

---

## Step 5: Build All 6 Test Files with a Loop

```cmake
enable_testing()

foreach(test_file
    test_graph
    test_bfs_dfs
    test_dijkstra
    test_astar
    test_mst
    test_scc
)
    add_executable(${test_file} ${test_file}.cpp)
    target_include_directories(${test_file} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_file} GTest::gtest_main)
    add_test(NAME ${test_file} COMMAND ${test_file})
endforeach()
```

> **`foreach(variable item1 item2 ...)`** loops over a list. On each iteration, `${variable}` expands to the current item. So `${test_file}` is `test_graph` on the first pass, `test_bfs_dfs` on the second, etc.

For each test file, four things happen in sequence:

**1. `add_executable(${test_file} ${test_file}.cpp)`**
Creates a separate compiled program. On the `test_graph` iteration, this becomes `add_executable(test_graph test_graph.cpp)` → produces `build/tests/test_graph.exe`.

**2. `target_include_directories(... PRIVATE ${CMAKE_SOURCE_DIR}/include)`**
> **`${CMAKE_SOURCE_DIR}`** is a special CMake variable that always points to the **root of your project** (where the top-level `CMakeLists.txt` is). So this adds `<project_root>/include` to the include path. Without this, `#include "Graph.hpp"` in the test files would fail — the compiler wouldn't know where to look.

**3. `target_link_libraries(... GTest::gtest_main)`**
Links Google Test into the executable. `GTest::gtest_main` specifically provides a pre-written `main()` function that discovers and runs all tests. Without this, each test file would need to write:
```cpp
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```
`GTest::gtest_main` does this automatically.

**4. `add_test(NAME ${test_file} COMMAND ${test_file})`**
Registers this binary with CTest. After this, `ctest` knows about it and can run it. Without `add_test`, `ctest` would not discover the executable.

---

## How to Run Everything

After configuring and building:

```bash
# Step 1: Configure (generates build system, downloads GoogleTest)
cmake -G "MinGW Makefiles" -B build

# Step 2: Build (compiles all executables)
cmake --build build

# Step 3: Run the main demo
./build/graph_engine.exe

# Step 4: Run all tests
cd build
ctest --output-on-failure

# Step 5: Run benchmarks
./build/benchmark.exe
```

`ctest --output-on-failure` runs every registered test and shows detailed output only for tests that fail, keeping the successful output clean.

---
---

# Part 3: The Data Files

## `data/sample_graph.csv`

```
# source,destination,weight   ← comment lines start with #, ignored by the loader
0,1,4                          ← edge from node 0 to node 1, weight 4
0,2,2
1,3,5
2,3,1
...
```

### How the Loader Reads This

`loadFromCSV()` in `GraphLoader.hpp`:
1. Opens the file
2. Reads line by line
3. Skips lines starting with `#` (comments)
4. Splits each remaining line on commas to get source, destination, weight
5. Calls `g.addEdge(source, destination, weight)` for each line

### Why This Specific Graph?

The graph is carefully designed so every algorithm has something interesting to demonstrate:

| Feature | Nodes/Edges Involved | Which Algorithm Benefits |
|---|---|---|
| Multiple directed cycles | Nodes that form loops | Tarjan's SCC (finds all cycles) |
| A negative-weight edge | e.g., weight = -3 | Bellman-Ford (Dijkstra fails with negatives!) |
| Clear shortest path | Nodes 0 through 11 | A\* vs Dijkstra comparison |
| Multiple components | Some nodes unreachable from 0 | BFS/DFS reachability testing |

### The CSV Format Rules

- Each line: `source,destination,weight`
- Lines starting with `#` are comments — ignored
- No spaces around the commas
- Weights can be negative (only Bellman-Ford handles this correctly)

---

## `data/road_network_sample.csv`

```
[Nodes]
# node_id,latitude,longitude
101,22.3190,87.3091
102,22.3250,87.3110

[Edges]
# source_id,destination_id,distance_meters
101,102,95
102,103,120
```

### A Different Format — Two Sections

This file uses `[Nodes]` and `[Edges]` section headers, read by `loadFromRoadNetworkCSV()`. The loader uses **boolean flags** to track which section it's currently in:

```
When it reads "[Nodes]" → set parsingNodes=true, parsingEdges=false
When it reads "[Edges]" → set parsingNodes=false, parsingEdges=true
Every other line → parse according to which flag is true
```

### Why Do Nodes Need Coordinates?

The A\* algorithm needs a **heuristic** — an estimate of the remaining distance to the destination. For real road networks:
- Nodes = intersections (with geographic position)
- Edges = road segments (with measured distances)
- Heuristic = straight-line geographic distance between two intersections

The straight-line distance is always ≤ the actual road distance (you can't drive shorter than a straight line). This property — never overestimating — is called **admissibility** and is required for A\* to guarantee the optimal path.

In practice you'd use this file like:
```cpp
std::unordered_map<int, std::pair<double, double>> coordMap;
Graph<int, int> g(true);
loadFromRoadNetworkCSV("data/road_network_sample.csv", g, coordMap);

auto heuristic = [&](int u, int v) {
    // Compute geographic distance between coordMap[u] and coordMap[v]
    // using the Haversine formula
};

auto result = astar(g, 101, 110, heuristic);
```

The node IDs (`101`, `102`, ...) are real road intersection IDs from the IIT Kharagpur campus area.

---

## `.gitignore`

```
# Build output
build/
*.exe
*.o

# CMake generated
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# IDE files
.vscode/settings.json
```

> **What is `.gitignore`?** Git tracks changes to your files. `.gitignore` is a list of patterns telling Git "don't track these files." Files matching these patterns are excluded from `git add`, `git status`, and `git commit`.

**Why ignore these specific things?**

| Pattern | Why Ignore |
|---|---|
| `build/` | Compiled binaries are machine-specific — they can't be shared across OS/compiler combinations, and can always be regenerated with `cmake --build` |
| `*.exe`, `*.o` | Individual compiled files — same reasoning |
| `CMakeCache.txt` | Stores machine-specific config (where YOUR SFML is installed). Committing it would break everyone else's build |
| `CMakeFiles/` | CMake's internal work folder — generated automatically, not source code |
| `.vscode/settings.json` | Your personal editor preferences (tab width, font size) shouldn't affect teammates |

> **A good `.gitignore` means `git status` shows only the files you actually wrote**, not compiler noise. This makes code review easier and keeps the repository small.

---

## Summary: The Whole Build System at a Glance

```
graph_algorithm_engine/
├── CMakeLists.txt          ← Root recipe: main + benchmark executables
│
├── tests/
│   └── CMakeLists.txt      ← Test recipe: downloads GoogleTest, builds 6 test programs
│
├── src/main.cpp            → compiled into:  build/graph_engine.exe
├── benchmarks/benchmark.cpp → compiled into: build/benchmark.exe
└── tests/test_*.cpp        → compiled into:  build/tests/test_*.exe (6 programs)
```

**How it all connects:**
1. Root `CMakeLists.txt` builds the main program and calls `add_subdirectory(tests)`
2. `tests/CMakeLists.txt` downloads GoogleTest and builds 6 test programs
3. `ctest` runs all 6 test programs and reports pass/fail
4. `./build/graph_engine.exe` runs the main demo
5. `./build/benchmark.exe` runs the performance benchmarks

---

## Key CMake Concepts Summary

| CMake Command | What It Does |
|---|---|
| `cmake_minimum_required(VERSION X)` | Enforce minimum CMake version |
| `project(Name VERSION X)` | Name the project, create version variables |
| `set(VAR value)` | Assign a value to a CMake variable |
| `add_compile_options(flags)` | Add compiler flags for all files |
| `find_package(Lib REQUIRED)` | Search for an installed library |
| `add_executable(name file.cpp)` | Create a compiled program target |
| `target_include_directories(target PRIVATE dir)` | Tell compiler where to find header files |
| `target_link_libraries(target lib)` | Link an external library into the program |
| `enable_testing()` | Activate CTest support |
| `add_subdirectory(dir)` | Also process CMakeLists.txt in that directory |
| `include(Module)` | Load a built-in CMake module |
| `FetchContent_Declare(name ...)` | Register a downloadable dependency |
| `FetchContent_MakeAvailable(name)` | Actually download and build the dependency |
| `foreach(var list)` / `endforeach()` | Loop over a list |
| `add_test(NAME x COMMAND y)` | Register an executable as a CTest test |
