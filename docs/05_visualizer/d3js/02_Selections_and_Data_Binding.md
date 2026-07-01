# 02. Selections and Data Binding (Enter, Update, Exit)
*Reference: `visualizer/graph.js` (Lines 185 - 243)*

This is the most confusing, yet most powerful concept in D3.js. How do you take an array of data and turn it into circles on the screen?

---

## 1. The Core Concepts

### What is Data Binding?
In traditional programming, if you have an array of 5 nodes and want to draw them, you write a `for` loop.

**Not D3!** D3 uses a declarative approach. You don't tell D3 *how* to loop. You say: "Here is a group of HTML elements, and here is an array of data. Bind them together."

### The Enter, Update, Exit Pattern
Imagine you have 3 circles on the screen, but your C++ backend just sent you an array of 5 nodes.
- **Enter:** D3 notices there are 2 missing circles. It puts these 2 pieces of data into the "Enter" room, waiting for you to tell it what shape to draw.
- **Update:** D3 notices 3 pieces of data perfectly match the 3 existing circles. It updates them.
- **Exit:** What if the C++ backend only sent 1 node? D3 puts the 2 leftover circles into the "Exit" room, waiting for you to delete them.

In modern D3 (v4+), the `.join()` command automatically handles all three rooms for you!

---

## 2. Line-by-Line Application in `graph.js`

Every time the user loads a new graph, we call the `updateGraph()` function. It takes the arrays `graphData.nodes` and `graphData.links` and binds them to the screen.

### Code Breakdown (Lines 185 - 243)

```javascript
node = nodeGroup.selectAll("g")
    .data(graphData.nodes, d => d.id)
```
**Explanation:** 
- `nodeGroup.selectAll("g")`: We select all the `<g>` (group) tags that *currently exist* inside our node folder. (If this is the first time loading, there are 0 groups!).
- `.data(...)`: We pass in our array of nodes from C++.
- `d => d.id`: **Crucial!** This is the "Key Function". It tells D3 how to match data to circles. If a circle on the screen has `id=5`, and the incoming data has `id=5`, D3 knows they are the same node and will update it instead of deleting it.

```javascript
    .join("g")
```
**Explanation:** 
- `join("g")` automatically looks at the "Enter" room. For every new piece of data that doesn't have a matching group on screen, it creates a brand new `<g>` tag! It also automatically deletes any old groups in the "Exit" room.

```javascript
    .attr("class", "node-group")
```
**Explanation:** 
- Gives every newly created `<g>` the CSS class `node-group` so our CSS file can style it.

```javascript
    .call(d3.drag()
        .on("start", dragstarted)
        .on("drag", dragged)
        .on("end", dragended));
```
**Explanation:** 
- We attach "Drag" physics to every single node. If the user clicks and drags a node, D3 fires the `dragstarted`, `dragged`, and `dragended` functions (which we explain in the Physics section).

### Drawing the Shapes Inside the Group

Remember, we just created a `<g>` (an invisible folder) for each node. Now we need to put a circle and some text *inside* each folder.

```javascript
node.selectAll("circle").remove();
node.selectAll("text").remove();
```
**Explanation:** 
- To prevent accidentally drawing multiple circles on top of each other when updating the graph, we quickly delete any existing circles and text inside the groups.

```javascript
node.append("circle")
    .attr("r", 16)
    .attr("class", "node-circle")
```
**Explanation:** 
- `append("circle")`: Creates an SVG `<circle>` tag inside each group.
- `.attr("r", 16)`: Sets the radius of the circle to 16 pixels.
- `.attr("class", "node-circle")`: Applies our beautiful CSS colors to the circle!

```javascript
node.append("text")
    .text(d => d.id)
    .attr("text-anchor", "middle")
    .attr("dy", ".35em")
```
**Explanation:** 
- `append("text")`: Creates an SVG `<text>` tag.
- `.text(d => d.id)`: **This is Data Binding in action!** D3 loops through every node. `d` represents the current piece of data. We tell D3 to set the text on the screen to the node's ID (e.g., "0", "1", "2").
- `text-anchor: middle`: Horizontally centers the text inside the circle.
- `dy: .35em`: A classic SVG trick. It pushes the text down slightly so it is perfectly vertically centered inside the circle!
