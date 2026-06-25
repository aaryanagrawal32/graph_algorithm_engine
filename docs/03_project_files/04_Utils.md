# Utils — GraphLoader, GraphExporter, Timer
### A Beginner-Friendly Line-by-Line Guide

> **Who is this file for?**
> This guide is written for someone who has learned basic C++ (variables, loops, functions) but has never seen concepts like templates, file streams, or RAII before. Every new concept is explained with a plain-English analogy **before** showing the code.

---

## The Three Utility Files at a Glance

| File | What It Does | Real-World Analogy |
|---|---|---|
| `Timer.hpp` | Measures how long code takes to run | A stopwatch that starts when you press play and stops itself |
| `GraphLoader.hpp` | Reads graph data from files on disk | A CSV reader that converts a spreadsheet into a graph |
| `GraphExporter.hpp` | Writes graph data to files | A PDF printer that saves your graph for other programs |

These three files are "helper" files — they don't contain any graph algorithms themselves. They give the algorithms the tools they need to load data, measure performance, and save results.

---
---

# Part 1: `Timer.hpp` — The Stopwatch

## What Problem Does It Solve?

Suppose you want to know how long Dijkstra's algorithm takes. You might think to do this:

```cpp
auto timeStart = /* record current time */;
dijkstra(graph, source);
auto timeEnd = /* record current time */;
print(timeEnd - timeStart);
```

That works, but it has a problem: **you have to remember to stop the timer**. If the code throws an error or returns early, the `timeEnd` line might never run and you get no result.

`Timer.hpp` solves this by using a C++ feature called **RAII** — which means "the timer starts itself when created and stops itself when it goes out of scope." You'll see exactly what this means below.

> **Plain English — What is "scope"?**
> A scope is a block of code wrapped in `{ }` curly braces. Variables created inside `{ }` are automatically destroyed when the closing `}` is reached. This is guaranteed by C++, even if an error occurs.

---

## Line by Line

### The Header Guard

```cpp
#pragma once
```

> **What this means:** This line tells the compiler "only include this file once, even if multiple other files try to include it." Without it, the same code could be pasted into your program twice, causing "duplicate definition" errors. Think of it like a bouncer at a door: the second time you try to enter, you get turned away.

---

### The Includes (Dependencies)

```cpp
#include <chrono>    // std::chrono — the standard library clock
#include <string>    // std::string — for the timer's name label
#include <iostream>  // std::cout — for printing elapsed time
#include <utility>   // std::move — for efficient string transfer
```

> **What this means:** These lines import code from the C++ Standard Library — a huge collection of pre-written tools that come with every C++ compiler. You don't have to write a clock from scratch; you just `#include <chrono>` and use it.

| Include | What It Gives You |
|---|---|
| `<chrono>` | A precise clock (accurate to nanoseconds) |
| `<string>` | The `std::string` type for text |
| `<iostream>` | `std::cout` to print to the terminal |
| `<utility>` | `std::move`, an optimization tool |

---

### The Class Definition

```cpp
class Timer {
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    bool active;
```

> **What is a class?** Think of a class as a blueprint. The `Timer` class is the blueprint for a stopwatch. Every stopwatch you create from this blueprint will have its own name, its own start time, and its own on/off flag.

**`name`** — A label like `"Dijkstra"` or `"BFS"`. When the timer stops, it prints this name so you know which algorithm was measured.

**`start`** — This stores the exact moment the timer was created. Later, we subtract this from the current time to find out how much time has passed.

> **What is `high_resolution_clock::time_point`?** Think of it as a specific instant frozen in time, like a photograph of the clock face at the moment the timer started. The type `high_resolution_clock` is the most precise clock available on your computer — often accurate to nanoseconds (billionths of a second).

**`active`** — A simple true/false flag. When `true`, the timer will print its result when it stops. When `false`, it stays silent. This lets us turn off the auto-print if we want to handle the result ourselves.

---

### The Constructor (Starting the Stopwatch)

```cpp
explicit Timer(std::string timerName)
    : name(std::move(timerName)),
      start(std::chrono::high_resolution_clock::now()),
      active(true) {}
```

> **What is a constructor?** It's a special function that runs automatically the moment you create a new object. You write `Timer t("Dijkstra")` and this code runs immediately, recording the start time.

**`explicit`** prevents accidental usage. Without it, the compiler might silently convert a plain string into a Timer in unexpected places. `explicit` forces you to be intentional: you must write `Timer("name")` explicitly.

**`: name(...), start(...), active(...)`** — This is called a **member initializer list**. It's the correct and efficient way to set up member variables in C++.

> **Why not just write assignments inside `{ }`?**
> ```cpp
> // LESS EFFICIENT — creates the variable, THEN assigns it
> name = timerName;
>
> // MORE EFFICIENT — creates the variable with the right value directly
> : name(std::move(timerName))
> ```
> The initializer list skips the "create empty, then fill" step.

**`std::move(timerName)`** — When you call `Timer("Dijkstra")`, the string `"Dijkstra"` is moved into `name` rather than copied. Moving a string is like handing over ownership of a document instead of photocopying it — it's faster (O(1) vs O(N)).

**`std::chrono::high_resolution_clock::now()`** — Takes a snapshot of the current time. This is the moment the stopwatch starts.

---

### The Destructor (Auto-Stopping)

```cpp
~Timer() {
    if (active) {
        double elapsed = elapsed_ms();
        std::cout << "[Timer] " << name << " took " << elapsed << " ms\n";
    }
}
```

> **What is a destructor?** It's a special function that runs automatically when an object is destroyed — i.e., when the closing `}` of its scope is reached.

This is the core of the RAII pattern. Here's what happens in practice:

```cpp
{
    Timer t("Kruskal");        // ← Constructor runs: clock starts
    auto mst = kruskal(graph);
}   // ← Closing brace reached. Destructor runs: clock stops and prints.
```

You **never** have to remember to stop the timer. It stops itself. Even if `kruskal()` throws an exception, the destructor still runs and you still get your timing result.

The `if (active)` check is important: if you've already called `.stop()` to get the result manually, the destructor should **not** print again. `active = false` prevents the double-print.

---

### The `stop()` Method

```cpp
double stop() {
    active = false;
    return elapsed_ms();
}
```

Use this when you need the timing result as a **number** to store or compare, not just to print it:

```cpp
{
    Timer t("Dijkstra");
    auto result = dijkstra(graph, src);
    double timeMs = t.stop();    // Get the number AND silence the destructor
    results.push_back(timeMs);   // Store it for comparison
}  // Destructor fires, but active=false so nothing is printed
```

---

### The `elapsed_ms()` Method (The Core Calculation)

```cpp
double elapsed_ms() const {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return static_cast<double>(duration) / 1000.0;
}
```

This is where the actual time calculation happens. Let's break it down step by step:

**Step 1 — Take a "now" snapshot:**
```cpp
auto end = std::chrono::high_resolution_clock::now();
```
`end` is the current time. We already have `start` from the constructor.

**Step 2 — Subtract to get duration:**
```cpp
end - start
```
Subtracting two time points gives you a **duration** — an amount of time. The type system ensures both points use the same clock, so this is always safe.

**Step 3 — Convert to microseconds:**
```cpp
std::chrono::duration_cast<std::chrono::microseconds>(...).count()
```
> **Why microseconds, not milliseconds?** Many algorithm runs finish in less than 1 millisecond. If we measured in whole milliseconds, we'd always see `0 ms`. Measuring in microseconds (1/1000th of a millisecond) gives us precision like `0.327 ms`.

`.count()` extracts a plain integer from the duration type.

**Step 4 — Convert to fractional milliseconds:**
```cpp
static_cast<double>(duration) / 1000.0
```
Divides by 1000 to go from microseconds → milliseconds. Using `1000.0` (not `1000`) forces the result to be a decimal number like `0.327`, not a rounded integer.

> **The `const` keyword:** This method is marked `const` because it doesn't change any member variables — it just reads `start`. Marking it `const` is like a promise: "I guarantee this method won't change the object." The compiler enforces this promise.

---
---

# Part 2: `GraphLoader.hpp` — The File Reader

## What Problem Does It Solve?

Hard-coding a graph inside your program is impractical for real use:

```cpp
// Imagine doing this for a 10,000-node road network... impossible.
g.addEdge(0, 1, 4);
g.addEdge(0, 2, 7);
// ... 50,000 more lines
```

Instead, you store the graph in a text file and load it at runtime. `GraphLoader.hpp` reads several different file formats and builds the graph for you.

---

## Before the Loaders: A Helper in a Private Namespace

```cpp
namespace GraphLoaderInternal {
    template<typename T>
    inline T parseToken(const std::string& token) {
        std::stringstream ss(token);
        T val;
        ss >> val;
        return val;
    }
}
```

There's a lot of new syntax here. Let's go through it piece by piece.

### What is a `namespace`?

> A namespace is like a folder for code. If you and a friend both write a function called `sort()`, they would conflict. Putting yours in `namespace Mine` means it's accessed as `Mine::sort()`, avoiding the clash. The `GraphLoaderInternal` namespace hides this helper so it doesn't pollute the global scope — it's an internal detail no one else needs to see.

### What is a `template<typename T>`?

> A template is a code blueprint that works for **any type**. Instead of writing:
> ```cpp
> int parseIntToken(string s)    { ... }
> double parseDoubleToken(string s) { ... }
> ```
> You write **one** template function and the compiler generates the right version for you:
> ```cpp
> parseToken<int>("42")     // returns int 42
> parseToken<double>("3.14") // returns double 3.14
> ```
> The `T` is a placeholder that gets replaced by the actual type when you use the function.

### What does `parseToken` actually do?

It converts a piece of text (like `"42"`) into a real C++ value (like the integer `42`). It does this using `std::stringstream`:

```cpp
std::stringstream ss(token);  // Create a stream from the string "42"
T val;
ss >> val;  // Read from the stream into val (just like cin >> val, but from a string)
return val;
```

> **Plain English:** A `stringstream` is like `std::cin` (keyboard input), but instead of reading from the keyboard, it reads from a string. The `>>` operator works exactly the same way.

### The Special Case for Strings

```cpp
template<>
inline std::string parseToken<std::string>(const std::string& token) {
    return token;  // A string is already a string — no conversion needed!
}
```

`template<>` with empty angle brackets means "this is a special override for one specific type." When `T = std::string`, we don't need to parse — the input is already a string, so we just return it directly.

---

## Loader 1: `loadFromCSV()` — Reading a Comma-Separated Edge List

### The File Format It Reads

```
# source,destination,weight   ← comment line (ignored)
0,1,4                         ← edge from node 0 to node 1, weight 4
0,2,7
1,3,2
```

### The Function Signature

```cpp
template<typename NodeType, typename WeightType, typename Hash>
void loadFromCSV(const std::string& filename, Graph<NodeType, WeightType, Hash>& g) {
```

> **`const std::string& filename`** — The `&` means "pass by reference." Instead of copying the filename string, we just look at the original. The `const` means we promise not to change it. This is the standard efficient way to pass strings in C++.
>
> **`Graph<...>& g`** — The graph is also passed by reference. This means the function **modifies the graph you give it** — it doesn't create a new graph and return it. This is intentional: the graph object can be very large, and we don't want to copy it.

### Opening the File

```cpp
std::ifstream file(filename);
if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
}
```

> **`std::ifstream`** stands for "input file stream." Think of it as a pipe connected to a file on your hard drive. Data flows from the file into your program through this pipe.

`is_open()` returns `false` if the file doesn't exist or can't be accessed (e.g., wrong path, no read permission). We throw an exception with the filename in the message — a good error message like `"Failed to open file: data/graph.csv"` tells you exactly what went wrong and where to look.

### Reading Line by Line

```cpp
std::string line;
int lineNumber = 0;
while (std::getline(file, line)) {
    lineNumber++;
```

`std::getline(file, line)` reads one complete line from the file into the `line` variable. The `while` loop keeps reading until the file runs out of lines (reaches End Of File). We track `lineNumber` so any error messages can say "error on line 7" instead of just "something went wrong."

### Skipping Comments and Blank Lines

```cpp
if (line.empty() || line[0] == '#') {
    continue;
}
```

`line.empty()` is true if the line is blank. `line[0] == '#'` is true if the first character is `#`. Either way, `continue` skips to the next loop iteration — we ignore blank lines and comment lines entirely.

### Splitting a Line on Commas

```cpp
std::stringstream ss(line);
std::string uStr, vStr, wStr;

if (std::getline(ss, uStr, ',') &&
    std::getline(ss, vStr, ',') &&
    std::getline(ss, wStr, ',')) {
```

We create a `stringstream` from the line (e.g., `"0,1,4"`) and use `std::getline` with a comma as the **delimiter** (separator) to split it into three parts:

```
line = "0,1,4"
         ↓ getline(ss, uStr, ',')  → uStr = "0"
         ↓ getline(ss, vStr, ',')  → vStr = "1"
         ↓ getline(ss, wStr, ',')  → wStr = "4"
```

The `&&` chains all three reads together. If any read fails (e.g., a line has only 2 columns), the whole `if` condition becomes false and we fall through to the error handler. This prevents crashes from malformed data.

### Adding the Edge

```cpp
NodeType u = GraphLoaderInternal::parseToken<NodeType>(uStr);
NodeType v = GraphLoaderInternal::parseToken<NodeType>(vStr);
WeightType w = GraphLoaderInternal::parseToken<WeightType>(wStr);

g.addEdge(u, v, w);
```

We convert our three strings into actual C++ values using `parseToken`, then add the edge to the graph. The template types (`NodeType`, `WeightType`) are automatically figured out from whatever graph type you passed in — you don't have to specify them manually.

---

## Loader 2: `loadFromAdjacencyList()` — A Different File Format

This loader reads a different format where each line lists a node and all its neighbors:

```
# Format: source  neighbor1,weight1  neighbor2,weight2  ...
0  1,4  2,7
1  2,1  3,5
```

The parsing uses nested `stringstream`s:
1. The outer `while (ss >> neighborBlock)` reads one space-separated token at a time (e.g., `"1,4"`)
2. Then each token is split on `,` to get the neighbor and weight

```cpp
std::string neighborBlock;
while (ss >> neighborBlock) {         // Read "1,4", then "2,7", etc.
    std::stringstream nss(neighborBlock);
    std::string vStr, wStr;
    if (std::getline(nss, vStr, ',') && std::getline(nss, wStr, ',')) {
        g.addEdge(u, parseToken<NodeType>(vStr), parseToken<WeightType>(wStr));
    }
}
```

> **Why two formats?** Different datasets come in different shapes. CSVs are common for downloaded data; adjacency list format is common in competitive programming. Supporting both makes the engine more versatile.

---

## Loader 3: `loadFromRoadNetworkCSV()` — Geographic Map Data

This is the most complex loader. It reads a file with two clearly labeled sections:

```
[Nodes]
101,22.3190,87.3091    ← node ID, latitude, longitude
102,22.3250,87.3110

[Edges]
101,102,95             ← source, destination, road distance
```

### Why Does It Need Latitude/Longitude?

The A\* algorithm needs a **heuristic** — an estimate of the remaining distance to the destination. For road networks, the straight-line geographic distance between two intersections is an excellent estimate. So we store the coordinates of each node.

### Section Detection with Boolean Flags

```cpp
bool parsingNodes = false;
bool parsingEdges = false;

if (line == "[Nodes]") {
    parsingNodes = true;
    parsingEdges = false;
    continue;
} else if (line == "[Edges]") {
    parsingNodes = false;
    parsingEdges = true;
    continue;
}
```

> **How flags work:** When we read `[Nodes]`, we flip `parsingNodes = true`. Every line after that is treated as node data. When we read `[Edges]`, we flip the flags and every subsequent line is treated as edge data.

This is a simple but effective **state machine** — the two booleans represent which "state" the parser is currently in.

### Storing Coordinates

```cpp
if (parsingNodes) {
    NodeType id = parseToken<NodeType>(idStr);
    double lat = std::stod(latStr);   // stod = "string to double"
    double lon = std::stod(lonStr);
    coordMap[id] = {lat, lon};        // Store {latitude, longitude} for this node
    g.addNode(id);
}
```

`coordMap` is an `unordered_map` (a hash table) mapping each node ID to a pair of coordinates. It's passed in by reference from the caller, so after loading, `main.cpp` can use the coordinates for the A\* heuristic.

---
---

# Part 3: `GraphExporter.hpp` — The File Writer

## What Problem Does It Solve?

After running an algorithm, you want to:
1. **Visualize the result** in the browser using D3.js
2. **Save the graph** so it can be reloaded later
3. **Record the step-by-step traversal order** for animation

`GraphExporter.hpp` provides functions to write all three.

---

## Exporter 1: `exportToJSON()` — For the D3.js Visualizer

This exports the entire graph in JSON format, which the web visualizer can read:

```json
{
  "nodes": [{"id": "0", "x": 22.31, "y": 87.30}, ...],
  "links": [{"source": "0", "target": "1", "weight": 4}, ...]
}
```

### Writing Nodes

```cpp
file << "  \"nodes\": [\n";
std::vector<NodeType> nodes = g.getAllNodes();
for (size_t i = 0; i < nodes.size(); ++i) {
    file << "    { \"id\": \"" << nodes[i] << "\"";

    auto it = coordMap.find(nodes[i]);
    if (it != coordMap.end()) {
        file << ", \"x\": " << it->second.first
             << ", \"y\": " << it->second.second;
    }

    file << " }";
    if (i + 1 < nodes.size()) { file << ","; }  // ← Trailing comma guard
    file << "\n";
}
```

> **Why the trailing comma guard?** JSON is strict: `[1, 2, 3,]` with a comma after the last item is **invalid JSON** and will cause errors in any JSON parser. The check `if (i + 1 < nodes.size())` ensures the comma only appears *between* items, never after the last one.

> **`coordMap.find(nodes[i])`** — We look up coordinates for each node. If found (`it != coordMap.end()`), we include `x` and `y` fields. If not found (the graph has no coordinates), we just output the `id` and skip the position. This makes the function work for both plain graphs and geographic road networks.

---

## Exporter 2: `exportAlgorithmSteps()` — For Animation

```cpp
template<typename NodeType>
void exportAlgorithmSteps(const std::vector<NodeType>& visitOrder, const std::string& filename) {
    file << "{\n  \"steps\": [\n";
    for (size_t i = 0; i < visitOrder.size(); ++i) {
        file << "    \"" << visitOrder[i] << "\"";
        if (i + 1 < visitOrder.size()) { file << ","; }
        file << "\n";
    }
    file << "  ]\n}\n";
}
```

`visitOrder` is a list of nodes in the order the algorithm visited them, e.g. `[0, 1, 3, 2, 4]` for a BFS run. This is written out as:

```json
{
  "steps": ["0", "1", "3", "2", "4"]
}
```

The D3.js visualizer reads this and animates each node lighting up one by one — node `0` first, then `1`, then `3`, etc. This lets you visually "watch" the algorithm explore the graph.

---

## Exporter 3: `exportToCSV()` — Round-Trip Saving

```cpp
template<typename NodeType, typename WeightType, typename Hash>
void exportToCSV(const Graph<NodeType, WeightType, Hash>& g, const std::string& filename) {
    file << "# source,destination,weight\n";
    for (const auto& u : g.getAllNodes()) {
        for (const auto& edge : g.neighbors(u)) {
            file << u << "," << edge.to << "," << edge.weight << "\n";
        }
    }
}
```

This writes every edge of the graph in the same format that `loadFromCSV()` reads. It's a perfect round-trip: load → modify → save → reload.

> **`for (const auto& u : g.getAllNodes())`** — This is a range-based for loop. `auto` lets the compiler figure out the type of `u` automatically. The `&` means we're referencing the original value, not copying it. The `const` means we won't change it.

---

## Key Design Decision: Exporters Warn, Loaders Throw

You may notice that export functions handle errors differently from load functions:

```cpp
// In exporters — just warn and return:
if (!file.is_open()) {
    std::cerr << "[Warning] Failed to export JSON to: " << filename << "\n";
    return;
}

// In loaders — throw an exception:
if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
}
```

**Why the difference?**

- **Loading is essential.** If you can't read the graph data, there's literally nothing to do. The whole program should stop. An exception forces the caller to handle this critical failure.

- **Exporting is optional.** If the visualization output folder doesn't exist, the algorithms can still run and print results to the console. A warning message is enough — we shouldn't crash a successful algorithm run just because we couldn't write a JSON file.

> **`std::cerr` vs `std::cout`:** Both print to the terminal, but `std::cerr` is for error/warning messages and `std::cout` is for normal output. They can be redirected separately in a shell, which is useful for scripts that capture program output without wanting to see warnings mixed in.

---

## Summary — What You've Learned

| Concept | Where It Appears | What It Does |
|---|---|---|
| RAII | `Timer` destructor | Auto-stops the timer when it goes out of scope |
| `template<typename T>` | All three files | Makes one function work for any data type |
| `std::ifstream` | All loaders | Opens a file for reading |
| `std::ofstream` | All exporters | Opens a file for writing |
| `std::stringstream` | `parseToken`, CSV parsing | Converts between strings and numbers |
| `std::move` | `Timer` constructor | Transfers ownership instead of copying |
| Pass by reference (`&`) | All function parameters | Avoids expensive copies of large objects |
| `explicit` | `Timer` constructor | Prevents accidental implicit conversions |
| `const` on methods | `elapsed_ms()` | Promises the method won't change the object |
| `namespace` | `GraphLoaderInternal` | Hides implementation details from users |
