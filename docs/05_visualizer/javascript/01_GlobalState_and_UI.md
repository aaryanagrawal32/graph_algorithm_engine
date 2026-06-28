# 01. Global State and UI Initialization
*Reference: `visualizer/graph.js` (Lines 1 - 141)*

This document provides a **line-by-line** breakdown of the first 141 lines of `graph.js`. We will explain exactly what the JavaScript does, and immediately show the HTML and CSS that it connects to.

---

## 1. Block Header (Lines 1 - 4)
```javascript
/* ============================================================================
   GRAPH ENGINE VISUALIZER - D3.js ANIMATION CORE
   
   This file contains all the JavaScript logic for the visualizer.
   It handles reading the JSON files, running the D3 physics engine,
   and animating the algorithms using CSS class toggling.
   ============================================================================ */
```
**Explanation:** This is a standard CSS/JS block comment `/* ... */` used to organize code into readable sections.

---

## 2. Global State Declarations (Lines 5 - 22)
```javascript
// Global State
// We use 'let' instead of C++ types like 'int' or 'string'. These variables 
// are accessible globally so all functions can read/modify them.
let graphData = null;       // Holds { nodes: [...], links: [...] }
```
**Explanation (Line 9):** `let` is the modern JavaScript keyword for declaring a variable that can change. `null` is a special value meaning "empty". Here, `graphData` will eventually store the entire parsed JSON graph (vertices and edges).

```javascript
let algorithmData = {
    // This object acts like a C++ struct or std::unordered_map
    bfs: null,             // { steps: [...] }
    dfs: null,             // { steps: [...] }
    dijkstra: null,        // { steps: [...] } (shortest path nodes)
    astar: null,           // { steps: [...] } (shortest path nodes)
    kruskal: null,         // { mst: [...] } (list of edges)
    tarjan: null           // { sccs: [...] } (list of component lists)
};
```
**Explanation (Lines 10 - 18):** This creates a JavaScript **Object Literal** using `{}`. An object is a collection of key-value pairs (like a `std::map<string, void*>`). 
- `bfs: null` creates a key named `bfs` initialized to `null`.
- Later, when we read `bfs_steps.json`, we will store it directly into `algorithmData.bfs` so we can look it up instantly without searching.

```javascript
// Simulation State
let activeAlgorithm = "";
let currentStep = 0;     // Acts as our playhead frame counter
let maxSteps = 0;
let isPlaying = false;
let playbackInterval = null;
let playbackSpeed = 500; // ms per step
```
**Explanation (Lines 20 - 26):**
- `activeAlgorithm = ""` initializes to an empty string. This will hold the name of the currently selected algorithm (e.g., `"dijkstra"`).
- `currentStep = 0` tracks the current animation frame (like a playhead on a YouTube video).
- `maxSteps = 0` stores the total number of frames in the current algorithm so we know when to stop.
- `isPlaying = false` is a boolean flag checking if the animation timer is running.
- `playbackInterval = null` will hold the unique ID of the JavaScript timer (so we can destroy it when we hit pause).
- `playbackSpeed = 500` is the default delay between frames (500 milliseconds = 0.5 seconds).

---

## 3. D3 Selection Variables (Lines 28 - 36)
```javascript
// D3 Selections & Simulation
let svg, containerG, linkG, nodeG;
let simulation = null;
let nodeSelection = null;
let linkSelection = null;
let labelSelection = null;
let weightSelection = null;
let zoomBehavior = null;
let showLabels = true;
```
**Explanation (Lines 28 - 36):**
- `let svg, containerG...` declares multiple variables on one line, all initially `undefined`.
- These variables will hold **D3 Selections** (references to HTML elements on the page, like pointers in C++). We define them globally so the physics engine and rendering loops can both access them.
- `showLabels = true` is a toggle flag for whether node IDs (text) are visible.

---

## 4. File Upload Tracker (Lines 38 - 47)
```javascript
// File Upload Tracker
const loadedFilesStatus = {
    graph: false,
    bfs: false,
    dfs: false,
    dijkstra: false,
    astar: false,
    kruskal: false,
    tarjan: false
};
```
**Explanation (Lines 38 - 47):**
- `const` declares a variable that cannot be reassigned. Note: we cannot reassign the variable `loadedFilesStatus` to point to a *new* object, but we CAN modify the booleans inside it! 
- This object tracks which JSON files have been successfully loaded into memory so we can enable/disable buttons accordingly.

---

## 5. The Application Bootloader (Lines 49 - 59)
```javascript
/* ============================================================================
   INITIALIZATION & DOM EVENTS
   
   In web development, we don't have a blocking main() function. Instead, we
   use an Event-Driven model. We tell the browser to wait until the HTML is fully
   loaded (DOMContentLoaded), and then we attach event listeners to buttons.
   ============================================================================ */
document.addEventListener("DOMContentLoaded", () => {
    setupUI();
    setupSVG();
    setupDragAndDrop();
    
    // Attempt auto-loading from server (fails gracefully if opened via file://)
    tryAutoLoadFromServer();
});
```
**Explanation:** 
- `document` is a built-in global object representing the entire HTML page.
- `.addEventListener("DOMContentLoaded", ...)` tells the browser: "Do not execute the code inside here until every HTML tag has been fully parsed and drawn on the screen." If you try to attach a button click event before the button exists in HTML, the code will crash!
- `() => { ... }` is an Arrow Function (a modern JavaScript lambda). It acts as the callback function that runs once loading finishes.
- Inside the lambda, it calls 4 initialization functions to boot up the app.

---

## 6. Wiring up the UI (Lines 61 - 103)
```javascript
function setupUI() {
    // Dropdown events
    const algoSelect = document.getElementById("algorithm-select");
    algoSelect.addEventListener("change", (e) => {
        setAlgorithm(e.target.value);
    });
```
**Explanation (Lines 61 - 66):**
- `document.getElementById("algorithm-select")` searches the HTML document for an element with `id="algorithm-select"`.

**Corresponding HTML (index.html):**
```html
<select id="algorithm-select" disabled>
    <option value="" disabled selected>-- Choose an Algorithm --</option>
    <option value="bfs">Breadth-First Search (BFS)</option>
    <!-- ... -->
</select>
```
**Corresponding CSS (style.css):**
```css
select {
    background: var(--bg-panel-solid); /* Dark gray background */
    border: 1px solid var(--border-glass); /* Thin translucent border */
    /* ... padding, rounded corners, font-size ... */
}
```
- `.addEventListener("change", (e) => { ... })` attaches a listener that fires *only* when the user selects a new option from the dropdown. 
- `e` is the Event object passed by the browser. `e.target` is the HTML `<select>` element itself. `e.target.value` gets the internal value of the clicked option (e.g., `"bfs"`). It passes this string to the `setAlgorithm()` function.

```javascript
    // Control Buttons
    document.getElementById("btn-play").addEventListener("click", togglePlay);
    document.getElementById("btn-next").addEventListener("click", stepForward);
    document.getElementById("btn-prev").addEventListener("click", stepBackward);
    document.getElementById("btn-reset").addEventListener("click", resetSimulation);
```
**Explanation (Lines 68 - 72):**
Finds the HTML media buttons by their IDs (`btn-play`, `btn-next`, etc.) and attaches `"click"` listeners. 

**Corresponding HTML (index.html):**
```html
<button id="btn-play" class="control-btn play" disabled>▶</button>
```
**Corresponding CSS (style.css):**
```css
.control-btn {
    background: rgba(255, 255, 255, 0.05); /* Translucent glass */
    transition: all 0.2s cubic-bezier(...); /* Smooth hover effects */
}
.control-btn:hover:not(:disabled) {
    background: rgba(255, 255, 255, 0.1); /* Gets slightly brighter on hover */
}
```
Notice the CSS `:not(:disabled)` pseudo-class. Because the HTML button currently has the `disabled` attribute, this CSS hover effect will *not* trigger!

```javascript
    // Speed Slider
    const speedSlider = document.getElementById("speed-slider");
    const speedVal = document.getElementById("speed-value");
    speedSlider.addEventListener("input", (e) => {
        playbackSpeed = parseInt(e.target.value);
        speedVal.innerText = `${(1000 / playbackSpeed).toFixed(1)}x`;
        if (isPlaying) {
            pause();
            play();
        }
    });
```
**Explanation (Lines 74 - 84):**
- Grabs the `<input type="range">` slider and the `<span>` that displays the text (e.g., "1.0x").
- `"input"` event fires continuously *as you drag* the slider.
- `e.target.value` grabs the raw number from the slider (e.g., `"500"`). `parseInt` converts the string to an integer and updates our global `playbackSpeed` variable.
- `` `${...}` `` is a Template Literal. It evaluates the math `(1000 / 500)`, formats it to 1 decimal place (`.toFixed(1)`), and injects it into a string to get `"2.0x"`.
- `speedVal.innerText = ...` instantly replaces the text inside the HTML `<span>`.
- `if (isPlaying) { pause(); play(); }` restarts the animation timer immediately so the speed change takes effect without requiring the user to manually stop and restart it.

```javascript
    // Zoom Buttons
    document.getElementById("btn-zoom-in").addEventListener("click", () => {
        svg.transition().duration(300).call(zoomBehavior.scaleBy, 1.3);
    });
    document.getElementById("btn-zoom-out").addEventListener("click", () => {
        svg.transition().duration(300).call(zoomBehavior.scaleBy, 1/1.3);
    });
    document.getElementById("btn-zoom-fit").addEventListener("click", fitGraphToScreen);
```
**Explanation (Lines 86 - 93):**
- Hooks up the zoom buttons (+, -, and fit). 
- `svg.transition().duration(300)` tells D3.js to animate the next command over 300 milliseconds.
- `.call(zoomBehavior.scaleBy, 1.3)` commands the D3 zoom handler to multiply the current camera scale by 1.3 (zooming in by 30%).

```javascript
    // Toggle Labels
    const btnLabels = document.getElementById("btn-toggle-labels");
    btnLabels.addEventListener("click", () => {
        showLabels = !showLabels;
        btnLabels.classList.toggle("active", showLabels);
        svg.selectAll(".node-label").style("opacity", showLabels ? 1 : 0);
    });
}
```
**Explanation (Lines 95 - 103):**
- Hooks up the 🏷 label toggle button.
- `showLabels = !showLabels` flips the global boolean (True -> False -> True).
- `btnLabels.classList.toggle("active", showLabels)` adds the CSS class `"active"` to the button if `showLabels` is true, and removes it if false.

**Corresponding HTML (index.html):**
```html
<button id="btn-toggle-labels" class="canvas-btn active" title="Toggle Labels">🏷</button>
```

**Corresponding CSS (style.css) for the button:**
```css
.canvas-btn.active {
    border-color: var(--primary); /* Turns the border blue */
    color: var(--primary); /* Turns the text blue */
    box-shadow: 0 0 10px var(--primary-glow); /* Adds a blue glowing drop-shadow */
}
```
This is the power of CSS: JavaScript only flips one word (`"active"`), and CSS handles all the glowing aesthetics!
- `svg.selectAll(".node-label").style("opacity", ...)` selects all text elements on the canvas and changes their CSS `opacity` property to 1 (fully visible) or 0 (completely invisible) using a ternary operator (`? 1 : 0`).

---

## 7. Setting up the Canvas (Lines 104 - 141)
```javascript
function setupSVG() {
    svg = d3.select("#graph-svg");
    containerG = svg.append("g").attr("class", "zoom-container");
```
**Explanation (Lines 104 - 106):**
- `d3.select("#graph-svg")` is D3's version of `getElementById`. We store it in the global `svg` variable.
- `.append("g")` creates a brand new HTML `<g>` tag (Group) inside the SVG. A `<g>` tag is like a folder. If you move or scale the `<g>` folder, everything inside it moves together (which is how we implement the panning camera!).
- `.attr("class", "zoom-container")` adds a CSS class to the group.

**Corresponding HTML Generated in Memory:**
```html
<svg id="graph-svg">
    <g class="zoom-container"></g>
</svg>
```

```javascript
    // Define markers for directed edge arrowheads
    svg.append("defs").append("marker")
        .attr("id", "arrowhead")
        .attr("viewBox", "0 -5 10 10")
        .attr("refX", 22) // Place arrowhead just at the border of node circle (radius 15 + padding)
        .attr("refY", 0)
        .attr("markerWidth", 6)
        .attr("markerHeight", 6)
        .attr("orient", "auto")
        .append("path")
        .attr("d", "M0,-4L9,0L0,4")
        .attr("class", "arrowhead");
```
**Explanation (Lines 108 - 119):**
- `.append("defs")` creates an HTML `<defs>` (Definitions) tag used for defining reusable assets like gradients or markers.
- `.append("marker")` creates an HTML `<marker>` tag, giving it `id="arrowhead"`.
- `viewBox`, `refX`, `markerWidth`, etc., are standard SVG attributes that define the coordinate system, size, and anchor point of the arrowhead so it sits perfectly on the edge of the circle (which is 15px wide + 7px padding = 22px).
- `.append("path")` creates the actual triangle shape using the `d` attribute which uses a specialized math string `"M0,-4L9,0L0,4"` (Move to 0,-4. Line to 9,0. Line to 0,4).
- It receives `class="arrowhead"`.

**Corresponding CSS (style.css):**
```css
.arrowhead {
    fill: var(--edge-stroke); /* Dark gray color */
    fill-opacity: 0.6;
    transition: fill 0.3s ease; /* Smooth color transitions */
}
```

*(Note: Lines 121 - 131 repeat this exact same process to create a second, identical marker named `arrowhead-path`. The only difference is it receives `class="arrowhead path-arrow"`, which allows us to color it pink later in CSS for shortest paths).*

```javascript
    linkG = containerG.append("g").attr("class", "links-group");
    nodeG = containerG.append("g").attr("class", "nodes-group");
```
**Explanation (Lines 133 - 134):**
- Inside our main camera folder (`containerG`), we create two sub-folders: one for links (lines) and one for nodes (circles). 
- We add the `links-group` **first** so it renders below the `nodes-group`. In SVG, elements are drawn in order. If lines were drawn after circles, the lines would overlap on top of the circles!

```javascript
    // Setup zoom & pan
    zoomBehavior = d3.zoom()
        .scaleExtent([0.1, 4])
        .on("zoom", (event) => {
            containerG.attr("transform", event.transform);
        });

    svg.call(zoomBehavior);
}
```
**Explanation (Lines 136 - 141):**
- `d3.zoom()` initializes D3's built-in pan/zoom mouse listener.
- `.scaleExtent([0.1, 4])` restricts zooming out to 10% size, and zooming in to 400% size.
- `.on("zoom", (event) => { ... })` fires constantly as you drag the mouse or use the scroll wheel.
- `event.transform` contains the math matrix for how far you scrolled or panned. 
- `containerG.attr("transform", event.transform)` takes that math and applies it to the HTML `<g>` tag we created earlier. Because all nodes and lines are inside this group folder, the entire map moves!
- `svg.call(zoomBehavior)` actually binds these mouse listeners to the main `<svg>` canvas on the screen.
