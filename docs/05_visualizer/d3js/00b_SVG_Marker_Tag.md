# 00b. The SVG Marker Tag (Arrowheads)

> **Reference: `index.html` `<defs>` section and `graph.js` (Lines where `marker-end` is set)**

This file provides a detailed, ground-up explanation of the SVG `<marker>` tag, explicitly distinguishing between built-in library methods and user-defined variables.

---

## 1. What is the `<marker>` tag?
In standard HTML, if you want an arrow, you might just download a `.png` image of an arrow. But in SVG (vector graphics), we draw things with math. 

The `<marker>` tag is a **built-in SVG element** that acts like a reusable rubber stamp. You define a shape once (like a tiny triangle) inside a `<marker>` tag. Then, you can tell any `<line>` or `<path>` to automatically "stamp" that marker at its start, middle, or end. 

Its most common use case is creating **arrowheads** for directed graphs.

---

## 2. Where is it declared? (The `<defs>` room)
Markers are not drawn on the screen directly. They are "definitions" waiting to be used. Because of this, they must be wrapped inside a `<defs>` tag.

```html
<svg>
  <!-- The invisible storage room -->
  <defs>
    <marker id="my-custom-arrow" ...>
      <!-- Draw the triangle here -->
    </marker>
  </defs>

  <!-- The actual line drawn on screen -->
  <line x1="0" y1="0" x2="100" y2="100" marker-end="url(#my-custom-arrow)"></line>
</svg>
```

- **`<defs>`** (Built-in keyword): Stands for "definitions". Anything inside it is invisible until referenced later.
- **`<marker>`** (Built-in keyword): The wrapper for our rubber stamp.
- **`id="my-custom-arrow"`** (User-defined): We made up this name so we can find it later.

---

## 3. Line-by-Line Breakdown of a Marker
Let's look at the exact code used to create the arrowhead in our D3 graph visualizer.

```html
<marker id="arrowhead" viewBox="0 -5 10 10" refX="28" refY="0" markerWidth="6" markerHeight="6" orient="auto">
    <path d="M0,-5L10,0L0,5" fill="#64748B"></path>
</marker>
```

### The `<marker>` Wrapper Attributes
*All of the following are **built-in SVG attributes**.*

- **`id="arrowhead"`** 
  - *What it is:* The unique name we chose for this marker.
- **`viewBox="0 -5 10 10"`** 
  - *What it is:* Sets up a mini-coordinate system just for the marker itself. `0 -5` is the top-left corner, and `10 10` means the box is 10 units wide and 10 units tall.
- **`refX="28"`** 
  - *What it is:* The "Reference X" coordinate. This is the **anchor point** of the marker. 
  - *Why 28?* Our node circles have a radius of 15px. If `refX` was `0`, the arrowhead would be drawn exactly at the center of the node (hidden underneath the circle). By setting it to `28`, we push the arrowhead 28 pixels backward along the line, so it sits perfectly on the outside edge of the circle.
- **`refY="0"`**
  - *What it is:* The vertical anchor point. `0` means it's perfectly centered vertically on the line.
- **`markerWidth="6"` and `markerHeight="6"`**
  - *What it is:* The physical size of the rubber stamp relative to the screen. 
- **`orient="auto"`**
  - *What it is:* **This is the magic keyword.** It tells the browser: *"Whatever angle the line is drawn at, rotate the arrowhead to match it perfectly."* Without this, all arrows would point straight to the right, regardless of where the line is going.

### The Shape Inside (The `<path>`)
```html
<path d="M0,-5L10,0L0,5" fill="#64748B"></path>
```
- **`<path>`** (Built-in keyword): The SVG tool for drawing custom shapes.
- **`d="..."`** (Built-in keyword): The drawing commands. 
  - `M 0,-5`: **M**ove pen to coordinate `(0, -5)`. (Top left corner of our triangle)
  - `L 10,0`: Draw a **L**ine to `(10, 0)`. (The tip of the arrow)
  - `L 0,5`: Draw a **L**ine to `(0, 5)`. (Bottom left corner of the triangle)
  - Because it's filled with color, the browser connects the last point back to the start automatically, making a solid triangle.
- **`fill="#64748B"`** (User-defined color): A hex code for slate-gray.

---

## 4. How to use it
Once the marker is safely stored in `<defs>`, we attach it to a line using CSS or SVG attributes.

```javascript
// D3 code from our visualizer
linkSelection.append("line")
    .attr("marker-end", "url(#arrowhead)");
```
- **`marker-end`** (Built-in CSS/SVG property): Tells the line to stick a marker at its final `x2, y2` coordinate. (You can also use `marker-start` or `marker-mid`).
- **`url(#arrowhead)`** (Built-in syntax): The CSS way of searching the HTML document for an ID named `arrowhead` and applying it.

## 5. Summary / Key Takeaways

| Term | Built-in or Ours? | What it does |
|---|---|---|
| `<defs>` | Built-in | Invisible storage room for SVG assets. |
| `<marker>` | Built-in | Defines a reusable graphic that attaches to paths. |
| `id="arrowhead"` | Ours | The name we use to link the line to the marker. |
| `refX` | Built-in | Shifts the arrow forward/backward so it doesn't hide under nodes. |
| `orient="auto"` | Built-in | Automatically rotates the arrow to match the line's angle. |
| `marker-end` | Built-in | The attribute placed on a `<line>` to attach the marker to its tip. |
