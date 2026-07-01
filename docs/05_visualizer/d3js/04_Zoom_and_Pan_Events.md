# 04. Zoom, Pan, and Drag Events
*Reference: `visualizer/graph.js` (Lines 68 - 87, and 141 - 156)*

A static graph is boring. The web visualizer allows users to click and drag nodes to rearrange the physics, and scroll their mouse wheel to zoom in on giant graphs. Here is how D3 handles these complex interactions.

---

## 1. The Core Concepts

### How does Zooming work in SVG?
Unlike an HTML `div` where zooming increases the `font-size`, zooming in an SVG works like a camera. 
We don't actually make the circles bigger! Instead, we apply a mathematical `transform="scale(2)"` to the giant `<g>` folder holding all the circles. This makes the browser stretch the entire folder to 200% size, giving the illusion of zooming in.

### How does Dragging work with Physics?
When you click and drag a node, you are fighting against the physics engine!
1. When you click: You must "wake up" the physics engine so it can react to your movement. You must also lock the node's position so the physics engine doesn't try to pull it away from your mouse.
2. While dragging: You manually update the node's `x` and `y` coordinates to exactly match your mouse cursor.
3. When you release: You unlock the node so the physics engine can take over again.

---

## 2. Line-by-Line Application in `graph.js`

### Implementing Zoom and Pan (Lines 68 - 87)

```javascript
let zoom = d3.zoom()
    .scaleExtent([0.1, 4])
```
**Explanation:** 
- Creates a new zoom behavior.
- `.scaleExtent([0.1, 4])` sets the minimum and maximum zoom limits. The user can zoom out to 10% size (0.1), or zoom in to 400% size (4). This prevents them from zooming out so far the graph disappears, or zooming in so far a single circle fills the screen!

```javascript
    .on("zoom", (event) => {
        g.attr("transform", event.transform);
    });
```
**Explanation:** 
- `.on("zoom")`: Whenever the user scrolls their mouse wheel or clicks and drags the empty background.
- `g.attr("transform", event.transform)`: We take the mathematical transform calculated by D3 (e.g., "translate by 50px and scale by 2") and apply it directly to our master `<g>` folder!

*(Remember from section 1, we attached this behavior to the SVG using `svg.call(zoom)`).*

### Implementing Dragging (Lines 141 - 156)

These are the three functions we attached to every node using `.call(d3.drag()...)` in Section 2!

```javascript
function dragstarted(event, d) {
    if (!event.active) simulation.alphaTarget(0.3).restart();
    d.fx = d.x;
    d.fy = d.y;
}
```
**Explanation (Clicking):** 
- `if (!event.active)`: If this is the very first frame of the drag, we wake up the physics engine by setting `alphaTarget(0.3)`. This gives the simulation enough energy to let the other nodes bounce out of the way, but not so much energy that the whole graph explodes.
- `d.fx = d.x`: `fx` stands for "Fixed X". By setting `fx` to the current `x` position, we nail the node to the screen. The physics engine is now completely ignoring this node, allowing us to control it with the mouse!

```javascript
function dragged(event, d) {
    d.fx = event.x;
    d.fy = event.y;
}
```
**Explanation (Moving):** 
- As the user moves the mouse, D3 fires this function rapidly. We constantly update the "Fixed" position to exactly match the mouse's `event.x` and `event.y`.

```javascript
function dragended(event, d) {
    if (!event.active) simulation.alphaTarget(0);
    d.fx = null;
    d.fy = null;
}
```
**Explanation (Releasing):** 
- We tell the physics engine to cool back down to `0` energy.
- `d.fx = null`: **Crucial!** We delete the "Fixed" position! The node is no longer nailed to the screen. The physics engine instantly takes control of it again, allowing it to naturally snap back into place based on the spring/magnetic forces!
