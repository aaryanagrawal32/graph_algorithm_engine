# 03. The Physics Engine (Force Simulation)

> **Reference: `graph.js` Lines 377–385 (simulation setup) and Lines 427–445 (the tick loop)**

---

## Part 1: Why Do We Need a Physics Engine?

Our C++ backend's `graph.json` contains nodes and edges but **no X/Y coordinates** for where to draw them on screen. If nodes had no positions, we'd have to either:
1. Manually calculate positions (complex math for large graphs)
2. Place nodes randomly (they would overlap and look terrible)

D3's Force Simulation solves this automatically. It is a real-time 2D physics engine that runs every frame, calculating where each node should go based on invisible "forces". Over a few seconds, the nodes naturally settle into a beautiful, non-overlapping layout.

---

## Part 2: What is a "Force" in D3?

A D3 force is a mathematical function that takes a node's current position `(x, y)` and velocity `(vx, vy)` and adjusts those velocities. D3 applies all forces each frame, which causes nodes to accelerate toward or away from each other.

We use four forces in this project, each with a specific purpose:

| Force Name | Our Label | Built-in D3 Function | What it Simulates |
|---|---|---|---|
| Link Force | `"link"` | `d3.forceLink()` | **Rubber bands** — connected nodes attract each other |
| Charge Force | `"charge"` | `d3.forceManyBody()` | **Magnetism** — every node repels every other node |
| Collision Force | `"collide"` | `d3.forceCollide()` | **Physical size** — nodes can't overlap each other |
| Center Force | `"center"` | `d3.forceCenter()` | **Gravity** — all nodes are attracted to the canvas center |

---

## Part 3: Line-by-Line: The Simulation Setup (Lines 377–385)

```javascript
simulation = d3.forceSimulation(graphData.nodes)
    .force("link", d3.forceLink(graphData.links).id(d => d.id).distance(120))
    .force("charge", d3.forceManyBody().strength(hasCoordinates ? -50 : -350))
    .force("collide", d3.forceCollide().radius(25))
    .force("center", d3.forceCenter(
        document.getElementById("graph-svg").clientWidth / 2,
        document.getElementById("graph-svg").clientHeight / 2
    ));
```

### Line 378: `simulation = d3.forceSimulation(graphData.nodes)`

**`d3.forceSimulation()`** — This is a **built-in D3 factory function**. Calling it creates a new physics engine object and immediately starts calculating. The return value is the simulation object itself.

**`graphData.nodes`** — **Our variable**. We pass in our array of node objects. D3 does something very important here: it directly **mutates** (modifies) each object in this array by adding physics properties:
- `d.x` — Current X position (initialized randomly by D3)
- `d.y` — Current Y position (initialized randomly by D3)
- `d.vx` — Current X velocity
- `d.vy` — Current Y velocity
- `d.fx` — Fixed X position (null normally, set when dragging)
- `d.fy` — Fixed Y position (null normally, set when dragging)

D3 adds these fields directly onto the same objects that are in your array. This is why later we can write `d.x` in arrow functions — D3 put those numbers there!

**`simulation`** — **Our global variable** (line 33). We store the physics engine object here so other functions (like `dragstarted`) can access it.

---

### Line 379: `.force("link", d3.forceLink(graphData.links).id(d => d.id).distance(120))`

**`.force(name, forceObject)`** — This is a **built-in D3 method** on the simulation. It registers a force under a given name. The name (like `"link"`) is just a label we choose — it is not a keyword. We could call it `"edges"` or `"springs"` and it would work the same.

**`d3.forceLink(graphData.links)`** — This is a **built-in D3 factory function**. Creates a "spring force" for edges. Springs pull connected nodes together if they're too far apart and push them apart if they're too close.

**`graphData.links`** — **Our variable**. The array of edge objects like `[{source: 0, target: 1, weight: 4}, ...]`.

**`.id(d => d.id)`** — This is a **built-in D3 method on the link force**. It tells D3 how to identify nodes when processing edges. Without this, D3 would default to using array index position. With `.id(d => d.id)`, when D3 sees an edge `{source: 5, target: 12}`, it knows to look up the node with `id=5` and the node with `id=12`. The `d` here is a node object from `graphData.nodes`, and we return `d.id` as its identifier.

**`.distance(120)`** — This is a **built-in D3 method on the link force**. Sets the "rest length" of each spring to 120 pixels. If two connected nodes are exactly 120px apart, the spring is at equilibrium (no force). If they're closer than 120px, the spring pushes them apart. If farther, it pulls them together.

---

### Line 380: `.force("charge", d3.forceManyBody().strength(hasCoordinates ? -50 : -350))`

**`d3.forceManyBody()`** — This is a **built-in D3 factory function**. Creates a charge force where EVERY node affects EVERY other node. Like gravity in space — every mass affects every other mass (but we use negative values for repulsion, not attraction).

**`.strength(value)`** — This is a **built-in D3 method on the charge force**. The strength determines the force magnitude:
- **Negative values** → repulsion (nodes push each other away)
- **Positive values** → attraction (nodes pull toward each other)

**`hasCoordinates ? -50 : -350`** — This is a JavaScript ternary operator (equivalent to a one-line if-else). If the graph has pre-computed coordinates from C++, we use gentle repulsion (`-50`) because nodes already have good positions. If there are no coordinates, we use strong repulsion (`-350`) to spread nodes far apart into a readable layout.

---

### Line 381: `.force("collide", d3.forceCollide().radius(25))`

**`d3.forceCollide()`** — This is a **built-in D3 factory function**. Creates a collision force that prevents nodes from physically overlapping. Unlike `forceManyBody` (which acts from a distance), this force only activates when nodes get too close.

**`.radius(25)`** — The collision "bubble" around each node has a radius of 25 pixels. Our circles have radius 15px, so this gives a 10px buffer zone of empty space around each circle.

---

### Lines 382–385: `.force("center", d3.forceCenter(x, y))`

**`d3.forceCenter(x, y)`** — This is a **built-in D3 factory function**. Creates a centering force that gently pulls ALL nodes toward the point `(x, y)`. Without this, the magnetic repulsion would send all nodes flying off the edge of the screen!

**`document.getElementById("graph-svg").clientWidth / 2`** — This is plain JavaScript. `document.getElementById()` is a **built-in browser method** that finds a DOM element by its ID. `.clientWidth` is a **built-in DOM property** that gives the element's pixel width. Dividing by 2 gives us the horizontal center of the SVG canvas.

---

## Part 4: The Simulation Alpha — The "Temperature"

The physics simulation has an internal variable called `alpha` (α) that controls how energetic the simulation is.

- **Alpha = 1.0**: Maximum energy. Nodes fly around rapidly, large forces.
- **Alpha = 0.0**: Frozen. No forces applied, simulation is asleep (saves CPU/battery).

Every tick, D3 automatically reduces alpha by a tiny amount (multiplies by `1 - alphaDecay`, roughly 0.0228). This "cooling" causes the simulation to gradually slow down and stop — just like a physical system losing energy to friction. By default, D3 stops (calls `simulation.stop()` internally) when alpha drops below `0.001`.

You can control this with:
- **`simulation.alpha(1.0)`** — Forcibly set alpha to maximum and jolt the system awake
- **`simulation.alphaTarget(0.3)`** — Tell the simulation to *maintain* alpha at 0.3 (used during dragging)
- **`simulation.alphaTarget(0)`** — Release the alpha target, let it cool normally

---

## Part 5: The Tick Loop (Lines 427–440)

This is the most important part of D3 physics: **D3 moves the numbers, YOU must move the HTML**.

```javascript
simulation.on("tick", () => {
    linkSelection
        .attr("x1", d => d.source.x)
        .attr("y1", d => d.source.y)
        .attr("x2", d => d.target.x)
        .attr("y2", d => d.target.y);

    weightSelection
        .attr("x", d => (d.source.x + d.target.x) / 2)
        .attr("y", d => (d.source.y + d.target.y) / 2 - 4);

    nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
});
```

### `simulation.on("tick", callback)`

**`.on("tick", callback)`** — This is a **built-in D3 method**. It registers a callback function to be called every time the simulation calculates a new frame. A "tick" happens approximately 60 times per second (matching the browser's screen refresh rate).

**`() => { ... }`** — An arrow function with NO parameters. Unlike D3 data callbacks where `d` is passed in, this function doesn't need parameters because we're using our global `linkSelection` and `nodeSelection` variables directly.

---

### Moving the Edge Lines (Lines 429–433)

```javascript
linkSelection
    .attr("x1", d => d.source.x)
    .attr("y1", d => d.source.y)
    .attr("x2", d => d.target.x)
    .attr("y2", d => d.target.y);
```

**`linkSelection`** — **Our global variable**. The D3 selection of all 30 `<line>` tags.

**`.attr("x1", d => d.source.x)`** — For EACH line, set its `x1` attribute (the starting X coordinate) to the source node's current X position.

The `d` here is the edge data object bound to each `<line>`. Each edge object originally looked like `{source: 0, target: 1, weight: 4}`. But after D3's link force processed `graphData.links`, it **mutated** the source and target: it replaced the integer `0` with a direct reference to the actual node object `graphData.nodes[0]`. So now `d.source` is an object with `d.source.x` and `d.source.y` properties!

After one tick, D3 has updated `graphData.nodes[0].x = 342.5`. This function reads that value and sets the line's start point to pixel `342.5`. Then reads node 1's position for the end point. The line now connects the two current node positions.

---

### Moving the Weight Labels (Lines 435–437)

```javascript
weightSelection
    .attr("x", d => (d.source.x + d.target.x) / 2)
    .attr("y", d => (d.source.y + d.target.y) / 2 - 4);
```

**`(d.source.x + d.target.x) / 2`** — The arithmetic mean of the two endpoints' X coordinates. This is the mathematical midpoint formula: `midX = (x1 + x2) / 2`. This places the weight label exactly at the midpoint of the edge.

**`/ 2 - 4`** — The `-4` shifts the label 4 pixels upward (remember: in SVG, smaller Y = higher on screen). This prevents the text from sitting directly on top of the line, making it easier to read.

---

### Moving the Node Groups (Line 439)

```javascript
nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
```

**`nodeSelection`** — **Our global variable**. The D3 selection of all 20 `<g class="node-group">` tags.

**`.attr("transform", d => ...)`** — For EACH node group, set its SVG `transform` attribute.

**`` `translate(${d.x}, ${d.y})` ``** — A template literal that produces a string like `"translate(342.5, 198.1)"`. D3 has updated `d.x` and `d.y` with the new physics positions.

The SVG `transform="translate(342.5, 198.1)"` attribute moves the entire `<g>` group (circle + label together) to that position. Because both the circle and text are children of the `<g>`, they both move at once without needing separate positioning.

This is the key insight: **The physics engine updates numbers. The tick function copies those numbers into HTML attributes. The browser re-renders the screen. This happens 60 times a second, creating the smooth animation.**

---

## Part 6: The Fit-to-Screen Call (Lines 443–445)

```javascript
setTimeout(() => {
    fitGraphToScreen();
}, 200);
```

**`setTimeout(callback, delayMs)`** — This is a **built-in browser function** (not D3). It schedules `callback` to run once after `delayMs` milliseconds. It does NOT block like `std::this_thread::sleep_for` — the rest of the code continues immediately.

We wait 200ms because the physics simulation needs a few ticks to spread the nodes into a reasonable layout before we try to calculate the bounding box and zoom to fit. Without this delay, the nodes would still be clustered randomly at the center and the "fit" calculation would produce a tiny, useless view.
