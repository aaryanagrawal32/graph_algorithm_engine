# 03b. Physics vs Geographic Coordinates (`fx` and `fy`)

> **Reference: `graph.js` Lines 352–376 (inside `loadGraph()`)**

This file explains the "smart" coordinate system in our visualizer. It explicitly distinguishes between built-in library properties (like D3's `fx`) and user-defined variables (like `hasCoordinates`).

---

## 1. The Core Problem
Our C++ engine might give us two entirely different types of graphs:
1. **An Abstract Graph:** (e.g., `Node 1 -> Node 2`). These nodes have no physical location. We need D3 to invent coordinates for them so they look nice on screen.
2. **A Geographic Network:** (e.g., OpenStreetMap road data). These nodes already have real-world X/Y coordinates (longitude/latitude). If we let D3's physics engine push them apart randomly, the map of the city would be destroyed!

To solve this, our JavaScript checks the incoming data and dynamically chooses one of two behaviors.

---

## 2. Scenario A: Abstract Graphs (Dynamic Physics)

```javascript
// Step 1: Detect if the C++ JSON provided an 'x' property
const hasCoordinates = graphData.nodes.length > 0 && graphData.nodes[0].x !== undefined;
```
- **`hasCoordinates`** (User-defined variable): A boolean we created. If the first node lacks an `x` value, this becomes `false`.

If `hasCoordinates` is `false`, we skip all mathematical scaling and immediately create the D3 physics simulation.

```javascript
// Later down...
.force("charge", d3.forceManyBody().strength(hasCoordinates ? -50 : -350))
```
- **`strength(-350)`** (Built-in D3 parameter): Because `hasCoordinates` is false, we set the magnetic repulsion force to a very strong `-350`.
- **Result:** The nodes start in random, overlapping positions at the center of the screen. The physics engine violently pushes them apart until the springs (edges) pull them back, settling them into a beautifully spaced, readable layout. D3 calculates 100% of the screen coordinates.

---

## 3. Scenario B: Geographic Networks (Mathematical Scaling)

If `hasCoordinates` is `true`, we must preserve the exact geographic shape of the graph, but we have to shrink it down to fit on the user's screen.

### Step 3a: The Math Scaling
```javascript
const minX = d3.min(graphData.nodes, d => d.x);
const maxX = d3.max(graphData.nodes, d => d.x);
// ... same for Y ...
const scaleX = width / (maxX - minX || 1);
```
- **`d3.min` / `d3.max`** (Built-in D3 methods): These quickly scan the array and find the smallest/largest geographic coordinates, defining the total "bounding box" of our city.
- **`scaleX`** (User-defined variable): A ratio we calculate to shrink the massive geographic bounding box down to our exact SVG canvas width.

### Step 3b: Locking the Physics with `fx` and `fy`
Now we apply the scaling to every node. But notice *where* we save the results:

```javascript
graphData.nodes.forEach(node => {
    node.fx = padding + (node.x - minX) * scaleX;
    node.fy = padding + (node.y - minY) * scaleY;
});
```

Here is the most crucial concept in D3 physics control:
- **`node.x` / `node.y`** (Built-in D3 properties): The current, active coordinates being manipulated by the physics engine every 1/60th of a second.
- **`node.fx` / `node.fy`** (Built-in D3 properties): **Fixed X** and **Fixed Y**. 

When D3's physics engine calculates its next tick, it checks every node. **If a node has a value inside `fx` or `fy`, D3 completely disables the physics forces for that node.** It permanently pins the node to that exact pixel coordinate. 

*(Note: We also use this exact same trick during Drag-and-Drop! When you click a node, we set `d.fx = event.x` to lock it to your mouse pointer, fighting off the physics engine. When you let go, we set `d.fx = null` to release it back to the physics engine).*

### The Final Result
Because we set `fx` and `fy` for every single node in a geographic graph:
1. The physics engine is technically still running (at a weaker `-50` strength).
2. However, it isn't allowed to move the nodes at all, because they are all "pinned" by `fx`/`fy`.
3. The nodes stay perfectly locked in their real-world, scaled geographic shape!

---

## 4. The Timeline: How D3 "Takes Over" Your Coordinates

It is very common to wonder: *Are `node.x` and `node.y` just the static values from the C++ JSON, or are they D3's built-in physics variables?*

The answer is: **They are both.** D3 actually takes ownership of the properties that C++ sends. Here is the timeline of what happens in memory:

### Phase 1: Before D3 Starts (It's just your C++ data)
When JavaScript first reads the `graph.json` file, it creates plain objects. 
- If C++ exported a road network: `{ id: 1, x: 87.31, y: 22.34 }` *(Pure C++ values)*
- If C++ exported an abstract graph: `{ id: 1 }` *(No coordinates exist!)*

### Phase 2: The Moment D3 Starts (The Takeover)
When you pass the array to D3 (`simulation = d3.forceSimulation(graphData.nodes)`), D3 inspects every object:
- **If it sees you already provided an `x` and `y`**, it says: *"Ah, thank you! I will use these as the starting positions for my physics simulation."*
- **If it sees you did NOT provide an `x` and `y`**, it says: *"No coordinates? No problem."* It then **mutates** your object, forcibly adding `x` and `y` properties and assigning them random starting numbers.

### Phase 3: While the Simulation Runs (It is now a live D3 variable)
From this point forward, D3 completely owns `node.x` and `node.y`. 

Sixty times every second (every "tick"), the physics engine runs its math, calculates the new positions, and **overwrites** the `node.x` and `node.y` values in your objects. 

So, when you see this code inside the tick function:
```javascript
nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
```
You are reading the **live, built-in physics coordinates** that D3 just calculated one millisecond ago, *not* the original static values that came from C++. (The only exception is if you used `fx` and `fy` to lock them down, as explained in Scenario B!)
