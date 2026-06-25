# C++ Syntax Cheat Sheet — Every Construct Used in This Project

This is a **single-page reference** for every C++ syntax feature used in the Graph Algorithm Engine. If you see something in the code and don't know what it does, search this file.

---

## 1. Basic Structure

### `#pragma once`
```cpp
#pragma once
```
**What it does**: Tells the compiler "only include this file once per compile, even if multiple files `#include` it."
**Why it matters**: Without it, you'd get "function already defined" errors when two files both include the same header.
**Alternative**: The older pattern `#ifndef FILE_H / #define FILE_H / ... / #endif` does the same thing.

---

### `#include`
```cpp
#include <vector>       // Angle brackets = standard library
#include "Graph.hpp"    // Quotes = your own project files
```
**What it does**: Pastes the entire contents of another file at this location. It's a text substitution done before compilation.
**Think of it as**: Copy-pasting the file's contents here.

---

### `namespace`
```cpp
namespace GraphUtils {
    struct SafeHash { ... };
}

// Using it:
GraphUtils::SafeHash h;   // :: is the "scope resolution operator"
```
**What it does**: Groups related names together to avoid conflicts. Like a folder for names.
**`::` operator**: "Look inside this namespace/class for the next name."

---

## 2. Variables and Types

### `auto`
```cpp
int x = 5;              // x is int
auto path = bfs(g, 0);  // path is vector<int> — compiler figures it out
auto& ref = someMap;    // ref is a reference to someMap
```
**What it does**: Tells the compiler to figure out the type from the right-hand side.
**Why use it**: Saves typing long types like `std::unordered_map<int, std::pair<double,double>>`.
**Rule**: Use `auto` when the type is obvious from context. Don't use it when it obscures what type something is.

---

### `const`
```cpp
const int x = 5;              // x can never be changed
const std::string& name = s;  // read-only reference to s
void print() const { ... }    // this method cannot change the object
```
**What it does**: Makes something read-only. The compiler enforces this — trying to modify it is a compile error.
**const reference (`const T&`)**: The most common use — pass a large object to a function without copying it, but guarantee you won't change it.

---

### References (`&`)
```cpp
int x = 5;
int& ref = x;    // ref IS x — same memory location
ref = 10;        // x is now 10
```
**vs. copy**:
```cpp
int y = x;   // y is a COPY — changing y doesn't change x
int& r = x;  // r IS x — changing r changes x
```
**In function parameters**:
```cpp
void modify(std::vector<int>& v) { v.push_back(1); }  // modifies the original
void read(const std::vector<int>& v) { /* ... */ }     // reads without copying
```

---

### `size_t`
```cpp
size_t n = v.size();  // size_t is an unsigned integer
for (size_t i = 0; i < v.size(); ++i) { ... }
```
**What it does**: An unsigned integer type used for sizes and indices. `vector::size()` returns `size_t`, not `int`.
**Why not `int`**: `int` is signed (can be negative). Sizes can't be negative. Using `size_t` avoids compiler warnings about comparing signed/unsigned.

---

## 3. Templates

### Function Templates
```cpp
template<typename T>
T maximum(T a, T b) {
    return (a > b) ? a : b;
}

// Usage:
int m1 = maximum(3, 7);           // T = int, compiler fills it in
double m2 = maximum(3.14, 2.71);  // T = double
```
**What it does**: Write one function that works for any type `T`. The compiler generates the actual code at compile time.
**Key insight**: There is no "template code" in the compiled binary — the compiler creates a separate concrete function for each type you use it with.

### Class Templates
```cpp
template<typename NodeType, typename WeightType = int>
class Graph { ... };

Graph<int> g;                  // NodeType=int, WeightType=int (default)
Graph<std::string, double> g2; // NodeType=string, WeightType=double
```
**Default template parameters (`= int`)**: If the caller doesn't specify `WeightType`, use `int` automatically.

### Template with Multiple Parameters
```cpp
template<typename NodeType, typename WeightType, typename Hash = GraphUtils::SafeHash>
void dijkstra(const Graph<NodeType, WeightType, Hash>& g, NodeType src) { ... }
```
The compiler deduces `NodeType`, `WeightType`, and `Hash` from the type of `g` you pass in.

---

## 4. STL Containers

### `std::vector<T>` — Dynamic Array
```cpp
std::vector<int> v;
v.push_back(5);        // Add to end
v.size();              // Number of elements
v[0];                  // Access by index (no bounds check)
v.at(0);              // Access by index (with bounds check, throws on error)
v.empty();             // true if size() == 0
v.clear();             // Remove all elements
for (int x : v) { }   // Range-based for loop
```

### `std::unordered_map<K, V>` — Hash Map
```cpp
std::unordered_map<int, double> dist;
dist[5] = 3.14;              // Insert or update key 5
dist.find(5);                // Returns iterator to key 5 (or .end() if not found)
dist.find(5) == dist.end();  // true if key 5 is NOT in the map
dist.count(5);               // 1 if key 5 exists, 0 if not
dist.size();                 // Number of key-value pairs
for (auto& [k, v] : dist) { ... }  // Loop over all key-value pairs
```

### `std::unordered_set<T>` — Hash Set
```cpp
std::unordered_set<int> visited;
visited.insert(5);
visited.count(5);                    // 1 if present, 0 if not
visited.find(5) == visited.end();    // true if NOT in set
visited.erase(5);                    // Remove element
```
**Set vs Map**: A set only stores keys (no values). Use it for "is X visited?" checks.

### `std::queue<T>` — FIFO Queue (for BFS)
```cpp
std::queue<int> q;
q.push(5);      // Add to back
q.front();      // Peek at front (don't remove)
q.pop();        // Remove from front (returns void!)
q.empty();      // true if empty
q.size();       // Number of elements
```
**FIFO**: First In, First Out. What you push first, you pop first.

### `std::stack<T>` — LIFO Stack (for DFS)
```cpp
std::stack<int> s;
s.push(5);      // Add to top
s.top();        // Peek at top (don't remove)
s.pop();        // Remove from top (returns void!)
s.empty();      // true if empty
```
**LIFO**: Last In, First Out. What you push last, you pop first.

### `std::priority_queue<T>` — Heap
```cpp
// Max-heap (default) — largest element always at top
std::priority_queue<int> maxPQ;
maxPQ.push(3); maxPQ.push(1); maxPQ.push(4);
maxPQ.top();   // = 4 (largest)

// Min-heap — smallest element at top (needed for Dijkstra)
std::priority_queue<int, std::vector<int>, std::greater<int>> minPQ;
minPQ.push(3); minPQ.push(1); minPQ.push(4);
minPQ.top();   // = 1 (smallest)
```
**`greater<int>`** reverses the comparison, turning the max-heap into a min-heap.

### `std::pair<A, B>` — Two Values Together
```cpp
std::pair<int, int> p = {3, 7};
p.first;   // = 3
p.second;  // = 7

// Common use: priority queue entries
std::pair<double, int> entry = {dist, nodeId};
```

### `std::tuple<A, B, C>` — Three+ Values Together
```cpp
std::tuple<int, int, int> edge = {weight, u, v};
std::get<0>(edge);  // = weight
std::get<1>(edge);  // = u
std::get<2>(edge);  // = v

// With C++17 structured binding:
auto [w, u, v] = edge;  // Unpack directly into named variables
```

---

## 5. Modern C++17 Features

### Structured Bindings
```cpp
// Old way:
auto p = std::make_pair(1, 2);
int a = p.first;
int b = p.second;

// C++17 way:
auto [a, b] = std::make_pair(1, 2);  // a=1, b=2

// With tuples:
auto [weight, u, v] = myTuple;   // Unpacks all three

// In loops:
for (auto& [key, value] : myMap) {
    // key and value are directly accessible
}
```

### `if constexpr`
```cpp
template<typename T>
void process(T x) {
    if constexpr (std::is_integral_v<T>) {
        // Only compiled for integer types
    } else {
        // Only compiled for non-integer types
    }
}
```
**`constexpr`**: "Evaluate at compile time, not runtime." `if constexpr` lets the compiler skip generating code for branches that don't apply to the current type.

### `std::optional<T>`
```cpp
std::optional<int> findValue(int key) {
    if (/* found */) return 42;
    return std::nullopt;  // "No value"
}

auto result = findValue(5);
if (result.has_value()) {
    int x = result.value();
}
```
**What it does**: A type that either holds a value or holds nothing. Cleaner alternative to returning `-1` or `nullptr` to signal "not found."

---

## 6. Functions and Lambdas

### Lambda Functions
```cpp
// Basic lambda
auto add = [](int a, int b) { return a + b; };
add(3, 4);  // = 7

// Lambda capturing local variables by reference [&]
int multiplier = 5;
auto scale = [&](int x) { return x * multiplier; };
scale(3);  // = 15 — uses the outer 'multiplier'

// Lambda as heuristic for A*
auto heuristic = [&](int u, int v) {
    double dx = coordMap[u].first - coordMap[v].first;
    double dy = coordMap[u].second - coordMap[v].second;
    return std::sqrt(dx*dx + dy*dy);
};
```
**`[&]`**: Capture everything by reference — the lambda can read and write all local variables.
**`[]`**: Capture nothing — the lambda can only use its own parameters.
**`[=]`**: Capture everything by value — the lambda gets copies.

### `std::function<ReturnType(ArgTypes)>`
```cpp
std::function<double(int, int)> h;  // A callable that takes 2 ints, returns double
h = [](int a, int b) { return std::abs(a - b); };  // Assign any lambda/function
```
Used in `AStar.hpp` to accept any heuristic function regardless of how it's implemented.

---

## 7. Object-Oriented C++

### `struct` vs `class`
```cpp
struct Point {        // Members are PUBLIC by default
    int x, y;
};

class Circle {        // Members are PRIVATE by default
    double radius;
public:
    double area();
};
```
**Convention**: Use `struct` for plain data bundles. Use `class` for objects with hidden state and behavior.

### Constructor and Member Initializer List
```cpp
class Timer {
    std::string name;
    double startTime;
public:
    Timer(std::string n) 
        : name(std::move(n)),         // Initialize name
          startTime(currentTime())    // Initialize startTime
    {
        // Constructor body (optional)
    }
};
```
**Member initializer list** (after `:`): More efficient than assigning inside the body — constructs directly instead of default-constructing then copying.

### Destructor
```cpp
class Timer {
    ~Timer() {
        // Runs automatically when the object goes out of scope
        std::cout << "Elapsed: " << elapsed() << " ms\n";
    }
};
```
**RAII pattern**: Acquire resource in constructor, release/use in destructor. The destructor fires automatically, even if an exception is thrown.

### `const` Methods
```cpp
class Graph {
    int nodeCount() const {   // This method cannot modify the object
        return adjList.size();
    }
};
```

---

## 8. Error Handling

### `throw` and `try/catch`
```cpp
// Throwing an exception
if (hasCycle) {
    throw std::runtime_error("Graph has a cycle — cannot topological sort");
}

// Catching it
try {
    auto order = topologicalSort(g);
} catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

### Common Exception Types
| Type | When to Use |
|---|---|
| `std::runtime_error` | Error detected at runtime (negative cycle, unreachable node) |
| `std::invalid_argument` | Wrong argument passed (e.g., undirected graph to Tarjan) |
| `std::out_of_range` | Index/key access out of bounds |

---

## 9. Iterators

### What Is an Iterator?
An iterator is like a pointer that knows how to move through a container.

```cpp
std::vector<int> v = {1, 2, 3};
auto it = v.begin();   // Points to first element (1)
*it;                   // Dereference: gets the value (1)
++it;                  // Move to next element
*it;                   // Now = 2
it == v.end();         // true when past the last element
```

### Range-Based For Loop (uses iterators internally)
```cpp
for (const auto& edge : g.neighbors(u)) {
    // edge is each element in the neighbors vector
    // const = don't modify it
    // & = don't copy it
}
```

### Reverse Iterators
```cpp
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    // Iterates from last element to first
}
```

---

## 10. Common Patterns

### `.find()` Pattern for Maps and Sets
```cpp
auto it = myMap.find(key);
if (it != myMap.end()) {
    // Key exists
    auto value = it->second;  // Access the value
} else {
    // Key doesn't exist
}
```
**Never use `myMap[key]` to check existence** — it inserts the key with a default value if it's not there!

### `std::numeric_limits<T>::max()`
```cpp
int INF = std::numeric_limits<int>::max();      // Largest possible int
double INF = std::numeric_limits<double>::infinity();  // +∞ for double
```
Used to initialize distances in Dijkstra/Bellman-Ford to "infinity."

### `std::move()`
```cpp
std::vector<int> a = {1, 2, 3};
std::vector<int> b = std::move(a);  // b now owns the data; a is empty
```
**What it does**: Transfers ownership of a resource instead of copying. O(1) instead of O(N) for containers.

### `++i` vs `i++`
```cpp
++i;  // Increment FIRST, then use value (pre-increment)
i++;  // Use value FIRST, then increment (post-increment)
```
For simple loop counters they're identical. For complex types, `++i` is slightly faster. Always prefer `++i` in loops.

---

## 11. Common Compiler Errors Explained

| Error Message | What It Means | Fix |
|---|---|---|
| `deduced conflicting types for parameter 'Hash'` | Two template args forced to be same type but aren't | Add a separate template parameter for each independent type |
| `'unordered_set' is not a member of 'std'` | Missing `#include <unordered_set>` | Add the include |
| `no match for 'operator=='` | Comparing two objects that don't have `==` defined | Define `operator==` or use a different comparison |
| `cannot convert ... to ... without a cast` | Implicit type conversion not allowed | Add explicit cast or use the correct type |
| `use of deleted function` | You tried to copy something that's move-only | Use `std::move()` or take by reference |
| `expected primary-expression before '>'` | Template angle bracket parsing issue | Add extra parentheses or use `typename` keyword |
| `undefined reference to ...` | Linker can't find the implementation | Add the missing `.cpp` to CMake target, or move implementation to header |
