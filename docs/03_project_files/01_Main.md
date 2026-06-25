# main.cpp — The Starting Point of the Program
### A Beginner-Friendly Guide

> **Who is this for?**
> This guide assumes you know basic C++ (variables, loops, functions) but have never built a multi-file project before. Every new concept — `auto`, lambdas, `try/catch`, `filesystem` — is explained in plain English before the code is shown.

---

## What Is This File?

`src/main.cpp` is the **entry point** of the Graph Algorithm Engine — the file that runs when you type `./build/graph_engine.exe` in your terminal.

Think of it like the **table of contents** of a book. It doesn't contain the actual algorithms (those live in `include/algorithms/`), but it orchestrates everything:

1. Loads a real graph from a file on disk
2. Runs all 7 algorithms on it
3. Prints human-readable results to the terminal
4. Saves data files so the web visualizer can animate the results

A new developer reading this file top-to-bottom should understand how to use the entire engine.

---

## The Include Block — Gathering All the Tools

```cpp
#include <iostream>      // std::cout, std::cerr — printing to the terminal
#include <string>        // std::string — text type
#include <vector>        // std::vector — a resizable list
#include <iomanip>       // std::setw, std::fixed — formatted table output
#include <tuple>         // std::tuple — holds 3 values at once
#include <unordered_map> // A dictionary/hash table
#include <utility>       // std::pair — holds 2 values at once
#include <fstream>       // std::ofstream — writing to files
#include <filesystem>    // Creating folders
#include <stdexcept>     // std::exception — the base class for errors
#include <cmath>         // std::sqrt — square root
```

> **What is `#include`?**
> Each `#include` line pastes the contents of that library file into your program before compilation. Think of it like copying a recipe from a cookbook — you don't write the recipe yourself, you just reference it. The angle brackets `<...>` mean "look in the standard library." Quotes `"..."` mean "look in my own project."

Then the project-specific headers:
```cpp
#include "Graph.hpp"
#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/AStar.hpp"
#include "algorithms/BellmanFord.hpp"
#include "algorithms/Kruskal.hpp"
#include "algorithms/TarjanSCC.hpp"
#include "algorithms/TopologicalSort.hpp"
#include "utils/GraphLoader.hpp"
#include "utils/GraphExporter.hpp"
#include "utils/Timer.hpp"
```

Each of these makes one algorithm or tool available. Without the include, the compiler wouldn't know what `bfs()` or `dijkstra()` means.

---

## `int main()` — Where the Program Starts

```cpp
int main() {
    std::cout << "======================================================================\n";
    std::cout << "               GRAPH ALGORITHM ENGINE - DEMONSTRATION SUITE           \n";
    std::cout << "======================================================================\n\n";
```

> **`int main()` is mandatory in every C++ program.** The operating system calls this function when your program launches. The `int` return type means it gives back a number when done — `0` means success, anything else means failure.

> **`std::cout <<`** is how you print to the terminal. The `<<` operator sends data into the output stream, one piece at a time. `"\n"` is the newline character — it moves to the next line.

---

## Section 1 — Creating the Output Folder

```cpp
try {
    std::filesystem::create_directories("visualizer");
} catch (const std::exception& e) {
    std::cerr << "[Warning] Failed to create visualizer directory: " << e.what() << "\n";
}
```

This creates a folder called `visualizer/` where the program will save its output files. If the folder already exists, it does nothing (no error).

> **What is `try / catch`?**
> Some operations can fail — like creating a folder on a read-only disk, or opening a file that doesn't exist. C++ uses exceptions to signal these failures. The `try { }` block runs your code. If something goes wrong, it "throws" an exception, and execution jumps to the `catch { }` block.
>
> ```
> try {
>     // Risky operation here
> } catch (const std::exception& e) {
>     // Handle the failure here
>     // e.what() gives a human-readable error message
> }
> ```

Here we catch the error and print a warning but **don't stop the program**. The export steps later will warn if they can't write files, but the algorithm demonstrations can still run.

> **`std::cerr` vs `std::cout`:** Both print to the terminal, but `std::cerr` is reserved for warnings and errors. `std::cout` is for normal output. Tools and shell scripts can separate them.

---

## Section 2 — Creating the Graphs

```cpp
Graph<int, int> directedGraph(true);    // true = directed (edges go one way)
Graph<int, int> undirectedGraph(false); // false = undirected (edges go both ways)
```

We create **two separate graphs** from the same data file:
- `directedGraph` — used for BFS, DFS, Dijkstra, A*, Bellman-Ford, Tarjan
- `undirectedGraph` — used for Kruskal (MST only works on undirected graphs)

> **What does `Graph<int, int>` mean?**
> `Graph` is a template class — a blueprint that works for any types. The two `int`s inside `< >` tell it:
> - First `int` = node type (nodes are integers like 0, 1, 2, ...)
> - Second `int` = weight type (edge weights are also integers)
>
> You could use `Graph<std::string, double>` if your nodes are text labels and weights are decimal numbers.

### Loading the Data

```cpp
try {
    loadFromCSV("data/sample_graph.csv", directedGraph);
    loadFromCSV("data/sample_graph.csv", undirectedGraph);
} catch (const std::exception& e) {
    std::cerr << "  [Error] Failed to load graph: " << e.what() << "\n";
    return 1;  // Exit with error code 1 = failure
}
```

`loadFromCSV` reads the graph from a text file. If the file doesn't exist, it throws an exception.

> **Why `return 1` here but not in the folder creation?**
> When the folder creation fails, it's non-fatal — you can still run algorithms. But if the graph file fails to load, **there is nothing to work with**. The whole program should stop. `return 1` signals failure to the OS. `return 0` (at the end of main) signals success.

---

## Section 3 — Coordinates for the Visualizer

```cpp
std::unordered_map<int, std::pair<double, double>> coordMap;
coordMap[0] = {50.0, 300.0};
coordMap[1] = {150.0, 180.0};
// ... more nodes
```

> **What is `std::unordered_map`?**
> It's a **dictionary** (also called a hash map). It maps a key to a value, like a phone book maps a name to a phone number. Here we map each node ID (an `int`) to an (x, y) position (a `pair<double, double>`).

> **What is `std::pair<double, double>`?**
> A `pair` is a simple container that holds exactly **two values**. `{50.0, 300.0}` creates a pair using brace initialization — the compiler figures out the types automatically.

This coordinate map serves two purposes:
1. **Web visualizer** — The JSON export includes these positions so D3.js knows where to draw each node on screen
2. **A\* heuristic** — The straight-line distance between two nodes' positions is used as A*'s "estimated remaining distance" guide

---

## Section 4 — Export the Graph for the Visualizer

```cpp
exportToJSON(directedGraph, "visualizer/graph.json", coordMap);
```

One line writes the entire graph (nodes + edges + coordinates) to a JSON file. The D3.js visualizer reads this file and draws the graph in the browser.

---

## Section 5 — Running BFS

```cpp
std::cout << "[2] Running Breadth-First Search (BFS) Traversal...\n";
{
    Timer t("BFS");
    auto bfsOrder = bfs(directedGraph, 0);
    double elapsed = t.stop();
```

### The Curly Braces `{ }` Create a Scope Block

> A **scope block** is any code wrapped in `{ }`. Variables declared inside a scope are automatically destroyed when the closing `}` is reached. Here, `Timer t` is created at the start and automatically cleaned up at the end. This is the RAII pattern from `Timer.hpp` — the timer starts on construction and stops when its scope ends.

### `auto` — Let the Compiler Figure Out the Type

```cpp
auto bfsOrder = bfs(directedGraph, 0);
```

> **`auto`** tells the compiler "you figure out what type this is." Since `bfs()` returns `std::vector<int>`, `bfsOrder` automatically becomes `std::vector<int>`. This saves you from writing:
> ```cpp
> std::vector<int> bfsOrder = bfs(directedGraph, 0);
> ```
> Both are identical — `auto` is just shorter and easier to read.

### Printing the Path with the Ternary Operator

```cpp
for (size_t i = 0; i < bfsOrder.size(); ++i) {
    std::cout << bfsOrder[i] << (i + 1 < bfsOrder.size() ? " -> " : "");
}
```

> **The ternary operator `condition ? value_if_true : value_if_false`** is a compact if/else in one line. Here it prints `" -> "` between elements but NOT after the last one, so you get `0 -> 1 -> 2` instead of `0 -> 1 -> 2 ->`.

> **`size_t`** is an unsigned integer type used for sizes and indices. `.size()` returns `size_t`, so we use it for `i` too to avoid a compiler warning about comparing signed vs unsigned numbers.

---

## Section 6 — Running Dijkstra

```cpp
auto dijRes = dijkstra(directedGraph, 0);
auto path = reconstructPath(dijRes, 0, 11);
```

`dijkstra()` returns a `DijkstraResult` object containing:
- `dist` — a map from each node to its shortest distance from node 0
- `prev` — a map recording which node came before each node on the optimal path

`reconstructPath(dijRes, 0, 11)` uses the `prev` map to trace backward from node 11 to node 0, then reverses it to get the forward path `[0, ..., 11]`.

```cpp
if (path.empty()) {
    std::cout << "Unreachable";
} else {
    std::cout << "\n  Path Cost: " << dijRes.dist[11];
}
```

We check if the path is empty before using it. If node 11 is unreachable from node 0, `path` will be an empty vector and we print "Unreachable" instead of trying to access `dist[11]` (which would be infinity).

---

## Section 7 — Running A* with a Custom Heuristic

```cpp
auto customHeuristic = [&](int u, int v) {
    auto itU = coordMap.find(u);
    auto itV = coordMap.find(v);
    if (itU != coordMap.end() && itV != coordMap.end()) {
        double dx = itU->second.first - itV->second.first;
        double dy = itU->second.second - itV->second.second;
        return std::sqrt(dx * dx + dy * dy) * 0.01;
    }
    return 0.0;
};
```

> **What is a lambda?**
> A lambda is an anonymous function — a function you define on the spot and assign to a variable. Think of it as a recipe written on a sticky note rather than in a cookbook.
>
> The syntax `[&](int u, int v) { ... }` means:
> - `[&]` — capture all local variables by reference (so the lambda can read `coordMap` without copying it)
> - `(int u, int v)` — the function takes two node IDs as arguments
> - `{ ... }` — the function body

**What does this heuristic do?** It calculates the straight-line (Euclidean) distance between node `u` and node `v` using their visual coordinates from `coordMap`. This gives A\* an estimate of how far it still needs to travel.

> **`itU->second.first`** — Let's decode this:
> - `coordMap.find(u)` returns an **iterator** `itU` pointing to the key-value pair `{node_id: {x, y}}`
> - `itU->second` gets the **value** (the `pair<double,double>` of coordinates)
> - `.first` gets the x-coordinate; `.second` gets the y-coordinate
> - `itU != coordMap.end()` checks that the node was actually found (`.end()` is returned when not found)

**`* 0.01` scaling:** Raw pixel distances (e.g., 300 pixels) would be much larger than the actual edge weights (small integers like 4, 7). Multiplying by 0.01 scales the heuristic down so it never overestimates — a requirement for A* to guarantee the optimal path.

```cpp
auto astarRes = astar(directedGraph, 0, 11, customHeuristic);
std::cout << "  Nodes Visited (Closed): " << astarRes.nodesVisited;
```

The key comparison is `nodesVisited` — A\* explores far fewer nodes than Dijkstra to find the same shortest path.

---

## Section 8 — Running Bellman-Ford

```cpp
try {
    auto dist = bellmanFord(directedGraph, 0);
    // print the distance table
} catch (const std::exception& e) {
    std::cerr << "  [Error] Bellman-Ford threw exception: " << e.what() << "\n\n";
}
```

Bellman-Ford can detect **negative-weight cycles** (loops where repeatedly following edges makes the total distance smaller and smaller forever). When it detects one, it throws an exception.

We wrap it in `try/catch` so a negative cycle doesn't crash the entire demonstration. Instead, it prints the error and continues to the next algorithm.

---

## Section 9 — Running Kruskal MST

```cpp
auto mst = kruskal(undirectedGraph);  // Note: UNDIRECTED graph!

for (const auto& edge : mst) {
    auto [wt, u, v] = edge;  // Unpack the tuple into 3 variables
    std::cout << "    " << u << " -- " << v << " (weight: " << wt << ")\n";
    totalWeight += wt;
}
```

`kruskal()` returns a list of edges as `vector<tuple<int, int, int>>` — each edge is a bundle of three values: `(weight, source_node, destination_node)`.

> **`auto [wt, u, v] = edge`** is C++17 **structured binding** — it unpacks a tuple/pair/struct into named variables in one line. Without it, you'd have to write:
> ```cpp
> int wt = std::get<0>(edge);
> int u  = std::get<1>(edge);
> int v  = std::get<2>(edge);
> ```
> Much less readable!

### Writing the MST to a JSON File

```cpp
std::ofstream mstFile("visualizer/kruskal_mst.json");
if (mstFile.is_open()) {
    mstFile << "{\n  \"mst\": [\n";
    // ... write each edge
    mstFile << "  ]\n}\n";
}
```

> **`std::ofstream`** is an "output file stream" — it writes data to a file. `std::ofstream mstFile("visualizer/kruskal_mst.json")` opens (or creates) that file. The `if (mstFile.is_open())` check is important — if the visualizer folder doesn't exist, we skip the export instead of crashing.

---

## Section 10 — Running Tarjan SCC

```cpp
TarjanSCC<int, int> tarjan;
auto sccs = tarjan.findSCC(directedGraph);
```

> **Why is `TarjanSCC` a class instead of a plain function?**
> Most algorithms here are free functions (just `bfs(graph, start)`). Tarjan's algorithm uses internal bookkeeping variables (a stack, discovery times, low values) that persist during execution. Wrapping them in a class keeps the code organized and the variables private.

```cpp
for (size_t i = 0; i < sccs.size(); ++i) {
    std::cout << "    Component " << i + 1 << ": { ";
    for (const auto& node : sccs[i]) {
        std::cout << node << " ";
    }
    std::cout << "}\n";
}
```

Nested for loops: the outer loop goes over each Strongly Connected Component; the inner loop prints each node within that component. `i + 1` makes display 1-based (humans count from 1, not 0).

---

## Section 11 — Topological Sort with String Nodes

```cpp
Graph<std::string, int> dag(true);  // <-- nodes are strings!
dag.addEdge("CS101", "CS201", 1);
dag.addEdge("CS101", "CS202", 1);
dag.addEdge("CS201", "CS301", 1);
dag.addEdge("CS202", "CS301", 1);
```

> **The graph template is flexible.** Here we use `Graph<std::string, int>` — nodes are course names (strings) and weights are integers. This models course prerequisites: CS101 must be taken before CS201 and CS202; both must be done before CS301.
>
> The weight `1` is irrelevant here — topological sort only cares about graph structure, not weights.

```cpp
auto kahnOrder = topologicalSort(dag);    // BFS-based (Kahn's algorithm)
auto dfsOrder  = topologicalSortDFS(dag); // DFS-based (another approach)
```

Both produce a valid ordering where prerequisites always come before the courses that depend on them.

### Negative Test — Proving Cycle Detection Works

```cpp
Graph<std::string, int> cyclicDag(true);
cyclicDag.addEdge("A", "B", 1);
cyclicDag.addEdge("B", "C", 1);
cyclicDag.addEdge("C", "A", 1);  // Creates a cycle: A→B→C→A

try {
    topologicalSort(cyclicDag);  // Should throw!
    std::cout << "    [Warning] Topological sort succeeded on cyclic graph (Unexpected!)\n";
} catch (const std::runtime_error& e) {
    std::cout << "    Caught expected exception: \"" << e.what() << "\" (Success!)\n";
}
```

> **Negative testing** means intentionally triggering an error to verify your error-handling code works. We deliberately create a cycle and verify the algorithm catches it. If the `catch` block runs, the test passed!

---

## The Final Lines

```cpp
std::cout << "\n======================================================================\n";
std::cout << "                  ALL DEMONSTRATIONS RUN SUCCESSFULLY                 \n";
std::cout << "======================================================================\n";

return 0;
```

`return 0` signals success to the operating system. Shell scripts can check this:
```bash
./build/graph_engine.exe
if [ $? -ne 0 ]; then
    echo "Program failed!"
fi
```

---

## What Files Are Created After Running?

After running `./build/graph_engine.exe`, the `visualizer/` folder contains:

| File | What It Contains | Used For |
|---|---|---|
| `graph.json` | All nodes (with coordinates) + all edges (with weights) | Drawing the base graph |
| `bfs_steps.json` | Order BFS visited nodes: `[0, 1, 2, 3, ...]` | BFS step animation |
| `dfs_steps.json` | Order DFS visited nodes | DFS step animation |
| `dijkstra_path.json` | Shortest path from node 0 to node 11 | Highlighting the optimal path |
| `astar_path.json` | A* path from node 0 to node 11 | Comparing with Dijkstra |
| `kruskal_mst.json` | MST edges: `{source, target, weight}` | Highlighting MST edges |
| `tarjan_sccs.json` | List of SCCs, each as a list of node IDs | Coloring nodes by component |

---

## Key Concepts Summary

| Concept | Where Used | Plain-English Meaning |
|---|---|---|
| `#include` | Top of file | Paste another file's code into this one |
| `int main()` | Line ~35 | The mandatory starting function of every C++ program |
| `auto` | `auto bfsOrder = bfs(...)` | "Compiler, figure out the type for me" |
| `try / catch` | Around file loading | Run risky code; handle failure gracefully |
| `return 1` vs `return 0` | On CSV failure / end | Signal failure or success to the OS |
| `{ }` scope block | Around each algorithm | Group related code; auto-cleanup variables |
| Lambda `[&](...) {...}` | A* heuristic | An anonymous function defined on the spot |
| Structured binding `auto [a, b, c]` | Kruskal loop | Unpack a tuple into named variables |
| `std::ofstream` | Kruskal JSON export | Write data to a file |
| `std::filesystem` | Folder creation | Create directories without shell commands |
