# 04. Upload Zone and Status Indicators
*Reference: `visualizer/index.html` (Lines 59 - 88) and `visualizer/style.css` (Lines 346 - 420)*

This document breaks down the HTML and CSS behind the drag-and-drop file uploader and the glowing status dots that tell the user if the data has loaded.

---

## 1. Status Indicators

### The HTML (`index.html`)
```html
            <section class="panel-section">
                <div style="display: flex; justify-content: space-between; align-items: flex-end;">
                    <h2 class="section-title">Data Source</h2>
                    <div class="status-indicator">
                        <span class="status-dot red"></span>
                        <span class="status-label" id="load-status">Graph: Not Loaded</span>
                    </div>
                </div>
```
**Explanation (Lines 60 - 67):**
- Uses a flexbox `div` to push the "Data Source" title to the left, and the `<div class="status-indicator">` to the right.
- `align-items: flex-end;` aligns them at the bottom of the invisible flexbox.
- Inside the indicator, we have an empty `<span>` for the glowing dot, and a second `<span>` for the text.

### The CSS (`style.css`)
```css
/* ============================================================================
   STATUS INDICATORS & UPLOAD ZONE
   ============================================================================ */
.status-indicator {
    display: flex;
    align-items: center;
    gap: 6px;
}
```
**Explanation (Lines 346 - 351):**
- Aligns the dot and the text perfectly horizontally next to each other with a 6px gap.

```css
.status-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    transition: all 0.3s ease;
}

.status-dot.red {
    background-color: var(--accent);
    box-shadow: 0 0 8px var(--accent);
}

.status-dot.green {
    background-color: var(--success);
    box-shadow: 0 0 8px var(--success);
}
```
**Explanation (Lines 353 - 365):**
- An empty HTML `<span>` is normally 0 pixels wide. By explicitly giving it `width: 8px; height: 8px; border-radius: 50%;`, we force it into a small circle.
- The base class just defines the shape. The `.red` and `.green` modifier classes inject the specific colors and drop-shadow glows. 
- *(Remember from the JavaScript documentation: JS simply swaps `.red` to `.green` when `graph.json` loads!)*

```css
.status-label {
    font-size: 11px;
    font-weight: 500;
    color: var(--text-muted);
}
```
**Explanation (Lines 367 - 371):**
- Styles the small "Graph: Not Loaded" text next to the dot.

---

## 2. The Drag-and-Drop Zone

### The HTML (`index.html`)
```html
                <div class="upload-zone" id="drop-zone">
                    <span class="upload-icon">✦</span>
                    <p class="upload-text">Drag & drop <strong>graph.json</strong> or click to browse</p>
                    <p class="helper-text" style="font-size: 10px; margin-top: 4px;">Supports multiple algorithm traces</p>
                    <!-- Hidden file input triggered by JS -->
                    <input type="file" id="file-input" accept=".json" multiple style="display: none;">
                </div>
```
**Explanation (Lines 69 - 75):**
- A large `div` used as the drop target.
- `<input type="file" ... style="display: none;">` is a critically important trick. Standard HTML file inputs are incredibly ugly and impossible to style well with CSS. So, we completely hide it (`display: none`), and use JavaScript to artificially "click" it whenever the user clicks our beautiful custom `.upload-zone` `div`!
- `multiple` allows the user to select more than one JSON file at a time in the File Explorer window.

### The CSS (`style.css`)
```css
.upload-zone {
    border: 2px dashed rgba(99, 102, 241, 0.3);
    border-radius: 12px;
    padding: 24px 16px;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    text-align: center;
    cursor: pointer;
    background: rgba(0, 0, 0, 0.2);
    transition: all 0.3s ease;
}
```
**Explanation (Lines 373 - 385):**
- `border: 2px dashed ...` gives it that classic "drop zone" dotted-line look.
- We use a semi-transparent dark background (`rgba(0, 0, 0, 0.2)`) to make it look slightly indented into the panel.
- `cursor: pointer;` changes the mouse to a hand when hovering over it, signaling to the user that it is clickable.

```css
.upload-zone:hover, .upload-zone.dragover {
    border-color: var(--primary);
    background: rgba(99, 102, 241, 0.08);
}
```
**Explanation (Lines 387 - 390):**
- If the user hovers their mouse (`:hover`) OR if JavaScript adds the class `.dragover` (which happens when dragging a file), we instantly change the dashed border to solid blue and tint the background blue.

```css
.upload-icon {
    font-size: 24px;
    color: var(--primary);
    margin-bottom: 8px;
    filter: drop-shadow(0 0 8px var(--primary-glow));
}

.upload-text {
    font-size: 13px;
    color: var(--text-main);
}
```
**Explanation (Lines 392 - 402):**
- Styles the `✦` icon (making it glow) and the text inside the drop zone.

---

## 3. The Loaded Files List

### The HTML (`index.html`)
```html
                <div class="file-list" id="uploaded-files-list">
                    <!-- Populated by JS -->
                </div>
            </section>
```
**Explanation (Lines 77 - 80):**
- An empty container. JavaScript dynamically injects `<div>`s here whenever a file is successfully processed.

### The CSS (`style.css`)
```css
.file-list {
    display: flex;
    flex-direction: column;
    gap: 4px;
    margin-top: 8px;
}

.file-item {
    display: flex;
    justify-content: space-between;
    font-size: 11px;
    padding: 6px 8px;
    background: rgba(0, 0, 0, 0.1);
    border-radius: 4px;
}

.file-name {
    color: var(--text-muted);
}

.file-status.loaded { color: var(--success); }
.file-status.missing { color: var(--accent); }
```
**Explanation (Lines 404 - 425):**
- The `.file-item` classes match the HTML that JavaScript generates on the fly.
- They form tiny, dark, rounded rows indicating which files have been uploaded.
- The `.loaded` and `.missing` classes apply Green or Red text to the checkmarks/X-marks respectively.
