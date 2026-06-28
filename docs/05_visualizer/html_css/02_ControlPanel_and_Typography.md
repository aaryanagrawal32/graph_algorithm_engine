# 02. Control Panel & Typography
*Reference: `visualizer/index.html` (Lines 20 - 24) and `visualizer/style.css` (Lines 57 - 149)*

This document breaks down the HTML structure of the sidebar and the CSS used to create the modern "Glassmorphism" effect and typography.

---

## 1. The Sidebar HTML (`index.html`)

```html
        <!-- Sidebar Controls -->
        <aside class="control-panel">
            <header class="panel-header">
                <h1 class="title">Graph Visualizer</h1>
                <p class="subtitle">C++ Algorithm Engine</p>
            </header>
```
**Explanation (Lines 20 - 24):**
- `<aside>` is a semantic HTML5 tag. It acts exactly like a `<div>`, but tells search engines and screen readers that this content is a "sidebar" or supplementary to the main canvas.
- Inside it, we use a `<header>` tag to group our titles.
- `<h1>` is the main page title. You should only ever have one `<h1>` per page for SEO reasons.
- `<p>` is a standard paragraph tag used here for a subtitle.

---

## 2. The Glassmorphism Effect (`style.css`)

```css
/* ============================================================================
   SIDEBAR / CONTROL PANEL (GLASSMORPHISM)
   
   Glassmorphism: A modern UI trend that uses translucent backgrounds with 
   background blur (backdrop-filter) to simulate frosted glass.
   Flexbox: display: flex makes it easy to stack elements vertically (flex-direction: column) 
   and add spacing between them (gap).
   ============================================================================ */
.control-panel {
    width: 420px;
    height: 100vh;
    background: var(--bg-panel);
```
**Explanation (Lines 57 - 69):**
- `width: 420px;` locks the sidebar to exactly 420 pixels wide. (Remember from Section 1, the `.main-content` canvas uses `flex-grow: 1` to take up the rest of the screen!).
- `background: var(--bg-panel);` injects the `rgba(..., 0.7)` color. Because it is 70% opaque, you can see the dark background circle behind the sidebar.

```css
    backdrop-filter: blur(20px);
    -webkit-backdrop-filter: blur(20px);
```
**Explanation (Lines 70 - 71):**
- **This is the secret to Glassmorphism!** `backdrop-filter: blur(20px)` tells the browser to blur whatever is *behind* the sidebar. 
- `-webkit-backdrop-filter` is the exact same rule, but specifically included to ensure it works on Apple Safari browsers (which use the WebKit engine).

```css
    border-right: 1px solid var(--border-glass);
    display: flex;
    flex-direction: column;
    padding: 24px;
    gap: 24px;
    box-shadow: 4px 0 24px rgba(0, 0, 0, 0.2);
    z-index: 10;
    overflow-y: auto;
}
```
**Explanation (Lines 72 - 79):**
- `border-right: 1px solid ...` draws a tiny 1px white line on the right edge of the sidebar to define its boundary.
- `display: flex; flex-direction: column;` means everything inside the sidebar will stack vertically on top of each other.
- `gap: 24px;` automatically pushes every item 24 pixels away from each other so they aren't touching.
- `box-shadow` draws a soft black shadow to the right of the panel (`4px 0`) to make it look like it's floating above the canvas.
- `z-index: 10;` forces the sidebar to render *on top* of the canvas just in case they overlap.
- `overflow-y: auto;` allows the user to scroll down the sidebar if their screen is too small to fit all the buttons.

---

## 3. Customizing the Scrollbar (`style.css`)

```css
/* Custom Scrollbar for the sidebar */
.control-panel::-webkit-scrollbar {
    width: 6px;
}
.control-panel::-webkit-scrollbar-track {
    background: transparent;
}
.control-panel::-webkit-scrollbar-thumb {
    background: rgba(255, 255, 255, 0.1);
    border-radius: 10px;
}
.control-panel::-webkit-scrollbar-thumb:hover {
    background: rgba(255, 255, 255, 0.2);
}
```
**Explanation (Lines 81 - 93):**
- Normally, browsers use ugly, thick gray scrollbars. 
- `::-webkit-scrollbar` is a pseudo-element that lets us redesign it entirely.
- We make it very thin (`6px`).
- We make the "track" (the background) invisible (`transparent`).
- We make the "thumb" (the part you drag) a soft, translucent white (`rgba(255, 255, 255, 0.1)`) with rounded corners (`border-radius: 10px`).
- When hovered, we increase the thumb's opacity to `0.2` so it lights up slightly.

---

## 4. Sub-Panels and Dividers (`style.css`)

```css
.panel-section {
    display: flex;
    flex-direction: column;
    gap: 12px;
}
```
**Explanation (Lines 95 - 99):**
- Inside the sidebar, we group buttons into `<section class="panel-section">`. This flexbox vertically stacks the buttons inside that group with a smaller `12px` gap.

```css
.divider {
    height: 1px;
    background: var(--border-glass);
    margin: 8px 0;
}
```
**Explanation (Lines 101 - 105):**
- Used to draw horizontal separator lines between sections.
- `height: 1px;` makes it a tiny line.
- `margin: 8px 0;` adds 8px of empty space above and below the line, and 0px on the left and right.

---

## 5. Typography (`style.css`)

```css
/* ============================================================================
   TYPOGRAPHY
   ============================================================================ */
.panel-header {
    margin-bottom: 8px;
}

.title {
    font-size: 24px;
    font-weight: 700;
    letter-spacing: -0.5px;
    background: linear-gradient(to right, #ffffff, #a5b4fc);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    margin-bottom: 4px;
}
```
**Explanation (Lines 107 - 121):**
- Targets the `<h1 class="title">` tag.
- `font-weight: 700;` makes it extremely bold.
- `letter-spacing: -0.5px;` pulls the letters slightly closer together for a sleek, modern tech look.
- **Gradient Text Trick:** 
  - `background: linear-gradient(...)` paints a gradient box behind the text.
  - `-webkit-background-clip: text;` tells the browser to cut the background box into the shape of the letters!
  - `-webkit-text-fill-color: transparent;` makes the actual font color invisible, allowing the beautiful gradient background to shine through the letter shapes!

```css
.subtitle {
    font-size: 13px;
    color: var(--primary);
    font-weight: 600;
    text-transform: uppercase;
    letter-spacing: 1px;
}
```
**Explanation (Lines 123 - 129):**
- Targets the `<p class="subtitle">` tag.
- `text-transform: uppercase;` forces the text to be ALL CAPS regardless of how it was typed in HTML.
- `letter-spacing: 1px;` spreads the caps out for an elegant subtitle look.

```css
.section-title {
    font-size: 11px;
    font-weight: 600;
    color: var(--text-muted);
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

.helper-text {
    font-size: 12px;
    color: var(--text-muted);
    line-height: 1.5;
}
```
**Explanation (Lines 131 - 143):**
- Styles the small labels above dropdowns and upload boxes.
- `line-height: 1.5;` means the vertical space between lines of text is 1.5 times the font size, making paragraphs much easier to read.
