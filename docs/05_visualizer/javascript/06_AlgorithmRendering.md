# 06. Algorithm Frame Rendering
*Reference: `visualizer/graph.js` (Lines 689 - 830)*

This document provides a **line-by-line** breakdown of the final step of the animation loop: translating the C++ JSON output into stunning visual colors using CSS classes.

---

## 1. The Rendering Switchboard (Lines 689 - 710)
```javascript
/* ============================================================================
   FRAME RENDER LOGIC BY ALGORITHM
   
   The Golden Rule of Web Styling: JavaScript does not manipulate colors directly.
   JavaScript simply adds or removes CSS classes (like '.visited' or '.path-node')
   and the CSS engine instantly applies the predefined colors/glows to the elements.
   ============================================================================ */
function updateSimulationFrame() {
    resetVisualStates();
```
**Explanation (Lines 689 - 697):**
- Every time the animation playhead advances, we first call `resetVisualStates()` to completely erase all colors and classes from the screen. This gives us a blank slate for the new frame.

```javascript
    if (activeAlgorithm === "bfs" || activeAlgorithm === "dfs") {
        renderBFSDFSFrame();
    } else if (activeAlgorithm === "dijkstra" || activeAlgorithm === "astar") {
        renderPathfindingFrame();
    } else if (activeAlgorithm === "kruskal") {
        renderKruskalFrame();
    } else if (activeAlgorithm === "tarjan") {
        renderTarjanFrame();
    }
}
```
**Explanation (Lines 699 - 710):**
- A simple routing function. Depending on the `activeAlgorithm` string, it calls a specialized render function that knows how to read that specific algorithm's JSON format.

---

## 2. Rendering BFS & DFS (Lines 712 - 733)
```javascript
function renderBFSDFSFrame() {
    const history = algorithmData[activeAlgorithm].steps.slice(0, currentStep);
    
    // Nodes
    svg.selectAll(".node-circle")
        .classed("visited", d => history.includes(d.id))
        .classed("current", d => history.length > 0 && d.id === history[history.length - 1]);
```
**Explanation (Lines 712 - 718):**
- `slice(0, currentStep)` grabs a sub-array of the history from the beginning up to the current frame. E.g., if we are on step 3, `history` is `["A", "B", "C"]`.
- `svg.selectAll(".node-circle")` grabs every single circle on the screen.
- `.classed("visited", boolean)` loops over every circle (`d`). If the `history` array includes this node's `id` (e.g., `"A"`), it adds the CSS class `"visited"`.
- `.classed("current", boolean)` checks if this node is the *very last* node in the history array (`history[history.length - 1]`). If so, it adds the CSS class `"current"`.

**Corresponding CSS (style.css):**
```css
/* Note how CSS applies beautiful colors the instant JavaScript adds these classes! */
.node-circle.visited {
    fill: var(--primary); /* Deep Blue */
    stroke: var(--primary-glow);
}
.node-circle.current {
    fill: var(--accent); /* Vibrant Pink */
    filter: drop-shadow(0 0 12px var(--accent)); /* Glowing effect */
}
```

```javascript
    // Links (edges to visited nodes)
    svg.selectAll(".graph-link")
        .classed("visited-edge", d => history.includes(d.source.id) && history.includes(d.target.id));
}
```
**Explanation (Lines 720 - 724):**
- Selects all edges.
- If *both* the source node and target node of the edge are in the `history` array, it colors the edge blue by adding `"visited-edge"`.

---

## 3. Rendering Pathfinding (Dijkstra/A*) (Lines 735 - 755)
```javascript
function renderPathfindingFrame() {
    const pathNodes = algorithmData[activeAlgorithm].steps.slice(0, currentStep);
    
    svg.selectAll(".node-circle")
        .classed("path-node", d => pathNodes.includes(d.id));
```
**Explanation (Lines 735 - 740):**
- Exactly like BFS, we slice the array up to `currentStep`.
- But this time, we add the class `"path-node"` to the circles in the shortest path.

```javascript
    svg.selectAll(".graph-link")
        .classed("path-edge", d => {
            const sourceIdx = pathNodes.indexOf(d.source.id);
            const targetIdx = pathNodes.indexOf(d.target.id);
            return sourceIdx !== -1 && targetIdx !== -1 && Math.abs(sourceIdx - targetIdx) === 1;
        })
```
**Explanation (Lines 742 - 747):**
- An edge is only part of the shortest path if:
  1. The source node is in the array (`!== -1`).
  2. The target node is in the array.
  3. They are exactly adjacent to each other in the array (`Math.abs(sourceIdx - targetIdx) === 1`). This prevents coloring an edge between the first and last node if they happen to connect!

```javascript
        .attr("marker-end", function(d) {
            const isPath = d3.select(this).classed("path-edge");
            return isPath ? "url(#arrowhead-path)" : "url(#arrowhead)";
        });
}
```
**Explanation (Lines 748 - 755):**
- If an edge was just classified as `"path-edge"`, we must also change the triangle arrowhead at the end of it!
- It dynamically swaps the `marker-end` attribute from `#arrowhead` (the gray one from Section 1) to `#arrowhead-path` (the pink one).

---

## 4. Rendering Kruskal's MST (Lines 757 - 785)
```javascript
function renderKruskalFrame() {
    const mstEdges = algorithmData.kruskal.mst.slice(0, currentStep);
    
    svg.selectAll(".graph-link")
        .classed("mst-edge", d => {
            return mstEdges.some(edge => 
                (edge.u === d.source.id && edge.v === d.target.id) ||
                (edge.u === d.target.id && edge.v === d.source.id)
            );
        });
```
**Explanation (Lines 757 - 766):**
- Kruskal's JSON array `mst` is an array of edge objects `{u: "A", v: "B"}`.
- We slice the array up to `currentStep`.
- We loop through all SVG edges. `.some()` checks if this SVG edge matches *any* edge in the `mstEdges` array. We check both directions (`u->v` and `v->u`) because MST is an undirected graph concept.
- If it matches, we add the `"mst-edge"` CSS class, which colors the line green.

```javascript
    // Highlight nodes that are part of the current MST
    svg.selectAll(".node-circle")
        .classed("mst-node", d => {
            return mstEdges.some(edge => edge.u === d.id || edge.v === d.id);
        });
}
```
**Explanation (Lines 768 - 775):**
- Selects all circles. If the circle's ID is involved in *any* of the edges currently in the MST, we color the circle green (`"mst-node"`).

---

## 5. Rendering Tarjan's SCC (Lines 787 - 830)
```javascript
function renderTarjanFrame() {
    const sccs = algorithmData.tarjan.sccs.slice(0, currentStep);
    
    // We use a predefined set of vibrant colors for different components
    const colors = ["#EF4444", "#3B82F6", "#10B981", "#F59E0B", "#8B5CF6", "#EC4899", "#06B6D4"];
```
**Explanation (Lines 787 - 792):**
- Tarjan's JSON array `sccs` is an array of arrays (e.g., `[["A", "B", "C"], ["D"]]`). Each sub-array is a Strongly Connected Component.
- We define a hardcoded array of vibrant hex colors. The first component gets Red, the second Blue, etc.

```javascript
    svg.selectAll(".node-circle")
        .attr("style", d => {
            for (let i = 0; i < sccs.length; i++) {
                if (sccs[i].includes(d.id)) {
                    const color = colors[i % colors.length];
                    return `fill: ${color}; stroke: ${color}; filter: drop-shadow(0 0 8px ${color}80);`;
                }
            }
            return null;
        });
```
**Explanation (Lines 794 - 804):**
- **Exception to the Rule:** Here we *do* use JavaScript to manipulate styles directly (`attr("style", ...)`). We have to do this because we don't know in advance how many SCCs the graph will have, so we can't write CSS classes for all of them!
- For each circle `d`, we loop through the `sccs` array. If the node is in component `i`, we pick `colors[i]`. (Using modulo `%` ensures we cycle back to Red if there are more than 7 components).
- We return a raw CSS string injecting the color into `fill`, `stroke`, and `filter`. The `80` at the end of the hex code (`${color}80`) adds 50% transparency to the drop-shadow.

```javascript
    svg.selectAll(".graph-link")
        .attr("style", d => {
            for (let i = 0; i < sccs.length; i++) {
                if (sccs[i].includes(d.source.id) && sccs[i].includes(d.target.id)) {
                    return `stroke: ${colors[i % colors.length]}; stroke-opacity: 1; stroke-width: 3px;`;
                }
            }
            return null;
        });
}
```
**Explanation (Lines 806 - 816):**
- Selects all edges.
- If an edge connects two nodes that are in the *same* component `i` (meaning both source and target are inside `sccs[i]`), it receives the same color string as the nodes.
- Edges that cross between different components get ignored (returning `null`), leaving them as default dark gray!
