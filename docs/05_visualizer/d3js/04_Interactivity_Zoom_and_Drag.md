# 04. Interactivity: Zoom, Pan, and Drag

> **Reference: `graph.js` Lines 144–150 (zoom setup), Lines 451–469 (drag functions), Lines 472–493 (fit to screen)**

---

## Part 1: Understanding Zoom in SVG

Before reading the code, you need to understand how SVG zoom works — it is fundamentally different from HTML zoom.

### In HTML:
When you zoom a webpage in Chrome (Ctrl+Scroll), the browser increases the size of every element — text gets bigger, images get bigger.

### In SVG:
We don't resize the elements at all. Instead, we apply a **mathematical transformation** to the `<g>` container holding everything. Zooming is equivalent to changing the "camera" position — the SVG canvas stays the same size, but we scale and translate the master group to simulate a zoom.

The math is done using an **SVG Transform Matrix** — a 3×3 matrix that encodes scale, rotation, and translation. D3's zoom behavior handles this math automatically and gives us a simple `event.transform` object we can directly apply.

---

## Part 2: Zoom and Pan — Line by Line (Lines 144–150)

> Note: `zoomBehavior` was declared as our global variable on line 38.
> The zoom was **configured** in `setupSVG()` (lines 144–150), but **used** in `fitGraphToScreen()` (line 490).

```javascript
// Line 144
zoomBehavior = d3.zoom()
```

**`d3.zoom()`** — This is a **built-in D3 factory function**. Calling it returns a new zoom behavior object. Think of it as a controller that watches for mouse wheel events and touch pinches, calculates the new scale and position, and fires events.

**`zoomBehavior`** — **Our global variable** (line 38). We store the zoom behavior object here so we can programmatically trigger zooms from the zoom buttons and the `fitGraphToScreen()` function.

---

```javascript
// Line 145
    .scaleExtent([0.1, 4])
```

**`.scaleExtent([min, max])`** — This is a **built-in D3 method on the zoom behavior**. Sets the min and max allowed zoom scale.
- `0.1` → User can zoom out to 10% (graph looks like tiny dots — a distant aerial view)
- `4` → User can zoom in to 400% (one circle is huge, only a few nodes visible)

Pressing Ctrl+Scroll beyond these limits does nothing.

---

```javascript
// Lines 146–148
    .on("zoom", (event) => {
        containerG.attr("transform", event.transform);
    });
```

**`.on("zoom", callback)`** — **Built-in D3 method**. Registers a callback function to fire whenever the user scrolls their mouse wheel or drags the background while not dragging a node.

**`(event) =>`** — Our arrow function. `event` is our parameter name (we could call it `e` or anything else). D3 passes in a zoom event object automatically.

**`event.transform`** — This is a **built-in D3 property on the zoom event object**. It is a special D3 transform object that encodes the current `scale` (how zoomed in we are) and `translate` (how far we've panned). For example, after zooming in twice and panning right, it might represent `scale=2.5, translateX=100, translateY=50`.

**`containerG.attr("transform", event.transform)`** — We take D3's calculated transform and apply it to our master `<g class="zoom-container">` group. D3 automatically converts `event.transform` to an SVG transform string like `"translate(100, 50) scale(2.5)"`. The entire graph — all edges, all nodes — moves and scales together because they're all inside `containerG`.

---

```javascript
// Line 150
svg.call(zoomBehavior);
```

**`.call(behavior)`** — **Built-in D3 method**. Applies the behavior to the selection. Internally, this calls `zoomBehavior(svg)` and tells the zoom behavior to attach its mouse wheel and drag event listeners to the SVG element. Without this line, the zoom would be configured but never listening to any events.

---

## Part 3: Understanding Drag with Physics

Dragging a node is more complex than just moving it with the mouse, because we are fighting against the physics engine!

The physics engine runs 60 times per second, repositioning nodes based on forces. If you tried to drag a node without telling the physics engine, it would immediately snap the node back to wherever the forces want it.

We solve this with D3's special `fx` and `fy` properties:

**`d.fx`** (Fixed X) and **`d.fy`** (Fixed Y) are **D3-recognized properties** that you add to a node data object. When `d.fx` is set to a number, the physics engine **ignores all forces on that node for X positioning** and pins it exactly at that coordinate. When `d.fx = null`, the physics engine takes full control again.

---

## Part 4: Drag Functions — Line by Line (Lines 451–469)

The three drag functions (`dragstarted`, `dragged`, `dragended`) were registered in the `loadGraph()` function using:
```javascript
.call(d3.drag()
    .on("start", dragstarted)
    .on("drag", dragged)
    .on("end", dragended))
```

### `dragstarted` (Lines 451–455)

```javascript
function dragstarted(event, d) {
    if (!event.active && simulation) simulation.alphaTarget(0.3).restart();
    d.fx = d.x;
    d.fy = d.y;
}
```

**`dragstarted`** — **Our function** (we named it). D3 calls this the moment the user clicks and holds on a node.

**`event`** — The drag event object D3 passes in. **Built-in D3 parameter**.

**`d`** — The data object bound to the node being dragged. **Built-in D3 parameter** passed automatically (we named it `d`).

**`event.active`** — This is a **built-in D3 property on the drag event**. It counts how many concurrent drag gestures are happening. It is `0` (falsy) when this is the first drag to start (user just clicked). It would be `1+` if somehow multiple drags were active simultaneously (not common on desktop).

**`!event.active`** — The `!` is JavaScript "NOT". `!0 = true`. So this condition is true when this is the first drag.

**`simulation.alphaTarget(0.3)`** — **Built-in D3 method on the simulation**. Instead of letting alpha cool down to 0 (which would freeze the simulation), we tell it to maintain a minimum alpha of 0.3. This keeps the physics engine "warm" so other nodes react dynamically while we drag one node around.

**`.restart()`** — **Built-in D3 method**. If the simulation had already cooled and stopped, this wakes it up again. Combined with `alphaTarget(0.3)`, this ensures the simulation is actively running.

**`d.fx = d.x`** — We set the "Fixed X" to the node's current calculated position. From this moment, the physics engine locks this node at this pixel coordinate and ignores all forces on it.

**`d.fy = d.y`** — Same for Y.

After these two lines, the node is effectively "grabbed" — it won't move unless WE tell it to.

---

### `dragged` (Lines 457–460)

```javascript
function dragged(event, d) {
    d.fx = event.x;
    d.fy = event.y;
}
```

**`dragged`** — **Our function**. D3 fires this repeatedly as the mouse moves (every single mouse movement event while the button is held down).

**`event.x`** — This is a **built-in D3 property on the drag event**. The current mouse cursor's X position in SVG coordinates (accounting for any zoom/pan transforms that are already applied to the canvas).

**`event.y`** — The current mouse cursor's Y position in SVG coordinates.

**`d.fx = event.x`** — We continuously update the "Fixed X" to match the mouse position. Because the physics engine reads `d.fx` and pins the node there every tick, the node chases the mouse perfectly.

**`d.fy = event.y`** — Same for Y.

---

### `dragended` (Lines 462–469)

```javascript
function dragended(event, d) {
    if (!event.active && simulation) simulation.alphaTarget(0);
    const hasCoordinates = graphData.nodes.length > 0 && graphData.nodes[0].x !== undefined;
    if (!hasCoordinates) {
        d.fx = null;
        d.fy = null;
    }
}
```

**`dragended`** — **Our function**. D3 fires this once when the user releases the mouse button.

**`simulation.alphaTarget(0)`** — **Built-in D3 method**. We remove the minimum alpha constraint we set in `dragstarted`. Now alpha is free to cool down to 0 again naturally. The physics will settle and the simulation will sleep.

**`const hasCoordinates = ...`** — **Our logic**. We check if the graph came from our C++ road network loader (which has pre-set geographic coordinates). If it does have coordinates, the nodes should stay pinned at their geographic positions even after dragging (you can move them temporarily but they stay put). If no pre-set coordinates (a random graph), we release the node.

**`d.fx = null`** — Setting Fixed X to `null` releases the lock. D3 detects `null` and stops pinning the node. The physics engine takes back full control and gently settles the node into its equilibrium position based on the spring and charge forces.

**`d.fy = null`** — Same for Y.

---

## Part 5: The `fitGraphToScreen` Function (Lines 472–493)

This function is called once 200ms after loading a graph. It automatically zooms and pans so the entire graph is visible on screen.

```javascript
function fitGraphToScreen() {
    if (!graphData || graphData.nodes.length === 0) return;

    const bounds = containerG.node().getBBox();
```

**`containerG.node()`** — **Built-in D3 method**. `.node()` extracts the raw DOM element from a D3 selection. `containerG` is a D3 selection wrapping a `<g>` element. `.node()` gives us the actual DOM `<g>` object.

**`.getBBox()`** — This is a **built-in SVG DOM method** (not D3). It stands for "Get Bounding Box". It returns an object `{x, y, width, height}` describing the smallest rectangle that contains all the content inside this SVG group. After the physics runs, this tells us exactly how big our graph is and where it is on the canvas.

```javascript
    const fullWidth = parent.clientWidth;
    const fullHeight = parent.clientHeight;
```

**`.clientWidth`** and **`.clientHeight`** — **Built-in DOM properties**. Give us the pixel dimensions of the SVG element itself (the "window" we're looking through).

```javascript
    const scale = 0.85 / Math.max(width / fullWidth, height / fullHeight);
```

**`Math.max()`** — **Built-in JavaScript function**. Returns the larger of its arguments.

`width / fullWidth` is the graph's width as a fraction of the canvas width.
`height / fullHeight` is the graph's height as a fraction of the canvas height.

We take the LARGER ratio (the more constraining dimension) and divide `0.85` by it. The `0.85` leaves a 15% margin around the graph so it doesn't touch the edges.

```javascript
    svg.transition().duration(750).call(
        zoomBehavior.transform,
        d3.zoomIdentity.translate(translate[0], translate[1]).scale(scale)
    );
```

**`svg.transition()`** — **Built-in D3 method**. Tells D3 that the upcoming attribute changes should be animated smoothly rather than happening instantly.

**`.duration(750)`** — **Built-in D3 method**. The animation takes 750 milliseconds (0.75 seconds). This creates the smooth "zoom to fit" animation you see when loading a graph.

**`d3.zoomIdentity`** — **Built-in D3 constant**. Represents a "no transformation" starting point (scale=1, translate=0,0). It's a base object we can chain transformations onto.

**`.translate(x, y).scale(scale)`** — **Built-in D3 methods on the zoom identity**. We first translate by our calculated offset, then scale by our calculated factor, building the final transform that will center and size the graph.
