# 03. UI Controls (Dropdowns, Buttons, Sliders)
*Reference: `visualizer/index.html` (Lines 26 - 57) and `visualizer/style.css` (Lines 145 - 331)*

This document breaks down the HTML form controls and the extensive CSS used to make standard browser inputs look modern and interactive.

---

## 1. The Algorithm Dropdown

### The HTML (`index.html`)
```html
            <section class="panel-section">
                <h2 class="section-title">Algorithm</h2>
                <select id="algorithm-select" disabled>
                    <option value="" disabled selected>-- Choose an Algorithm --</option>
                    <option value="bfs">Breadth-First Search (BFS)</option>
                    <!-- ... -->
                </select>
            </section>
```
**Explanation (Lines 28 - 38):**
- `<select>` creates a dropdown menu.
- The `disabled` attribute on the `<select>` prevents the user from clicking it immediately when the page loads (JavaScript will remove this attribute once the `graph.json` file is loaded).
- Inside the select, each `<option>` represents a choice.
- `value="bfs"` is the internal string sent to JavaScript. The text "Breadth-First Search (BFS)" is what the user actually sees.
- The first option is a placeholder. `selected` makes it the default choice on page load, and `disabled` prevents the user from manually re-selecting it.

### The CSS (`style.css`)
```css
/* ============================================================================
   INPUTS & SELECTION CONTROLS
   ============================================================================ */
select {
    appearance: none;
    background: var(--bg-panel-solid);
    color: var(--text-main);
    border: 1px solid var(--border-glass);
    padding: 12px 16px;
    border-radius: 8px;
    font-size: 13px;
    font-family: inherit;
    cursor: pointer;
    outline: none;
    transition: all 0.2s ease;
    width: 100%;
}
```
**Explanation (Lines 293 - 308):**
- **Critical rule:** `appearance: none;` tells the browser (Chrome, Firefox, Safari) to completely destroy its default, ugly 1990s dropdown styling, allowing us to build it from scratch!
- `background`, `color`, and `border` inject our modern color variables.
- `padding: 12px 16px;` adds empty space inside the box (12px top/bottom, 16px left/right).
- `border-radius: 8px;` rounds the sharp corners of the box.
- `font-family: inherit;` forces the dropdown to use the 'Inter' font from the `body`, rather than the browser's default form font.
- `width: 100%;` forces the dropdown to stretch all the way across the sidebar.

```css
/* The :not(:disabled) pseudo-class ensures we only show hover/focus effects if it is enabled */
select:focus:not(:disabled) {
    border-color: var(--primary);
    box-shadow: 0 0 0 2px var(--primary-glow);
}

/* The :disabled pseudo-class lets us style elements automatically when JS disables them */
select:disabled {
    opacity: 0.5;
    cursor: not-allowed;
}
```
**Explanation (Lines 310 - 319):**
- `:focus` triggers when the user clicks on the dropdown. We change the border to blue (`var(--primary)`) and add a glowing blue shadow. 
- However, `:not(:disabled)` prevents this glowing effect if JavaScript hasn't unlocked the dropdown yet!
- `:disabled` triggers automatically because of the HTML attribute. It drops opacity to 50% (faded) and changes the mouse cursor to a red "Stop" icon (`not-allowed`).

---

## 2. Playback Control Buttons

### The HTML (`index.html`)
```html
            <section class="panel-section">
                <h2 class="section-title">Playback Controls</h2>
                <div class="controls-row">
                    <button id="btn-prev" class="control-btn" disabled title="Previous Step">⏮</button>
                    <button id="btn-play" class="control-btn play" disabled title="Play/Pause">▶</button>
                    <!-- ... -->
                </div>
            </section>
```
**Explanation (Lines 40 - 48):**
- We group the media buttons inside a `div` with the class `controls-row`.
- The `<button>` tags use emojis for icons to keep the codebase simple.
- `title="Play/Pause"` creates a native tooltip that appears when you hover the mouse over the button for a few seconds.

### The CSS (`style.css`)
```css
/* ============================================================================
   BUTTONS (GLASS & PRIMARY)
   ============================================================================ */
.controls-row {
    display: flex;
    gap: 8px;
}
```
**Explanation (Lines 145 - 150):**
- `display: flex;` forces the buttons to sit side-by-side horizontally. `gap: 8px;` adds a small space between them.

```css
.control-btn {
    flex: 1;
    background: rgba(255, 255, 255, 0.05);
    border: 1px solid var(--border-glass);
    color: var(--text-main);
    padding: 12px;
    border-radius: 8px;
    cursor: pointer;
    display: flex;
    justify-content: center;
    align-items: center;
    transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
}
```
**Explanation (Lines 152 - 164):**
- `flex: 1;` is a magic Flexbox rule. It tells every button: "Expand equally so that together, you fill 100% of the row's width." No matter how many buttons you add, they will always be perfectly identical in size!
- `background: rgba(255, 255, 255, 0.05);` gives the buttons a very faint (5%) white tint, mimicking glass.
- `transition: all 0.2s cubic-bezier(...)` tells CSS to smoothly animate *any* changes to this button (like color changes on hover) over 0.2 seconds. The `cubic-bezier` math creates a "springy" animation curve.

```css
.control-btn:hover:not(:disabled) {
    background: rgba(255, 255, 255, 0.1);
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
}
```
**Explanation (Lines 166 - 170):**
- When hovered (and not disabled):
  1. The glass background gets brighter (`0.1` opacity).
  2. `transform: translateY(-2px)` physically lifts the button up 2 pixels on the screen!
  3. `box-shadow` adds a shadow underneath, making the lift look incredibly realistic and 3D.

```css
.control-btn.play {
    background: var(--primary);
    border-color: var(--primary);
    box-shadow: 0 0 16px var(--primary-glow);
}
.control-btn.play:hover:not(:disabled) {
    background: #4F46E5; /* Slightly darker indigo */
    box-shadow: 0 0 24px var(--primary-glow);
}
```
**Explanation (Lines 172 - 180):**
- The play button in HTML has an extra class: `class="control-btn play"`.
- This CSS targets that specific button and completely overrides the glass background, giving it a solid, glowing blue appearance to draw the user's eye!

---

## 3. The Speed Range Slider

### The HTML (`index.html`)
```html
            <section class="panel-section">
                <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px;">
                    <h2 class="section-title" style="margin: 0;">Speed</h2>
                    <span id="speed-value" style="font-size: 11px; color: var(--primary); font-weight: 600;">2.0x</span>
                </div>
                <input type="range" id="speed-slider" class="styled-slider" min="50" max="2000" value="500" step="50">
            </section>
```
**Explanation (Lines 50 - 56):**
- We use an inline `style="display: flex; justify-content: space-between;"` on the `div` to perfectly push the word "Speed" to the far left, and the "2.0x" text to the far right.
- `<input type="range">` generates a draggable slider.
- `min`, `max`, `value` (starting position), and `step` (how much it increments) define the mathematical bounds of the slider (representing milliseconds).

### The CSS (`style.css`)
```css
/* ============================================================================
   RANGE SLIDER STYLING (Cross-Browser)
   ============================================================================ */
.styled-slider {
    -webkit-appearance: none;
    width: 100%;
    height: 6px;
    background: var(--bg-panel-solid);
    border-radius: 3px;
    outline: none;
}
```
**Explanation (Lines 321 - 328):**
- Like the dropdown, `-webkit-appearance: none;` destroys the browser's default, ugly slider UI.
- We redefine the track of the slider as a simple, 6px tall dark box with rounded edges.

```css
.styled-slider::-webkit-slider-thumb {
    -webkit-appearance: none;
    appearance: none;
    width: 16px;
    height: 16px;
    border-radius: 50%;
    background: var(--primary);
    cursor: pointer;
    box-shadow: 0 0 10px var(--primary-glow);
    transition: transform 0.1s ease;
}
```
**Explanation (Lines 330 - 340):**
- `::-webkit-slider-thumb` targets the draggable "knob" on the slider (specifically for Chrome/Safari).
- We style it as a 16x16 pixel perfect circle (`border-radius: 50%`).
- We color it blue (`var(--primary)`) and give it a glowing blue shadow.

```css
.styled-slider::-webkit-slider-thumb:hover {
    transform: scale(1.2);
}
```
**Explanation (Lines 342 - 344):**
- When the user hovers over the knob, `transform: scale(1.2)` forces it to instantly grow 20% larger, providing great tactile feedback!
