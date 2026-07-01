# 00. Prerequisites: JavaScript Concepts & SVG Math

> **Read this first before any other D3.js file!**
> 
> D3.js is built on top of two foundations: **JavaScript language features** and **SVG (the drawing canvas)**. If you jump straight into D3 without knowing these, the code will feel like magic — but confusing magic.
> 
> This file teaches both from scratch with C++ comparisons.

---

## Part 1: JavaScript Concepts Used Heavily in D3

### 1.1 — Arrow Functions and the `d =>` Syntax

This is the **single most confusing syntax** in D3 for C++ developers.

**In C++**, if you want a small throwaway function, you write a lambda:
```cpp
auto myFunc = [](int x) { return x * 2; };
```

**In JavaScript**, the equivalent is called an **Arrow Function**:
```javascript
let myFunc = (x) => { return x * 2; };
// Shorthand (when body is a single expression):
let myFunc = x => x * 2;
```

Both mean exactly the same thing: "`myFunc` is a function that takes one argument named `x` and returns `x * 2`."

**Key Rule:** The letter `x` (or `d`, or `node`, or `potato`) is **just a parameter name you invented**. You could call it anything. D3 conventionally uses `d` because it stands for "data", but it is not a keyword.

**A Callback Function** is when you pass your function *as an argument* to another function, letting that other function call yours:
```javascript
// This is YOUR formula. You are NOT calling it. You are handing it over.
linkSelection.attr("x1", d => d.source.x);
//                         ^^^^^^^^^^^^^^
//              D3 will call this 50+ times per second, once per line,
//              passing the data for each line as the argument 'd'.
```

Think of it like this: You give D3 a stamping machine (`d => d.source.x`). D3 picks up each edge from your array and stamps it through the machine to get the answer for that specific edge.

---

### 1.2 — `let` vs. `const` (Compared to C++ `int`)

In C++, you declare a variable with its type:
```cpp
int currentStep = 0;
string activeAlgorithm = "";
```

In JavaScript, there are no explicit types. You use `let` for variables that can change, and `const` for variables that can never be reassigned:
```javascript
let currentStep = 0;         // Can be changed: currentStep = 5;  ✓
const padding = 120;         // Cannot be reassigned: padding = 200; ✗ ERROR
```

In our `graph.js`, variables declared at the very top (lines 12–39) with `let` are **global variables**. They are accessible from every single function in the file. This is why `simulation`, `nodeSelection`, `linkSelection` etc. are declared there with `let` — they are created inside `loadGraph()` but used inside `dragstarted()`, `tick`, and everywhere else.

---

### 1.3 — Template Literals (The Modern String)

In C++, building strings requires `+` concatenation which is clunky:
```cpp
string msg = "Node " + to_string(id) + " has coords (" + to_string(x) + ")";
```

In modern JavaScript, you use **Template Literals** with backticks (`` ` ``):
```javascript
let msg = `Node ${id} has coords (${x})`;
//         ^^^^^^^^^^^^^^^^^^^^^^^^^^^
//   The ${...} syntax injects any JS expression directly into the string!
```

You will see this constantly in D3:
```javascript
nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
//                                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  For each node, produces a string like: "translate(342.5, 198.1)"
```

---

### 1.4 — Objects: The JavaScript `struct`

In C++, you use `struct` to group related data:
```cpp
struct Node { int id; double x; double y; };
```

In JavaScript, you use an **Object** with curly braces:
```javascript
let node = { id: 5, x: 342.5, y: 198.1 };
```

You access fields with a dot, just like C++:
```javascript
console.log(node.id);  // → 5
console.log(node.x);   // → 342.5
```

When our C++ engine outputs `graph.json`, each node entry looks like this:
```json
{ "id": 5, "x": 350.0, "y": 200.0 }
```

D3 reads this JSON and creates an array of JavaScript objects like the one above. When you write `d => d.id`, the `d` is one of those objects, and `d.id` is reading its `id` field.

---

### 1.5 — `null` vs. Unassigned (Important for D3 Drag!)

In C++, uninitialized pointer behavior is undefined. In JavaScript, `null` is an intentional "this has no value" marker.

In our drag code (lines 463-469), we use `null` to **release** a node from the physics engine:
```javascript
d.fx = null;  // Setting fx to null tells D3: "Stop fixing this node, let physics control it"
d.fy = null;
```

---

## Part 2: Understanding SVG — The Drawing Canvas

### 2.1 — What is SVG and Why Not `<canvas>`?

The webpage's main drawing area is an `<svg>` tag. SVG stands for **Scalable Vector Graphics**.

There are two ways to draw graphics in a browser:
| Feature | `<canvas>` (Pixel-based) | `<svg>` (Math-based) |
|---|---|---|
| Zoom in 400% | Image gets blurry/pixelated | Stays perfectly crisp |
| Interactivity | You must manually track clicks | Each shape is its own DOM element you can click |
| Used for | Video games, photo editing | Graphs, charts, diagrams |

D3.js exclusively uses SVG. Because SVG shapes are defined by math (e.g., "draw a circle centered at x=300, y=200 with radius=15"), zooming just changes the math scale — the circles never go blurry.

---

### 2.2 — The SVG Coordinate System (CRITICAL!)

⚠️ **This is the #1 thing that confuses everyone coming from math or C++.**

In school math, the Y-axis goes **up**. In SVG, the Y-axis goes **down**.

```
(0,0) -------- x increases →
  |
  |
  y increases ↓
```

So if a node has `y = 300`, it is 300 pixels **down** from the top of the screen. If another node has `y = 100`, it is **above** the first node (closer to the top), not below it.

This matters enormously when reading the tick function:
```javascript
// This puts the node at (342px from left, 198px from top)
nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
```

---

### 2.3 — SVG Tags: The Building Blocks

Inside an `<svg>` element, you use special SVG-only tags to draw shapes. These are NOT HTML tags:

| SVG Tag | What It Draws | Key Attributes |
|---|---|---|
| `<circle>` | A circle | `cx` (center X), `cy` (center Y), `r` (radius) |
| `<line>` | A straight line | `x1, y1` (start point), `x2, y2` (end point) |
| `<text>` | Text on the canvas | `x, y` (position), `text-anchor` (alignment) |
| `<path>` | Any arbitrary shape | `d` (a series of drawing commands) |
| `<g>` | An invisible group/folder | `transform` (move the whole group) |
| `<defs>` | A hidden definition area | Contains reusable shapes (like arrowheads) |
| `<marker>` | Defines an arrowhead shape | Used as `url(#id)` on lines |

> **Important:** You CANNOT use `<div>` or `<button>` inside an `<svg>`. Those are HTML tags. SVG is a completely separate language with its own tag vocabulary.

---

### 2.4 — The `transform="translate(x, y)"` Attribute

This is how D3 moves our node groups (circles + labels) around the screen during physics.

A `<g>` group tag doesn't have `x` and `y` attributes. Instead, it uses `transform`:

```html
<!-- Move the whole group 100px right and 200px down -->
<g transform="translate(100, 200)">
    <circle r="15"></circle>     <!-- circle is now centered at (100, 200) -->
    <text>5</text>               <!-- text is also at (100, 200) -->
</g>
```

When D3's physics engine calculates a new position for node 5 (`d.x = 342.5, d.y = 198.1`), it updates the group's transform:
```javascript
// 60 times per second, D3 produces this string and sets it on the <g> tag:
// "translate(342.5, 198.1)"
nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
```

This drags the entire group (circle + label together) to the new position instantly.

---

### 2.5 — The `<path d="...">` Command Language

The `<path>` tag draws arbitrary shapes using a mini command language in its `d` attribute. You will see this when we explain the arrowheads.

| Command | Meaning |
|---|---|
| `M x,y` | **M**ove to point (x,y) without drawing |
| `L x,y` | Draw a **L**ine to point (x,y) |
| `Z` | **Z**ip/close the path back to start |

Our arrowhead is drawn with: `d="M0,-4L9,0L0,4"`

Reading it step by step:
- `M0,-4` → Move to point (0, -4) (top-left of arrow)
- `L9,0` → Draw a line to point (9, 0) (the sharp tip)
- `L0,4` → Draw a line to point (0, 4) (bottom-left of arrow)

This draws a small triangle pointing right — the arrowhead shape!

---

Now that you understand these fundamentals, the D3 code will make complete sense. Proceed to the next file.
