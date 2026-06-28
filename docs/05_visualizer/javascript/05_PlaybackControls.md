# 05. Playback and Media Controls
*Reference: `visualizer/graph.js` (Lines 491 - 688)*

This document provides a **line-by-line** breakdown of how the animation playback engine works, mimicking a video player with Play, Pause, Next, and Previous buttons.

---

## 1. Selecting an Algorithm (Lines 496 - 540)
```javascript
/* ============================================================================
   ALGORITHM SELECTION & PLAYBACK CONTROL
   
   This section handles the media buttons (Play, Pause, Step).
   It uses JavaScript timers (setInterval) to create an animation loop 
   that increments the 'currentStep' variable.
   ============================================================================ */
function setAlgorithm(algo) {
    pause();
    
    // Check if the requested algorithm trace is loaded
    if (!algorithmData[algo]) {
        logToConsole(`Algorithm trace not loaded for "${algo.toUpperCase()}"! Run main.cpp and upload the generated steps file.`, "error");
        document.getElementById("algorithm-select").value = activeAlgorithm || "";
        return;
    }
```
**Explanation (Lines 496 - 511):**
- When the user selects an algorithm from the dropdown, we immediately `pause()` any running animation.
- If the required JSON data isn't loaded (e.g., `algorithmData["bfs"] == null`), we log an error, reset the dropdown back to its previous value, and `return` (abort the function).

```javascript
    activeAlgorithm = algo;
    currentStep = 0;
```
**Explanation (Lines 513 - 514):**
- Sets the global `activeAlgorithm` to the new selection.
- Rewinds the animation playhead (`currentStep`) back to `0`.

```javascript
    // Set up step bounds based on algorithm data format
    if (algo === "bfs" || algo === "dfs") {
        maxSteps = algorithmData[algo].steps.length;
    } else if (algo === "dijkstra" || algo === "astar") {
        maxSteps = algorithmData[algo].steps ? algorithmData[algo].steps.length : 0;
    } else if (algo === "kruskal") {
        maxSteps = algorithmData[algo].mst ? algorithmData[algo].mst.length : 0;
    } else if (algo === "tarjan") {
        maxSteps = algorithmData[algo].sccs ? algorithmData[algo].sccs.length : 0;
    }
```
**Explanation (Lines 516 - 526):**
- The total length of the "video" depends on how the C++ engine formatted the JSON. 
- For BFS/DFS, the array is called `steps`. For Kruskal, it's called `mst`.
- `.length` counts the total number of frames in the array and stores it in `maxSteps`.

```javascript
    document.getElementById("btn-play").disabled = false;
    document.getElementById("btn-next").disabled = false;
    document.getElementById("btn-prev").disabled = true;
    
    logToConsole(`Switched to ${algo.toUpperCase()}. Ready to play ${maxSteps} steps.`, "system");
    
    // Trigger initial frame
    resetVisualStates();
    updateSimulationFrame();
}
```
**Explanation (Lines 528 - 537):**
- Now that an algorithm is loaded, we enable the Play and Next buttons in the HTML (`disabled = false`). We keep Previous disabled because we are at frame 0.

**Corresponding HTML and CSS for Disabled Buttons:**
```html
<!-- JavaScript removes the 'disabled' attribute from this button! -->
<button id="btn-play" class="control-btn play" disabled>▶</button>
```
```css
.control-btn:disabled {
    opacity: 0.25; /* Makes the button look faded/ghosted */
    cursor: not-allowed; /* Shows a red "Stop" icon when hovered */
}
```
- It logs a success message, calls `resetVisualStates()` to clear all colors from the map, and calls `updateSimulationFrame()` to draw Frame 0.

---

## 2. Play and Pause (Lines 542 - 564)
```javascript
function togglePlay() {
    if (isPlaying) {
        pause();
    } else {
        play();
    }
}
```
**Explanation (Lines 542 - 548):**
- Acts as a toggle switch. If currently playing, it pauses. Otherwise, it plays.

```javascript
function play() {
    if (currentStep >= maxSteps) return; // Reached end
    
    isPlaying = true;
    document.getElementById("btn-play").innerHTML = "⏸";
    
    playbackInterval = setInterval(() => {
        stepForward();
        
        if (currentStep >= maxSteps) {
            pause();
        }
    }, playbackSpeed);
}
```
**Explanation (Lines 550 - 562):**
- `currentStep >= maxSteps` checks if the animation is already finished. If so, abort.
- `isPlaying = true` updates the global state flag.
- `document.getElementById("btn-play").innerHTML = "⏸"` dynamically swaps the HTML text from a Play Triangle to a Pause icon!
- `setInterval(callback, speed)` is a critical JavaScript tool. It tells the browser: "Run this callback function over and over, waiting `playbackSpeed` milliseconds between each run."
- It stores the ID of this timer inside `playbackInterval`.
- Inside the loop, it calls `stepForward()`, which draws the next frame. If it hits the end of the video, it calls `pause()`.

```javascript
function pause() {
    isPlaying = false;
    document.getElementById("btn-play").innerHTML = "▶";
    clearInterval(playbackInterval);
}
```
**Explanation (Lines 564 - 568):**
- Swaps the button icon back to the Play triangle.
- `clearInterval(playbackInterval)` tells the browser to instantly destroy the timer we created in `play()`, stopping the animation loop!

---

## 3. Stepping Frames (Lines 570 - 688)
```javascript
function stepForward() {
    if (currentStep >= maxSteps) return;
    
    currentStep++;
    updateSimulationFrame();

    document.getElementById("btn-prev").disabled = false;
    if (currentStep >= maxSteps) {
        document.getElementById("btn-next").disabled = true;
        pause();
    }
}
```
**Explanation (Lines 570 - 581):**
- Increments the frame counter (`currentStep++`) and calls `updateSimulationFrame()` to draw the new graphics (covered in Section 6).
- Since we are no longer at frame 0, we can safely enable the Previous button (`.disabled = false`).
- If we hit the absolute end, we disable the Next button and pause the video.

```javascript
function stepBackward() {
    if (currentStep <= 0) return;

    currentStep--;
    updateSimulationFrame();

    document.getElementById("btn-next").disabled = false;
    if (currentStep <= 0) {
        document.getElementById("btn-prev").disabled = true;
    }
}
```
**Explanation (Lines 583 - 593):**
- The exact reverse of `stepForward`. Decrements the frame counter and re-draws the screen.

```javascript
function resetSimulation() {
    pause();
    currentStep = 0;
    resetVisualStates();
    
    document.getElementById("btn-prev").disabled = true;
    document.getElementById("btn-next").disabled = maxSteps === 0;
    logToConsole("Simulation reset.", "system");
}
```
**Explanation (Lines 595 - 603):**
- Hard resets the engine. Pauses the timer, rewinds to frame 0, strips all colors off the map using `resetVisualStates()`, and resets the buttons.

```javascript
function resetVisualStates() {
    svg.selectAll(".node-circle")
        .attr("class", "node-circle")
        .attr("style", null);
        
    svg.selectAll(".graph-link")
        .attr("class", "graph-link")
        .attr("marker-end", "url(#arrowhead)");
}
```
**Explanation (Lines 605 - 613):**
- **Critical Concept:** This function acts like an eraser.
- It grabs every circle on the screen and violently overwrites its `class` attribute to just `"node-circle"`. 
- By doing this, it deletes any animation classes (like `"visited"` or `"path-node"`) that might have been applied during the animation!
- It also resets the `marker-end` attribute of all lines back to the standard dark gray arrowhead, erasing any pink path arrowheads.
