# 03. Force Simulation (The Physics Engine)
*Reference: `visualizer/graph.js` (Lines 111 - 139, and 245 - 253)*

How do the nodes know where to go on the screen? Why do they bounce around and settle into a nice web shape instead of drawing on top of each other? 

Because we are running a real-time 2D physics simulation!

---

## 1. The Core Concepts

### What is `d3.forceSimulation`?
D3 includes a built-in physics engine. When you feed it an array of nodes, it assigns a random `x` and `y` coordinate to each one. Then, 60 times a second, it applies mathematical forces to them.

### The Three Invisible Forces
To make a graph look good, we apply three main forces:
1. **Force Center (Gravity):** A black hole in the exact center of the screen that constantly pulls all nodes towards it. Without this, the nodes would float away into deep space!
2. **Force ManyBody (Magnetism):** Every node acts like a magnet with a negative charge. They constantly repel each other. This ensures no two nodes ever overlap on the screen.
3. **Force Link (Springs):** The edges connecting the nodes act like rubber bands. They pull connected nodes together, fighting against the magnetic repulsion!

The simulation runs these math equations rapidly, moving the nodes slightly every frame until they reach an equilibrium (where the springs and magnets cancel each other out) and stop moving.

---

## 2. Line-by-Line Application in `graph.js`

### Initializing the Physics Engine (Lines 111 - 120)

```javascript
simulation = d3.forceSimulation()
```
**Explanation:** 
- Creates a new, empty physics engine.

```javascript
    .force("link", d3.forceLink().id(d => d.id).distance(100))
```
**Explanation:** 
- Adds the "Rubber Band" force.
- `.id(d => d.id)` tells the physics engine how to find which node is connected to which (by checking their IDs).
- `.distance(100)` sets the resting length of the rubber band to 100 pixels. If nodes get further than 100px apart, it pulls them in. If they get closer than 100px, it pushes them out.

```javascript
    .force("charge", d3.forceManyBody().strength(-300))
```
**Explanation:** 
- Adds the "Magnetism" force.
- `forceManyBody` makes every node affect every other node.
- `.strength(-300)` is a negative number, meaning repulsion. (A positive number would mean attraction, making them all instantly crash into each other).

```javascript
    .force("center", d3.forceCenter(width / 2, height / 2));
```
**Explanation:** 
- Adds the "Gravity" force.
- `width / 2` and `height / 2` are the exact pixel coordinates of the middle of the screen.

### Feeding Data to the Engine (Lines 245 - 250)

When we load a new graph from C++, we have to put it into the engine.

```javascript
simulation.nodes(graphData.nodes)
    .on("tick", ticked);
```
**Explanation:** 
- We hand the array of nodes to the simulation.
- `.on("tick", ticked)`: **This is the most important line in D3 physics!** 
- A "tick" is one frame of animation (occurring ~60 times a second). D3 calculates the new math, but **D3 does not draw the shapes on the screen for you!** 
- Every tick, D3 calls our custom function named `ticked`. Inside that function, we must manually update the HTML to match the new math.

```javascript
simulation.force("link")
    .links(graphData.links);
```
**Explanation:** 
- We hand the array of edges to the rubber band force so it knows which nodes are connected.

```javascript
simulation.alpha(1).restart();
```
**Explanation:** 
- `alpha` is the "heat" or "energy" of the simulation. `1` is maximum energy. Over a few seconds, alpha cools down to `0`, and the simulation goes to sleep to save laptop battery. We set it to 1 to violently wake it up and calculate the new layout!

### The Render Loop (Lines 125 - 139)

This is the `ticked` function that runs 60 times a second.

```javascript
function ticked() {
    link
        .attr("x1", d => d.source.x)
        .attr("y1", d => d.source.y)
        .attr("x2", d => d.target.x)
        .attr("y2", d => d.target.y);
```
**Explanation:** 
- We grab all the SVG `<line>` tags.
- D3's physics engine has been secretly updating `d.source.x` and `d.source.y` (the math coordinates) in the background. We read those math coordinates and manually set the HTML `x1`, `y1` (start point) and `x2`, `y2` (end point) attributes of the line!

```javascript
    node
        .attr("transform", d => `translate(${d.x},${d.y})`);
}
```
**Explanation:** 
- We grab all the `<g>` tags (which hold our circles and text).
- Instead of setting x/y directly, we use an SVG `transform` to instantly teleport the entire group (circle + text together) to the new `d.x` and `d.y` coordinates calculated by the physics engine!
