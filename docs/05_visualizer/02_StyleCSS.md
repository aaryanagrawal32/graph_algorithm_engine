# style.css — The Design and Colors
### A Beginner-Friendly Guide

> **Who is this for?**
> This guide is for someone who understands C++ but has never written CSS. We will explain how the visualizer goes from a plain white page with basic buttons to a sleek, dark-mode, animated application.

---

## What Is This File?

If `index.html` is the skeleton, `style.css` is the skin, clothes, and makeup. 

CSS (Cascading Style Sheets) is a language that tells the browser **how** HTML elements should look: what color they are, how big they are, where they are positioned, and how they animate.

---

## Part 0: CSS Building Blocks

A CSS "rule" looks like this:

```css
.control-btn {
    background: #111827;
    border-radius: 8px;
}
```

1. **`.control-btn`** is the **Selector**. It answers: *"Who are we styling?"* The dot `.` means "find all HTML elements that have `class="control-btn"`".
2. **`background:`** is the **Property**. It answers: *"What feature are we changing?"*
3. **`#111827`** is the **Value**. It answers: *"What is the new setting?"* (This is a hex code for a very dark blue).

### Common Selectors
- `body` (no dot) = Styles the HTML tag `<body>` itself.
- `.console-output` (with dot) = Styles elements with `class="console-output"`.
- `#btn-play` (with hash) = Styles the exact one element with `id="btn-play"`.

---

## Part 1: The Code Walkthrough

### 1. Variables (The Color Palette)

```css
:root {
    --bg-app: #0B0F19;
    --primary: #6366F1;       /* Vibrant Indigo */
    --accent: #F43F5E;        /* Rose Pink */
    --success: #10B981;       /* Emerald Green */
}
```
Like `const` variables in C++, we define our colors once at the top. `:root` means these variables are available everywhere. Later, instead of copying `#6366F1` 50 times, we just write `var(--primary)`. If we ever want to change our primary color to red, we only change it in one place!

### 2. The Basic Reset

```css
* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}
```
The `*` means "select absolutely everything." Browsers add default spacing (margins) to headings and paragraphs. This block wipes all that out to zero so we have a perfectly clean slate to work from.

### 3. Glassmorphism (The Frosted Glass Effect)

Modern web apps often use a "frosted glass" effect for their sidebars. We do this for `.control-panel`:

```css
.control-panel {
    background: rgba(17, 24, 39, 0.75); /* Semi-transparent dark blue */
    backdrop-filter: blur(16px);        /* This creates the frosted glass! */
    border-right: 1px solid rgba(255, 255, 255, 0.08); /* Faint white border */
    box-shadow: 10px 0 30px rgba(0, 0, 0, 0.3); /* Drop shadow on the right */
}
```
`rgba(R, G, B, Alpha)` sets color and transparency. The `0.75` alpha means it's 75% solid, 25% see-through. 
`backdrop-filter: blur(16px)` takes whatever is *behind* the panel (the graph canvas) and blurs it.

### 4. Flexbox (How Things Are Arranged)

In HTML, elements normally stack top-to-bottom. If you want them side-by-side, you use Flexbox.

```css
.playback-controls {
    display: flex;
    justify-content: space-between;
    gap: 10px;
}
```
- `display: flex;` activates the flexbox layout system.
- `justify-content: space-between;` spaces the play/pause/next buttons evenly across the row.
- `gap: 10px;` puts a 10-pixel gap between each button.

### 5. Hover Effects and Transitions

We want buttons to react when you move your mouse over them.

```css
.control-btn {
    transition: all 0.2s; /* Make changes smooth, taking 0.2 seconds */
}

.control-btn:hover:not(:disabled) {
    background: rgba(255, 255, 255, 0.1);
    transform: translateY(-1px); /* Move the button UP slightly */
}
```
The `:hover` pseudo-class applies these styles *only when the mouse is over the element*. 
The `transition: all 0.2s` makes it animate smoothly rather than snapping instantly. `transform: translateY(-1px)` makes the button physically lift up by 1 pixel, giving a tactile feel.

### 6. Animations (Keyframes)

The pulsing "Drag & Drop" icon is created using CSS Keyframes.

```css
@keyframes pulseIcon {
    0%, 100% { transform: translateY(0); }
    50% { transform: translateY(-4px); }
}

.upload-icon {
    animation: pulseIcon 2s infinite ease-in-out;
}
```
This defines an animation named `pulseIcon`. At the start (0%) and end (100%), the icon is at its normal position. Halfway through (50%), it moves up 4 pixels. 
We then tell `.upload-icon` to run this animation, making it take 2 seconds, looping `infinite`ly.

---

## Part 2: Styling the SVG Graph

The actual graph nodes and edges are drawn as SVG tags (like `<circle>` and `<line>`) by JavaScript, but they are styled here in CSS!

```css
.node-circle {
    fill: var(--node-fill);      /* Inside color */
    stroke: var(--node-stroke);  /* Border color */
    stroke-width: 2px;           /* Border thickness */
    transition: fill 0.3s ease, stroke 0.3s ease;
}
```

When an algorithm visits a node, JavaScript adds a new class `visited` to the circle. CSS then takes over and animates it to the new color automatically:

```css
.node-circle.visited {
    fill: rgba(6, 182, 212, 0.2);
    stroke: var(--secondary);
    filter: drop-shadow(0 0 8px rgba(6, 182, 212, 0.4)); /* Glowing aura */
}
```

If it is the currently active node, we add an animation so it throbs like a heartbeat:
```css
.node-circle.active {
    animation: pulsingActive 1.5s infinite alternate;
}
@keyframes pulsingActive {
    0% { transform: scale(1); }
    100% { transform: scale(1.1); } /* Grow by 10% */
}
```

---

## Summary Cheat Sheet

| CSS Property | What It Does |
|---|---|
| `background` | Sets the background color or gradient |
| `color` | Sets the text color |
| `border-radius` | Rounds the corners of a box (e.g., `8px` or `50%` for a circle) |
| `display: flex` | Arranges items side-by-side easily |
| `gap` | Space between flex items |
| `opacity` | Transparency (0 is invisible, 1 is solid) |
| `transition` | Makes changes (like hovering) animate smoothly over time |
| `transform` | Scales, rotates, or moves an element (e.g., `scale(1.1)`) |
| `fill` / `stroke` | Like `background` and `border` but specifically for SVG drawings |
| `box-shadow` | Adds a drop shadow behind an element |
| `filter: drop-shadow()` | Adds a glowing aura around an SVG shape |
