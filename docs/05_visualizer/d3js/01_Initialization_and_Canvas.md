# 01. Initialization and Canvas Setup

> **Reference: `graph.js` Lines 32–38 (global declarations) and Lines 111–151 (the `setupSVG()` function)**
>
> **Prerequisite: Read `00_Prerequisite_JavaScript_and_SVG.md` first!**

---

## Part 1: The Global Variable Declarations (Lines 32–38)

Before we read any D3 code, we need to understand the "storage rooms" that hold D3 objects globally. Open `graph.js` and look at lines 32–38:

```javascript
// Line 32 — OUR VARIABLE (we invented these names)
let svg, containerG, linkG, nodeG;

// Line 33 — OUR VARIABLE
let simulation = null;

// Lines 34–37 — OUR VARIABLES
let nodeSelection = null;
let linkSelection = null;
let labelSelection = null;
let weightSelection = null;

// Line 38 — OUR VARIABLE
let zoomBehavior = null;
```

**Every single one of these is a variable WE declared. None of them are D3 keywords.**

We declare them at the top of the file (outside any function) so that ALL functions can access and modify them. This is exactly like a C++ global variable.

| Variable Name | What It Will Hold |
|---|---|
| `svg` | The main D3 "selection" object wrapping our `<svg id="graph-svg">` tag |
| `containerG` | A D3 selection of the master `<g>` group inside the SVG |
| `linkG` | A D3 selection of the `<g>` folder for all edge lines |
| `nodeG` | A D3 selection of the `<g>` folder for all node circles |
| `simulation` | The D3 physics engine object |
| `nodeSelection` | A live D3 selection of all node `<g>` tags on screen |
| `linkSelection` | A live D3 selection of all edge `<line>` tags on screen |
| `labelSelection` | A live D3 selection of all label `<text>` tags on screen |
| `weightSelection` | A live D3 selection of all weight `<text>` tags on screen |
| `zoomBehavior` | The D3 zoom handler object |

We initialize them to `null` because at the time the browser loads the page, no graph has been uploaded yet. They get assigned their real values when `setupSVG()` and `loadGraph()` run.

---

## Part 2: The `setupSVG()` Function (Lines 111–151)

This function runs once when the page loads. Its job is to build the SVG layer structure and set up zoom.

### Line 112: `svg = d3.select("#graph-svg");`

**`d3`** — This is the D3 library itself. When the browser downloaded `d3.v7.min.js` from the CDN, it created a global JavaScript object called `d3`. Everything in the D3 library lives under this object.

**`.select()`** — This is a **built-in D3 method**. `d3.select(cssSelector)` works like `document.querySelector()` in plain JavaScript. It searches the DOM for the first element matching the CSS selector and wraps it in a special D3 object (called a "D3 Selection") that gives access to all D3 methods.

**`"#graph-svg"`** — This is a CSS selector. The `#` means "find by ID". It matches our `<svg id="graph-svg">` tag in `index.html`.

**`svg`** — This is **our variable** (declared on line 32). We store the D3 selection here so every other function can use it.

After this line, `svg` is not the raw HTML `<svg>` DOM element. It is a special D3 wrapper around it that lets us chain D3 methods.

---

### Line 113: `containerG = svg.append("g").attr("class", "zoom-container");`

This is D3 method chaining — we perform two actions in a single line.

**`.append("g")`** — This is a **built-in D3 method**. It creates a new tag inside the selected element and returns a new D3 selection for that newly created tag. `"g"` tells it to create an SVG `<g>` (group) tag. After this call, our SVG structure looks like:
```html
<svg id="graph-svg">
    <g></g>   ← just created
</svg>
```

**`.attr("class", "zoom-container")`** — This is a **built-in D3 method**. `attr(name, value)` sets an HTML/SVG attribute on the selected element. Here it sets `class="zoom-container"` for CSS styling. After this call:
```html
<svg id="graph-svg">
    <g class="zoom-container"></g>
</svg>
```

**`containerG`** — This is **our variable** (declared on line 32). We store the D3 selection of this new `<g>` here. This group will contain EVERYTHING — all edges and all nodes. When the user zooms in or pans, we apply a transform to this single `<g>` and the whole graph moves together.

---

### Lines 116–126: The Arrowhead Marker Definition

This block defines the shape of the arrowheads drawn at the end of each directed edge. Understanding it requires knowing about SVG `<defs>` and `<marker>`.

#### What is `<defs>`?
In SVG, `<defs>` is a hidden storage area for reusable shapes. Elements defined inside `<defs>` are **not drawn** on screen. They are just templates that can be referenced later by ID. Our HTML structure after this block:
```html
<svg id="graph-svg">
    <defs>
        <marker id="arrowhead"> ... shape definition ... </marker>
    </defs>
    <g class="zoom-container"></g>
</svg>
```

#### Line 116: `svg.append("defs").append("marker")`

**`svg.append("defs")`** — Creates a `<defs>` block inside the SVG. Returns a D3 selection of that `<defs>` tag.

**`.append("marker")`** — Chains immediately. Creates a `<marker>` tag inside the `<defs>`. Returns a D3 selection of the `<marker>` tag. All subsequent `.attr()` calls are now setting attributes on this `<marker>`.

#### Line 117: `.attr("id", "arrowhead")`
Sets `id="arrowhead"` on the marker. This ID is how we reference the arrowhead later. When we later write `.attr("marker-end", "url(#arrowhead)")` on a line, SVG looks up this ID in `<defs>` and draws it at the line's end.

#### Line 118: `.attr("viewBox", "0 -5 10 10")`
**`viewBox`** defines the internal coordinate space of the marker. `"0 -5 10 10"` means: the marker's internal coordinate system starts at x=0, y=-5, is 10 units wide and 10 units tall. This lets us draw the path shape in this simple local space.

#### Line 119: `.attr("refX", 22)`
**`refX`** defines where the "tip" of the arrowhead aligns to the line's endpoint. `22` pixels back from the marker's coordinate origin. This is carefully calculated: our node circles have radius 15, plus ~7px padding, so the arrow tip lands exactly at the edge of the circle without overlapping it.

#### Line 120: `.attr("refY", 0)`
Centers the arrowhead vertically on the line (0 means centered in the `viewBox`).

#### Lines 121–122: `markerWidth` and `markerHeight`
These control the size the marker is drawn at on screen. `6` units in both dimensions makes the arrowhead small and elegant.

#### Line 123: `.attr("orient", "auto")`
**`orient: "auto"`** is a built-in SVG instruction. It tells SVG to automatically rotate the arrowhead so it always points in the same direction as the line it is attached to. Without this, all arrowheads would point right regardless of the edge direction.

#### Line 124: `.append("path")`
Creates a `<path>` tag inside the `<marker>`. The `<path>` tag is what actually draws the arrowhead triangle shape.

#### Line 125: `.attr("d", "M0,-4L9,0L0,4")`
**`d`** is the attribute on `<path>` that holds drawing commands (see the prerequisites file for the command language):
- `M0,-4` → Move to (0, -4) — top-left of arrow
- `L9,0` → Line to (9, 0) — the sharp tip pointing right
- `L0,4` → Line to (0, 4) — bottom-left of arrow

The result is a small triangle. SVG automatically fills it with the stroke color of the line it's attached to.

#### Line 126: `.attr("class", "arrowhead")`
Applies the CSS class `arrowhead` so `style.css` can color it.

#### Lines 128–138: The Second Arrowhead
This is an identical copy, but with `id="arrowhead-path"` and class `arrowhead path-arrow`. When Dijkstra or A* highlights a shortest path edge, the edge gets a different color class and uses this second arrowhead so the arrow also changes color.

---

### Line 140: `linkG = containerG.append("g").attr("class", "links-group");`

**`containerG.append("g")`** — Creates a new `<g>` inside our master container group. This `<g>` will be the folder for ALL edge lines.

**`linkG`** — This is **our variable**. We store this selection here. Later, when `loadGraph()` runs, it uses `linkG.selectAll(...)` to create lines inside this folder.

---

### Line 141: `nodeG = containerG.append("g").attr("class", "nodes-group");`

Same idea — creates a second `<g>` folder inside `containerG` for all node circles.

**`nodeG`** — This is **our variable**.

After lines 140–141, the SVG structure is:
```html
<svg id="graph-svg">
    <defs> ... arrowhead markers ... </defs>
    <g class="zoom-container">       ← containerG
        <g class="links-group"></g>  ← linkG (lines are created here)
        <g class="nodes-group"></g>  ← nodeG (circles are created here)
    </g>
</svg>
```

**Critical question: Why declare `linkG` BEFORE `nodeG`?**
In SVG, there is no CSS `z-index`. Elements drawn later in the file appear on TOP of elements drawn earlier. By declaring `linkG` (edges) before `nodeG` (circles), we guarantee:
- All edges are drawn first → they appear underneath
- All circles are drawn second → they appear on top of the edges

If we reversed the order, the edge lines would be drawn on top of the circles, making the graph look ugly.

---

### Lines 144–150: Setting Up Zoom and Pan

```javascript
zoomBehavior = d3.zoom()
    .scaleExtent([0.1, 4])
    .on("zoom", (event) => {
        containerG.attr("transform", event.transform);
    });

svg.call(zoomBehavior);
```

#### Line 144: `zoomBehavior = d3.zoom()`
**`d3.zoom()`** — This is a **built-in D3 factory function**. Calling it creates and returns a new "zoom behavior" object. This object understands mouse wheel events and touch pinch gestures and translates them into math transforms.

**`zoomBehavior`** — This is **our variable** (declared on line 38). We store the returned zoom behavior object here.

#### Line 145: `.scaleExtent([0.1, 4])`
**`.scaleExtent()`** — This is a **built-in D3 method on the zoom behavior**. It sets the minimum and maximum zoom scale. `[0.1, 4]` means:
- `0.1` → User can zoom OUT to 10% size (the graph looks tiny, far away)
- `4` → User can zoom IN to 400% size (one circle fills the screen)

Without this limit, the user could zoom out infinitely until the graph disappears, or zoom in until a single pixel fills the screen.

#### Lines 146–148: `.on("zoom", (event) => { ... })`
**`.on("event", callback)`** — This is a **built-in D3 method**. It registers an event listener. Whenever D3's zoom behavior detects a scroll or drag, it fires our callback.

**`(event) =>`** — This is an arrow function. `event` is the parameter name we chose. D3 passes in an event object automatically.

**`event.transform`** — This is a **built-in D3 property on the zoom event**. It is a math object containing the current scale and translation values. We don't calculate these ourselves — D3 does that automatically based on the mouse wheel input.

**`containerG.attr("transform", event.transform)`** — We take that D3-calculated transform and apply it to our master container group. This moves and scales the entire graph as a unit. For example, when you scroll your mouse wheel forward, `event.transform` might contain `scale(1.5) translate(-100, -50)` and D3 applies that math to stretch and reposition the `<g>` tag.

#### Line 150: `svg.call(zoomBehavior)`
**`.call()`** — This is a **built-in D3 method**. `selection.call(behavior)` passes the selection (`svg`) to the behavior's function. This is how you "attach" a D3 behavior to an element. Without this line, we would have configured the zoom but never attached it to the SVG — the mouse wheel would do nothing.
