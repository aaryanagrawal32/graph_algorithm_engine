# 01. Base HTML and Global Layout CSS
*Reference: `visualizer/index.html` (Lines 1 - 18) and `visualizer/style.css` (Lines 1 - 56)*

This document provides a true **line-by-line** breakdown of the foundational HTML and CSS that sets up the webpage.

---

## 1. The HTML Skeleton (`index.html`)

```html
<!DOCTYPE html>
```
**Explanation (Line 1):** This mandatory declaration tells the browser that this is a modern HTML5 document. Without it, the browser might render the page in "quirks mode", causing CSS bugs.

```html
<html lang="en">
```
**Explanation (Line 2):** The root element of the entire page. `lang="en"` tells screen readers and search engines the page is in English.

```html
<head>
    <meta charset="UTF-8">
```
**Explanation (Lines 3 - 4):** The `<head>` tag contains metadata (hidden information). `<meta charset="UTF-8">` ensures the browser can correctly render special characters, emojis, and symbols (like the ▶ button).

```html
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
```
**Explanation (Line 5):** A critical tag for modern web design. It tells mobile phones to scale the website width to exactly match the phone's screen width, preventing the website from looking tiny on mobile.

```html
    <title>Graph Algorithm Visualizer</title>
```
**Explanation (Line 6):** Sets the text that appears in the Chrome/Safari browser tab.

```html
    <!-- Google Fonts: Inter -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
```
**Explanation (Lines 8 - 11):** 
- `preconnect` tells the browser to instantly open a network connection to Google's servers before the rest of the page even loads, speeding up font loading.
- The third `<link>` actually downloads the "Inter" font family in 4 different weights (Normal, Medium, Semi-Bold, Bold).

```html
    <!-- D3.js Library -->
    <script src="https://d3js.org/d3.v7.min.js"></script>
```
**Explanation (Lines 13 - 14):** Downloads the D3.js physics and SVG drawing library via CDN (Content Delivery Network). `min.js` means the code is "minified" (all spaces removed) to download faster.

```html
    <link rel="stylesheet" href="style.css">
</head>
```
**Explanation (Lines 16 - 17):** Links our custom CSS file, allowing the browser to read all the rules we define in `style.css`.

---

## 2. Design System Variables (`style.css`)

```css
:root {
    --bg-app: #0B0F19;
    --bg-panel: rgba(26, 32, 44, 0.7);
    --bg-panel-solid: #1A202C;
    --border-glass: rgba(255, 255, 255, 0.08);
```
**Explanation (Lines 5 - 10):**
- `:root` is a special CSS pseudo-class that matches the highest element (the `<html>` tag). Any variables defined here are accessible everywhere.
- `--bg-app: #0B0F19;` creates a variable (Custom Property) for a very dark navy blue.
- `rgba(26, 32, 44, 0.7)` creates a color with 70% opacity (the `0.7` part). This allows the background to bleed through, which is the foundation of the Glassmorphism aesthetic.
- `--border-glass` creates an almost completely transparent white border (8% opacity).

```css
    --text-main: #F8FAFC;
    --text-muted: #94A3B8;
    --primary: #6366F1;
    --primary-glow: rgba(99, 102, 241, 0.4);
    --success: #34D399;
    --accent: #EC4899;
    --edge-stroke: #334155;
}
```
**Explanation (Lines 11 - 18):**
- Defines the rest of the color palette. 
- `--primary` is the vibrant indigo used for buttons.
- `--success` is the green used for Kruskal's MST.
- `--accent` is the hot pink used for Dijkstra's shortest path.

---

## 3. Global CSS Resets (`style.css`)

```css
* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}
```
**Explanation (Lines 20 - 24):**
- The asterisk `*` targets literally every single element on the page.
- `margin: 0; padding: 0;` strips away the ugly default margins browsers add to headers and paragraphs.
- `box-sizing: border-box;` is a mandatory rule in modern CSS. Without it, if you make a box `100px` wide and add `10px` of padding, the box expands to `120px` wide. With `border-box`, the padding is pushed *inward*, keeping the box exactly `100px`.

---

## 4. The HTML Body Setup

**HTML Side (`index.html`):**
```html
<body>
    <div class="app-container">
        <!-- Sidebar ... -->
        <!-- Main canvas ... -->
    </div>
    <script src="graph.js"></script>
</body>
```
**Explanation:** 
- The `<body>` contains everything visible to the user.
- We immediately wrap everything in a master `div` with the class `app-container`.
- At the very bottom of the body, we load `graph.js`. We put it at the bottom so the browser renders the visual HTML before it spends time executing complex JavaScript.

**CSS Side (`style.css`):**
```css
body {
    background-color: var(--bg-app);
    color: var(--text-main);
    font-family: 'Inter', system-ui, -apple-system, sans-serif;
```
**Explanation (Lines 26 - 29):**
- `var(--bg-app)` injects the variable we defined in `:root`.
- `font-family` tells the browser to use 'Inter'. If it fails to download, it falls back to the operating system's default UI font (`system-ui`).

```css
    height: 100vh;
    overflow: hidden;
    display: flex;
    justify-content: center;
    align-items: center;
}
```
**Explanation (Lines 30 - 34):**
- `height: 100vh;` forces the body to be exactly 100% of the Viewport Height (the size of the browser window).
- `overflow: hidden;` prevents scrollbars from ever appearing on the main page.
- `display: flex; justify-content: center; align-items: center;` creates a Flexbox layout and centers everything inside the body dead in the middle of the screen.

---

## 5. Master Layout Containers (`style.css`)

```css
.app-container {
    width: 100vw;
    height: 100vh;
    display: flex;
    flex-direction: row;
    background: radial-gradient(circle at center, #111827 0%, #0B0F19 100%);
}
```
**Explanation (Lines 36 - 42):**
- `.app-container` targets our master `div`.
- `width: 100vw; height: 100vh;` ensures it fills 100% of the viewport width and height.
- `display: flex; flex-direction: row;` means its children (the Sidebar and the Canvas) will sit side-by-side horizontally.
- `background: radial-gradient(...)` draws a subtle glowing circle in the center of the dark background, fading to pitch black at the edges.

```css
.main-content {
    flex-grow: 1;
    position: relative;
    overflow: hidden;
    display: flex;
    flex-direction: column;
}
```
**Explanation (Lines 44 - 50):**
- The `.main-content` is the right-side of the screen (the D3 Canvas).
- `flex-grow: 1;` tells it: "After the sidebar takes up its fixed width (e.g. 420px), you are allowed to stretch and consume 100% of the remaining horizontal space!"
- `position: relative;` allows us to absolutely position UI buttons (like zoom controls) *relative* to the corners of this canvas area, rather than relative to the entire webpage.
