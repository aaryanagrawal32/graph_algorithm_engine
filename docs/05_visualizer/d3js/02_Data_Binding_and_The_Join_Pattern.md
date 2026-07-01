# 02. Data Binding and The Join Pattern

> **Reference: `graph.js` Lines 342–425 (inside `loadGraph()` function)**
>
> **This is the most important and most confusing topic in D3. Read slowly.**

---

## Part 1: The Fundamental Problem D3 Solves

Imagine your C++ engine has just finished running and produced a `graph.json` file with 20 nodes. You drop it into the visualizer. The browser reads it and now has this JavaScript array:

```javascript
// Our C++ engine produced this via JSON
graphData.nodes = [
    { id: 0, x: null, y: null },
    { id: 1, x: null, y: null },
    // ... 18 more nodes
];
```

In plain JavaScript, to draw 20 circles for these nodes, you would write a `for` loop:
```javascript
for (let i = 0; i < graphData.nodes.length; i++) {
    let circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
    circle.setAttribute("r", "15");
    nodeG.appendChild(circle);
}
```

This works, but it has a serious problem: **What happens when you load a different graph with 35 nodes?** You now have 20 stale circles on screen plus 35 new ones = 55 circles. You have to manually delete the old ones first.

**D3's Data Join pattern solves this automatically.** D3 remembers which data is already on screen and handles adding, updating, and removing elements for you.

---

## Part 2: Core Theory — The Three "Rooms"

Before reading the code, you need to understand the abstract concept.

Imagine D3 has three waiting rooms:

**🟢 Enter Room**: "I have new data that doesn't have a matching HTML element yet. What shape should I make for it?"

**🟡 Update Room**: "I have data that already has a matching HTML element. Should I change anything about it?"

**🔴 Exit Room**: "I have HTML elements on screen that no longer have matching data. Should I delete them?"

When you call `.data(graphData.nodes)`, D3 compares your data array against the HTML elements currently on screen and sorts everything into these three rooms.

---

## Part 3: The Complete `loadGraph()` function — Line by Line

### Lines 342–343: Storing the Data

```javascript
function loadGraph(data) {
    graphData = data;
```

**`loadGraph`** — This is **our function**. We named it. It receives the parsed JSON object from the file reader.

**`data`** — This is **our parameter name**. The entire JSON object from `graph.json` arrives here.

**`graphData`** — This is **our global variable** (declared on line 12 at the top of the file). We store the data globally so the physics engine and all other functions can access it.

---

### Lines 345–346: Stopping the Old Simulation

```javascript
    if (simulation) simulation.stop();
```

**`simulation`** — This is **our global variable** (declared on line 33). It holds the D3 physics engine.

**`.stop()`** — This is a **built-in D3 method on the simulation object**. It freezes the physics engine immediately. If we didn't call this, the old physics engine would still be running, fighting against the new one we're about to create — the nodes would fly all over the screen!

---

### Lines 349–350: Clearing the Old Graphics

```javascript
    linkG.selectAll("*").remove();
    nodeG.selectAll("*").remove();
```

**`linkG`** — This is **our global variable** (assigned in `setupSVG()`). It is the D3 selection of the `<g class="links-group">` tag.

**`.selectAll("*")`** — This is a **built-in D3 method**. `selectAll(cssSelector)` finds all matching elements inside the selection. `"*"` is a CSS wildcard meaning "every single child element". So `linkG.selectAll("*")` selects ALL lines inside the links group.

**`.remove()`** — This is a **built-in D3 method**. It deletes every selected element from the DOM. After these two lines, the SVG canvas is completely blank — no circles, no lines.

---

### Lines 377–385: Creating the Physics Simulation

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

This is covered in detail in `03_The_Physics_Engine.md`. For now, just know that after this block, D3 has started calculating `x` and `y` coordinates for each node object in `graphData.nodes`. It modifies the objects directly — after the simulation runs, `graphData.nodes[0].x` will have a real number in it.

---

### Lines 387–393: Creating the Edge Lines (The Join Pattern in Action)

```javascript
    linkSelection = linkG.selectAll(".graph-link")
        .data(graphData.links)
        .enter().append("line")
        .attr("class", "graph-link")
        .attr("stroke-width", 2)
        .attr("marker-end", "url(#arrowhead)");
```

This is the most important block. Let's go character by character.

#### `linkG.selectAll(".graph-link")`

**`linkG`** — Our variable. The `<g class="links-group">` folder.

**`.selectAll()`** — **Built-in D3 method**. Selects ALL elements matching the selector inside `linkG`. Since we just cleared everything with `.remove()`, this returns an **empty selection** — it found 0 matching elements.

**`".graph-link"`** — CSS class selector. Matches elements that have class `graph-link`. Returns empty because we just deleted everything.

#### `.data(graphData.links)`

**`.data()`** — **Built-in D3 method**. This is where the "Join" happens! It takes your data array and tries to match each data item to one of the selected elements. Since our selection is empty (0 elements) but our data has, say, 30 edges:
- 0 things matched → Update Room is empty
- 0 elements have no data → Exit Room is empty
- 30 data items have no element → **Enter Room has 30 items**

**`graphData.links`** — **Our variable**. The array of edge objects from the JSON, like `[{source: 0, target: 1, weight: 4}, ...]`.

#### `.enter()`

**`.enter()`** — **Built-in D3 method**. This explicitly goes into the "Enter Room" and returns a selection representing all the data items that are waiting for a new element to be created.

#### `.append("line")`

**`.append("line")`** — **Built-in D3 method**. For each item in the Enter Room, create a new `<line>` SVG element and insert it into `linkG`. This is the magic moment — D3 creates 30 new `<line>` tags automatically!

After this call, our SVG looks like:
```html
<g class="links-group">
    <line></line>   ← created for edge 0→1
    <line></line>   ← created for edge 0→2
    ...30 lines total...
</g>
```

The result is a D3 selection containing all 30 new `<line>` elements.

#### `.attr("class", "graph-link")`

**`.attr()`** — **Built-in D3 method**. Sets the `class` attribute to `"graph-link"` on all 30 lines at once. This lets CSS apply the dark gray stroke color and smooth transition animation.

#### `.attr("stroke-width", 2)`

Sets the line thickness to 2 pixels on all 30 lines.

#### `.attr("marker-end", "url(#arrowhead)")`

**`marker-end`** — This is a **built-in SVG attribute** (not D3). It tells the browser to draw a marker at the end of each line. `url(#arrowhead)` references the arrowhead marker we defined inside `<defs>` earlier. The browser finds the marker by its ID and draws that triangle shape at the line's endpoint — this is how we get the directed graph arrows!

#### `linkSelection = ...`

**`linkSelection`** — **Our global variable** (declared on line 35). We store the final D3 selection here. Now `linkSelection` is a live handle to all 30 `<line>` tags on screen. We will use it inside the physics tick loop to move the lines.

---

### Lines 396–404: Creating Edge Weight Labels

```javascript
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

This follows the exact same Enter pattern. The new things:

**`.append("text")`** — Creates SVG `<text>` elements (not HTML `<p>` tags!) to display the edge weights.

**`.attr("fill", "#64748B")`** — Sets the text color. In SVG, text color is controlled with `fill`, not `color`.

**`.attr("text-anchor", "middle")`** — This is a built-in SVG attribute. It centers the text horizontally around its `x` position (otherwise text would be left-aligned from the x coordinate).

**`.text(d => d.weight)`** — **Built-in D3 method**. Sets the text content of each `<text>` element. D3 loops through every text element, passes the bound data as `d`, and we return `d.weight`. The `d` here is one edge object like `{source: 0, target: 1, weight: 4}`, and `d.weight` extracts the number `4`.

**`weightSelection`** — **Our global variable** (line 37). Stores this selection for use in the tick loop.

---

### Lines 406–414: Creating the Node Groups

```javascript
    nodeSelection = nodeG.selectAll(".node-group")
        .data(graphData.nodes)
        .enter().append("g")
        .attr("class", "node-group")
        .call(d3.drag()
            .on("start", dragstarted)
            .on("drag", dragged)
            .on("end", dragended));
```

Same Enter pattern, but for nodes.

**`.enter().append("g")`** — Creates a `<g>` group for each node. A group (not a circle!) because each node needs both a circle AND a text label inside it.

**`.call(d3.drag()...)`** — This attaches drag behavior to every node group at once.

**`d3.drag()`** — **Built-in D3 factory function**. Creates a new drag behavior object.

**`.on("start", dragstarted)`** — **Built-in D3 method on drag**. Registers our custom function `dragstarted` (which we defined on line 451) to run when the user first clicks and holds a node.

**`.on("drag", dragged)`** — Registers our `dragged` function (line 457) to run on every mouse movement while dragging.

**`.on("end", dragended)`** — Registers our `dragended` function (line 462) to run when the user releases the mouse.

**`nodeSelection`** — **Our global variable** (line 34). Stores all 20 node group `<g>` elements.

---

### Lines 416–418: Drawing the Circles Inside Each Node Group

```javascript
    nodeSelection.append("circle")
        .attr("class", "node-circle")
        .attr("r", 15);
```

**`nodeSelection.append("circle")`** — We now use our stored `nodeSelection`. This creates a `<circle>` inside each of the 20 node `<g>` groups. After this, each group looks like:
```html
<g class="node-group">
    <circle class="node-circle" r="15"></circle>
</g>
```

**`.attr("r", 15)`** — Sets the circle radius to 15 pixels. Notice we do NOT set `cx` or `cy` (center position). That's because the circle will be positioned using its parent `<g>` tag's `transform` attribute — which the physics engine updates every tick.

---

### Lines 421–425: Drawing the Labels Inside Each Node Group

```javascript
    labelSelection = nodeSelection.append("text")
        .attr("class", "node-label")
        .attr("text-anchor", "middle")
        .attr("dy", ".3em")
        .text(d => d.id);
```

**`nodeSelection.append("text")`** — Creates a `<text>` element inside each node group. The group now looks like:
```html
<g class="node-group">
    <circle class="node-circle" r="15"></circle>
    <text class="node-label" text-anchor="middle" dy=".3em">5</text>
</g>
```

**`.attr("dy", ".3em")`** — `dy` shifts the text element down relative to its `y` position. `".3em"` means 30% of the font size. This is the standard SVG trick to visually center text inside a circle (because SVG text is positioned at its baseline, not its center — a small vertical offset corrects this).

**`.text(d => d.id)`** — Sets the visible text content of each label. D3 loops through all labels, passes each node's data object as `d`, and we return `d.id`. So node 5's label gets text "5", node 12's label gets text "12", etc.

**`labelSelection`** — **Our global variable** (line 36). Stored for later use by the toggle-labels button.
