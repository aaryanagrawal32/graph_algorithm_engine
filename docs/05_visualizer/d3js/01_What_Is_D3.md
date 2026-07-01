# 01. What is D3.js and The DOM?
*Reference: `visualizer/graph.js` (Lines 89 - 105)*

D3.js (Data-Driven Documents) is a notoriously powerful but complex JavaScript library. This guide explains its core philosophy and how we use it to initialize our canvas.

---

## 1. The Core Concepts

### What is the DOM?
The **DOM** (Document Object Model) is how the browser stores the webpage in memory. If HTML is the "source code" of the page, the DOM is the "live structure" the browser builds from it. 

When you use JavaScript to change the webpage (like adding a circle or changing a color), you are manipulating the DOM.

### What makes D3 special?
Normal JavaScript manipulating the DOM looks like this:
```javascript
let myBox = document.getElementById("my-box");
myBox.style.backgroundColor = "blue";
myBox.innerText = "Hello!";
```

D3.js introduces two major paradigms:
1. **Method Chaining:** You perform multiple actions in one continuous line of code.
2. **Data-Driven:** Instead of saying "create 5 circles", you say "here is an array of 5 data points, map them to circles."

**D3 Method Chaining Example:**
```javascript
d3.select("#my-box")
  .style("background-color", "blue")
  .text("Hello!");
```

---

## 2. Line-by-Line Application in `graph.js`

In our visualizer, we use D3 to select our main SVG canvas and set up a giant `<g>` (group) tag inside it to hold all our circles and lines.

### Code Breakdown (Lines 89 - 105)

```javascript
svg = d3.select("#graph-svg")
```
**Explanation:** 
- `d3.select()` searches the DOM (the live HTML) for an element with the exact ID `graph-svg`.
- If you look at `index.html`, this matches our `<svg id="graph-svg" width="100%" height="100%"></svg>` tag.

```javascript
    .call(zoom)
```
**Explanation:** 
- `call()` is a D3 function that takes the element we just selected (the SVG) and passes it directly to another function.
- Here, we pass the SVG to the `zoom` function (which we configure in `04_Zoom_and_Pan_Events.md`). This magically enables the user to scroll their mouse wheel to zoom in on the canvas!

```javascript
    .on("dblclick.zoom", null); 
```
**Explanation:** 
- `.on("event_name", function)` tells D3 to listen for a user action.
- By default, D3's zoom behavior makes the canvas zoom in if you double-click.
- We don't want this! We pass `null` to disable the default `dblclick` zooming behavior, because it feels jarring to the user.

```javascript
g = svg.append("g");
```
**Explanation:** 
- `append("g")` creates a brand new HTML tag inside the SVG we selected. 
- `<g>` stands for "Group". It is an invisible folder inside an SVG that holds other shapes. 
- We store a reference to this group in the variable `g`.

```javascript
linkGroup = g.append("g").attr("class", "links");
nodeGroup = g.append("g").attr("class", "nodes");
labelGroup = g.append("g").attr("class", "labels");
```
**Explanation:** 
- Now we take our invisible `g` folder, and we create **three more invisible sub-folders** inside it!
- `.attr("class", "links")` gives the new group a CSS class name so we can identify it.
- **Why do this?** In SVG, there is no "z-index" (layers). Things are drawn in the exact order they appear in the HTML. By making a `links` group *before* the `nodes` group, we guarantee that all the lines are drawn *underneath* the circles, instead of drawing ugly lines across the top of the circles!
