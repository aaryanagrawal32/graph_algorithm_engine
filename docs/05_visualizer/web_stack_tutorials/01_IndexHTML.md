# index.html — The Skeleton of the Web Page
### A Beginner-Friendly Line-by-Line Guide

> **Who is this for?**
> This guide is for someone who understands C++ but has never written an HTML file before. We will explain how the visualizer's user interface is structured.

---

## What Is This File?

`index.html` is the **skeleton** of the visualizer web app. It defines all the elements on the screen: the buttons, the sidebar, the drop zone, and the empty canvas where the graph will be drawn. 

It does **not** contain any logic (that's `graph.js`) or any visual styling (that's `style.css`). It just says "put a button here, put a dropdown here."

---

## Part 0: HTML Building Blocks

Before looking at the code, you need to understand HTML syntax. HTML is built using **tags**.

A tag looks like this:
```html
<button id="btn-play" class="control-btn">Play</button>
```

- `<button>` is the **opening tag**. It tells the browser "create a button here."
- `</button>` is the **closing tag**. It marks the end of the button.
- `Play` is the **content**. It is the text displayed on the button.
- `id="btn-play"` is an **attribute**. An ID is a unique name for this exact element. `graph.js` uses this ID to find the button and attach C++ like logic to it.
- `class="control-btn"` is another attribute. A class is a group label. `style.css` uses this class to say "make all elements with the class 'control-btn' look nice and rounded."

---

## Part 1: The Code Walkthrough

### The Head: Setting Up

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Graph Algorithm Engine Visualizer</title>
```
The `<head>` section contains metadata that is **invisible** on the actual page. It sets the title shown in the browser tab and ensures the layout scales correctly on different screens (`viewport`).

```html
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&family=Fira+Code:wght@400;500&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="style.css">
```
These lines link external resources:
1. `preconnect` hints speed up loading.
2. Downloads the "Inter" font for the UI and "Fira Code" for the monospaced terminal logs.
3. Links our `style.css` file, telling the browser to apply all our custom colors and layout rules.

```html
    <script src="https://d3js.org/d3.v7.min.js"></script>
</head>
```
Downloads the **D3.js library** from the internet. D3 is a powerful data visualization library that we use in `graph.js` to draw the circles and lines of our graph.

---

### The Body: The Visible Application

```html
<body>
    <div class="app-container">
```
The `<body>` contains everything you actually see on screen.
A `<div>` is a generic rectangular container (like a `std::vector` but for visual elements). It groups things together.

#### The Sidebar (Control Panel)

```html
        <aside class="control-panel">
            <header class="panel-header">
                <div class="logo-container">
                    <div class="logo-glow"></div>
                    <span class="logo-icon">⬢</span>
                    <h1>Graph Engine</h1>
                </div>
                <p class="subtitle">Interactive Algorithm Visualizer</p>
            </header>
```
An `<aside>` is a `<div>` specifically meant for sidebars. Inside it, we have a `<header>` with our logo, title (`<h1>`), and subtitle (`<p>`).

#### File Upload & Status

```html
            <section class="panel-section" id="upload-section">
                <h2 class="section-title">Data Source</h2>
                <div class="upload-zone" id="drop-zone">
                    <span class="upload-icon">✦</span>
                    <p class="upload-text">Drag & drop <strong>graph.json</strong> or click to browse</p>
                    <input type="file" id="file-input" accept=".json" multiple style="display: none;">
                </div>
                <div class="file-list" id="uploaded-files-list"></div>
            </section>
```
This is the drag-and-drop file upload area. We hide the default browser file chooser (`style="display: none;"`) and use JavaScript to make the `drop-zone` act as the chooser.

```html
            <section class="panel-section" id="status-section">
                <div class="status-indicator">
                    <span class="status-dot green"></span>
                    <span class="status-label" id="load-status">Graph: Not Loaded</span>
                </div>
            </section>
```
Displays whether the graph data was successfully loaded into memory.

#### Configuration (Dropdowns)

```html
            <section class="panel-section">
                <h2 class="section-title">Configuration</h2>
                
                <div class="control-group">
                    <label for="algorithm-select">Select Algorithm</label>
                    <select id="algorithm-select" disabled>
                        <option value="" disabled selected>-- Choose an Algorithm --</option>
                        <option value="bfs">Breadth-First Search (BFS)</option>
                        <option value="dfs">Depth-First Search (DFS)</option>
                        <option value="dijkstra">Dijkstra's Shortest Path</option>
                        <option value="astar">A* shortest path</option>
                        <option value="kruskal">Kruskal's MST</option>
                        <option value="tarjan">Tarjan's SCC</option>
                    </select>
                </div>
```
A `<select>` tag creates a dropdown menu. It starts `disabled` because you can't choose an algorithm until a graph is loaded.

```html
                <div class="control-row path-nodes-select" style="display: none;">
                    <div class="control-group half">
                        <label for="start-node-select">Start Node</label>
                        <select id="start-node-select"></select>
                    </div>
                    <div class="control-group half">
                        <label for="end-node-select">End Node</label>
                        <select id="end-node-select"></select>
                    </div>
                </div>
            </section>
```
These dropdowns allow selecting a source and destination. They are hidden by default (`display: none;`) and only shown by JavaScript when pathfinding algorithms (like Dijkstra or A*) are chosen.

#### Simulation Controls & Speed Slider

```html
            <section class="panel-section">
                <h2 class="section-title">Simulation Controls</h2>
                <div class="playback-controls">
                    <button id="btn-prev" class="control-btn" title="Previous Step" disabled>⏮</button>
                    <button id="btn-play" class="control-btn play" title="Play / Pause" disabled>▶</button>
                    <button id="btn-next" class="control-btn" title="Next Step" disabled>⏭</button>
                    <button id="btn-reset" class="control-btn" title="Reset Simulation" disabled>⟲</button>
                </div>
```
Playback buttons with `title` attributes that show tooltips when hovered.

```html
                <div class="control-group speed-slider-group">
                    <div class="slider-labels">
                        <span>Speed</span>
                        <span id="speed-value">1.0x</span>
                    </div>
                    <input type="range" id="speed-slider" min="100" max="2000" step="100" value="500" class="styled-slider">
                </div>
            </section>
```
An `<input type="range">` creates a draggable slider to control the animation speed.

#### The Console

```html
            <section class="panel-section flex-grow">
                <h2 class="section-title">Execution Console</h2>
                <div class="console-output" id="console-log">
                    <div class="console-line system">System initialized. Waiting for graph.json...</div>
                </div>
            </section>
        </aside>
```
A faux-terminal window. JavaScript constantly adds new `<div class="console-line">` elements here to simulate standard output logs.

---

### The Main Canvas

```html
        <main class="visualization-area">
            
            <div class="canvas-controls">
                <button id="btn-zoom-in" class="canvas-btn" title="Zoom In">+</button>
                <button id="btn-zoom-out" class="canvas-btn" title="Zoom Out">−</button>
                <button id="btn-zoom-fit" class="canvas-btn" title="Fit to Screen">⛶</button>
                <button id="btn-toggle-labels" class="canvas-btn active" title="Toggle Labels">🏷</button>
            </div>
```
The `<main>` tag occupies the large area on the right. Overlaid on top of it are canvas controls (zoom in/out/fit, toggle labels).

```html
            <div class="legend-overlay" id="legend">
                <h3>Legend</h3>
                <div class="legend-items" id="legend-items-list">
                    <div class="legend-item"><span class="legend-color node-default"></span> Default Node</div>
                    <div class="legend-item"><span class="legend-color edge-default"></span> Default Edge</div>
                </div>
            </div>
```
A floating legend box indicating the meaning of the colors used in the graph animation.

```html
            <svg id="graph-svg"></svg>
        </main>
    </div>
```
The empty canvas. SVG stands for Scalable Vector Graphics. The D3 library dynamically generates circles and lines inside this `<svg>`.

```html
    <script src="graph.js"></script>
</body>
</html>
```
Finally, we load `graph.js`. 
**Why at the bottom?** Because `graph.js` will immediately look for elements like `id="btn-play"`. If the script ran at the top before the browser had read the `<button>` tags, it would crash.

---

## Summary Cheat Sheet

| HTML Tag | What It Does visually | How we use it here |
|---|---|---|
| `<div>` | A generic invisible box | Groups elements together for layout |
| `<h1>`, `<h2>` | Large bold headings | Titles for the sidebar sections |
| `<p>` | A paragraph of text | Text instructions |
| `<button>` | A clickable button | Play, pause, next step controls |
| `<select>` | A dropdown menu | Choosing algorithms and nodes |
| `<input type="...">` | Form inputs | File upload triggers and range sliders |
| `<svg>` | A drawing canvas | Where D3 draws the graph circles and lines |
| `<script>` | Loads JavaScript | Connects `graph.js` to the page |
| `id="xyz"` | Unique identifier | How C++/JS finds exactly one element |
| `class="xyz"` | Group identifier | How CSS styles a group of elements |
