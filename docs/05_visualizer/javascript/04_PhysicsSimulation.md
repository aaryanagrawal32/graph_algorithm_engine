# 04. The D3 Physics Engine
*Reference: `visualizer/graph.js` (Lines 334 - 490)*

This document provides a **line-by-line** breakdown of how the D3.js physics engine takes the raw graph data and generates actual HTML `<svg>` elements on the screen.

---

## 1. Engine Reset & Scaling (Lines 334 - 371)
```javascript
/* ============================================================================
   D3 FORCE DIRECTED SIMULATION
   
   If the JSON graph nodes don't have explicit X/Y coordinates, we use a 
   physics engine to automatically push them apart and draw them beautifully.
   This section handles binding our JavaScript data to HTML <svg> elements.
   ============================================================================ */
function loadGraph(data) {
    graphData = data;
```
**Explanation (Lines 334 - 339):**
- Stores the newly loaded data into the global `graphData` variable.

```javascript
    // Stop previous simulation
    if (simulation) simulation.stop();

    // Clear svg elements
    linkG.selectAll("*").remove();
    nodeG.selectAll("*").remove();
```
**Explanation (Lines 341 - 346):**
- If a physics simulation is already running from a previous file upload, `simulation.stop()` kills the old timer.
- `linkG.selectAll("*").remove()` deletes all HTML elements inside the `<g class="links-group">` folder, clearing the canvas.

```javascript
    // If nodes have coordinate coordinates, scale them to fit nicely on the canvas
    const hasCoordinates = graphData.nodes.length > 0 && graphData.nodes[0].x !== undefined;
    
    if (hasCoordinates) {
        // Find min/max coordinate ranges
        const minX = d3.min(graphData.nodes, d => d.x);
        const maxX = d3.max(graphData.nodes, d => d.x);
        const minY = d3.min(graphData.nodes, d => d.y);
        const maxY = d3.max(graphData.nodes, d => d.y);
```
**Explanation (Lines 348 - 356):**
- Checks if the C++ engine provided hardcoded X/Y coordinates.
- Uses `d3.min` and `d3.max` to find the bounding box of all nodes.

```javascript
        const svgBounds = document.getElementById("graph-svg").getBoundingClientRect();
        const padding = 120;
        const width = svgBounds.width - padding * 2;
        const height = svgBounds.height - padding * 2;

        const scaleX = width / (maxX - minX || 1);
        const scaleY = height / (maxY - minY || 1);
        
        // Scale and map nodes
        graphData.nodes.forEach(node => {
            node.fx = padding + (node.x - minX) * scaleX;
            node.fy = padding + (node.y - minY) * scaleY;
        });
    }
```
**Explanation (Lines 358 - 371):**
- `.getBoundingClientRect()` returns the actual pixel width and height of the `<svg id="graph-svg">` element on the screen.
- Calculates a scaling factor to stretch the graph to fit the screen.
- Iterates through the nodes, calculating their new scaled positions.
- Assigning values to `node.fx` and `node.fy` (fixed-x, fixed-y) tells D3.js to lock these nodes in place and *not* apply physics to them!

---

## 2. Initializing the Physics Math (Lines 373 - 381)
```javascript
    // Initialize Force layout
    simulation = d3.forceSimulation(graphData.nodes)
        .force("link", d3.forceLink(graphData.links).id(d => d.id).distance(120))
        .force("charge", d3.forceManyBody().strength(hasCoordinates ? -50 : -350))
        .force("collide", d3.forceCollide().radius(25))
        .force("center", d3.forceCenter(
            document.getElementById("graph-svg").clientWidth / 2,
            document.getElementById("graph-svg").clientHeight / 2
        ));
```
**Explanation (Lines 373 - 381):**
This is the core math of the visualizer.
1. `d3.forceSimulation` gives every node a mathematical `x` and `y` variable.
2. `forceLink` acts like **springs**. Every edge tries to pull its two nodes exactly 120 pixels apart.
3. `forceManyBody` acts like **magnets**. A negative strength (-350) means every node repels every other node, preventing them from clumping together.
4. `forceCollide` adds a physical hitbox (radius 25) so nodes can never overlap.
5. `forceCenter` pulls the entire cluster into the middle of the `<svg>` canvas so it doesn't float off-screen.

---

## 3. Data Binding & HTML Generation (Lines 383 - 421)
Now that the physics engine is calculating math in RAM, we need to actually draw HTML elements on the screen.

```javascript
    // Render Links
    linkSelection = linkG.selectAll(".graph-link")
        .data(graphData.links)
        .enter().append("line")
        .attr("class", "graph-link")
        .attr("stroke-width", 2)
        .attr("marker-end", "url(#arrowhead)"); // Use arrowhead marker
```
**Explanation (Lines 383 - 389):**
**This is the famous D3 Data-Binding Pattern:**
1. `selectAll(".graph-link")` — Select all lines currently on the screen (which is 0, since we cleared it).
2. `.data(graphData.links)` — Bind our array of edges to this empty selection.
3. `.enter()` — Since we have 0 lines but N edges, D3 loops N times for the "entering" data.
4. `.append("line")` — Create an actual HTML `<line>` tag for each edge.
5. `.attr(...)` — Apply the CSS class `.graph-link` and the `marker-end` attribute (which attaches the triangle arrowhead we built in Section 1!).

**Corresponding HTML Generated in Memory:**
```html
<line class="graph-link" stroke-width="2" marker-end="url(#arrowhead)"></line>
```
**Corresponding CSS (style.css):**
```css
.graph-link {
    stroke: #334155; /* Dark gray color */
    stroke-opacity: 0.6;
    transition: stroke 0.3s ease; /* Smoothly changes color when animated later! */
}
```

```javascript
    // Render Link Weights
    weightSelection = linkG.selectAll(".link-weight")
        .data(graphData.links)
        .enter().append("text")
        .attr("class", "link-weight")
        .attr("fill", "#64748B")
        .attr("font-size", "9px")
        .attr("font-weight", "500")
        .attr("text-anchor", "middle")
        .text(d => d.weight);
```
**Explanation (Lines 391 - 400):**
- Exactly the same pattern, but appending `<text>` tags to display the edge weights. `d => d.weight` extracts the weight number from the C++ JSON and puts it inside the HTML tags.

```javascript
    // Render Nodes
    nodeSelection = nodeG.selectAll(".node-group")
        .data(graphData.nodes)
        .enter().append("g")
        .attr("class", "node-group")
        .call(d3.drag()
            .on("start", dragstarted)
            .on("drag", dragged)
            .on("end", dragended));

    nodeSelection.append("circle")
        .attr("class", "node-circle")
        .attr("r", 15);
```
**Explanation (Lines 402 - 414):**
- Appends a `<g class="node-group">` folder for each node.
- `.call(d3.drag()...)` attaches mouse drag event listeners so the user can grab nodes and throw them around!
- Finally, it appends a `<circle radius="15">` *inside* the folder.

**Corresponding HTML Generated:**
```html
<g class="node-group">
    <circle class="node-circle" r="15"></circle>
</g>
```
**Corresponding CSS (style.css):**
```css
.node-circle {
    fill: #1E293B; /* Dark inner color */
    stroke: #475569; /* Border color */
    stroke-width: 2px;
}
.node-group:hover .node-circle {
    stroke: #6366F1; /* Border turns blue when hovered */
    filter: drop-shadow(0 0 8px rgba(99,102,241,0.4)); /* Adds a glow */
}
```

```javascript
    // Render Node Labels (IDs)
    labelSelection = nodeSelection.append("text")
        .attr("class", "node-label")
        .attr("text-anchor", "middle")
        .attr("dy", ".3em")
        .text(d => d.id);
```
**Explanation (Lines 416 - 421):**
- Appends `<text>` inside the node folder to display the Node ID. `dy=".3em"` nudges the text down slightly so it is perfectly centered vertically inside the circle.

---

## 4. The Animation Loop (Lines 423 - 442)
```javascript
    // Bind tick event
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

    // Run simulation a bit and fit to screen
    setTimeout(() => {
        fitGraphToScreen();
    }, 200);
}
```
**Explanation (Lines 423 - 442):**
- The physics engine calculates math at 60 frames per second. On every frame, it fires a `"tick"` event.
- Inside the event, we take the mathematical variables in RAM (`d.source.x`) and inject them directly into the HTML tag properties (`x1`, `y1`). This forces the HTML elements to move on the screen, creating smooth animation!
- For nodes, we use the `transform="translate(x, y)"` property, which moves the entire `<g>` folder, carrying the circle and the text label together.
- `setTimeout(..., 200)` waits 200 milliseconds for the physics to settle, then calls `fitGraphToScreen()` to center the camera.

---

## 5. Zoom & Drag Helpers (Lines 444 - 490)
```javascript
/* ============================================================================
   ZOOM & DRAG HELPERS
   ============================================================================ */
function dragstarted(event, d) {
    if (!event.active && simulation) simulation.alphaTarget(0.3).restart();
    d.fx = d.x;
    d.fy = d.y;
}
```
**Explanation (Lines 444 - 451):**
- Fired when a user clicks a node.
- `simulation.alphaTarget(0.3).restart()` "wakes up" the physics engine (which goes to sleep when nodes stop moving to save CPU).
- `d.fx = d.x` locks the node to the exact coordinates of the mouse cursor, ripping it away from the physics engine's control.

```javascript
function dragged(event, d) {
    d.fx = event.x;
    d.fy = event.y;
}
```
**Explanation (Lines 453 - 456):**
- Fired as the mouse moves. It constantly updates the locked `fx/fy` variables so the node follows the cursor.

```javascript
function dragended(event, d) {
    if (!event.active && simulation) simulation.alphaTarget(0);
    // If graph has fixed coordinates, keep it pinned, otherwise release
    const hasCoordinates = graphData.nodes.length > 0 && graphData.nodes[0].x !== undefined;
    if (!hasCoordinates) {
        d.fx = null;
        d.fy = null;
    }
}
```
**Explanation (Lines 458 - 466):**
- Fired when the user lets go of the mouse.
- Sets `d.fx = null`, giving control of the node back to the physics engine so it snaps back into place elastically.

```javascript
function fitGraphToScreen() {
    if (!graphData || graphData.nodes.length === 0) return;

    const bounds = containerG.node().getBBox();
    const parent = svg.node();
    const fullWidth = parent.clientWidth;
    const fullHeight = parent.clientHeight;
    
    const width = bounds.width;
    const height = bounds.height;
    const midX = bounds.x + width / 2;
    const midY = bounds.y + height / 2;
    
    if (width === 0 || height === 0) return; 
    
    const scale = 0.85 / Math.max(width / fullWidth, height / fullHeight);
    const translate = [fullWidth / 2 - scale * midX, fullHeight / 2 - scale * midY];

    svg.transition().duration(750).call(
        zoomBehavior.transform,
        d3.zoomIdentity.translate(translate[0], translate[1]).scale(scale)
    );
}
```
**Explanation (Lines 468 - 490):**
- `.getBBox()` calculates the "Bounding Box" (the absolute width and height encompassing all nodes).
- It calculates a `scale` factor to shrink or grow the bounding box so it takes up 85% (`0.85`) of the screen.
- It calculates a `translate` coordinate to move the center of the bounding box to the absolute center of the screen.
- `svg.transition().duration(750)` smoothly animates the camera moving to these new coordinates over 0.75 seconds.
