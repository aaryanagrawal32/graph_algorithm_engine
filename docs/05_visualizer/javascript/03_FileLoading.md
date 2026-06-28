# 03. File Loading and Asynchronous Programming
*Reference: `visualizer/graph.js` (Lines 160 - 333)*

This document provides a **line-by-line** breakdown of how the visualizer loads JSON files into memory, bypassing browser security restrictions using Drag-and-Drop and the `fetch` API.

---

## 1. Drag-and-Drop UI (Lines 160 - 180)
```javascript
/* ============================================================================
   FILE UPLOAD & DRAG-AND-DROP HANDLERS
   
   Because of browser security (CORS), we cannot use standard C++ file readers 
   (like std::ifstream) on local files. Instead, we use HTML Drag-and-Drop and
   the asynchronous FileReader API.
   ============================================================================ */
function setupDragAndDrop() {
    const dropZone = document.getElementById("drop-zone");
```
**Explanation (Lines 160 - 168):**
- Gets a reference to the HTML `drop-zone` div.

**Corresponding HTML (index.html):**
```html
<div class="upload-zone" id="drop-zone">
    <span class="upload-icon">✦</span>
    <p class="upload-text">Drag & drop <strong>graph.json</strong> or click to browse</p>
    <input type="file" id="file-input" accept=".json" multiple style="display: none;">
</div>
```
**Corresponding CSS (style.css):**
```css
.upload-zone {
    border: 2px dashed rgba(99, 102, 241, 0.3); /* Dashed border */
    transition: all 0.3s ease; /* Smooth hover transition */
}
```

```javascript
    dropZone.addEventListener("dragover", (e) => {
        e.preventDefault();
        dropZone.classList.add("dragover");
    });
```
**Explanation (Lines 170 - 173):**
- Listens for when the user drags a file *over* the box.
- `e.preventDefault()` stops the browser's default behavior (which is to immediately open the file in a new tab, abandoning the page!).
- `.classList.add("dragover")` dynamically adds a CSS class to the HTML element.

**Corresponding CSS (style.css):**
```css
.upload-zone:hover, .upload-zone.dragover {
    border-color: var(--primary); /* Turns the dashed border blue! */
    background: rgba(99, 102, 241, 0.08); /* Adds a light blue background tint */
}
```

```javascript
    dropZone.addEventListener("dragleave", () => {
        dropZone.classList.remove("dragover");
    });
```
**Explanation (Lines 175 - 177):**
- If the user drags a file over the box, but then moves the mouse away, this event fires and removes the `"dragover"` class, turning the box back to gray.

```javascript
    dropZone.addEventListener("drop", (e) => {
        e.preventDefault();
        dropZone.classList.remove("dragover");
        handleFiles(e.dataTransfer.files);
    });
```
**Explanation (Lines 179 - 183):**
- When the user releases the mouse button to drop the file, the `"drop"` event fires.
- `e.preventDefault()` again stops the browser from opening the file in a new tab.
- We remove the blue hover class.
- `e.dataTransfer.files` contains the actual file objects provided by the operating system. We pass this array to `handleFiles()`.

```javascript
    // Click to upload fallback
    const fileInput = document.getElementById("file-input");
    dropZone.addEventListener("click", () => fileInput.click());
    fileInput.addEventListener("change", (e) => handleFiles(e.target.files));
}
```
**Explanation (Lines 185 - 189):**
- Finds the hidden `<input type="file">` tag.
- When the user clicks the `dropZone` box, JavaScript artificially clicks the hidden file input (`fileInput.click()`), which opens the Windows/Mac File Explorer!
- When the user selects files in the Explorer, the `"change"` event fires, and we pass the files to `handleFiles()`.

---

## 2. Asynchronous File Reading (Lines 191 - 211)
```javascript
function handleFiles(files) {
    for (let i = 0; i < files.length; ++i) {
        const file = files[i];
        const reader = new FileReader();
```
**Explanation (Lines 191 - 195):**
- Loops through all files the user dropped.
- `new FileReader()` creates a built-in browser object designed to read local hard drive files into RAM.

```javascript
        reader.onload = (event) => {
            try {
                const data = JSON.parse(event.target.result);
                processJSONData(file.name.toLowerCase(), data);
            } catch (e) {
                logToConsole(`Error parsing ${file.name}: ${e.message}`, "error");
            }
        };
```
**Explanation (Lines 197 - 204):**
- `reader.onload` is a callback. It says: "Wait until the hard drive has finished reading this file. Once it is 100% in RAM, execute this block."
- `event.target.result` contains the raw text string of the file (e.g., `'{"nodes": []}'`).
- `JSON.parse(...)` takes that raw string and instantly converts it into a structured JavaScript Object. 
- If the JSON has a syntax error (like a missing comma), `JSON.parse` will throw an error, which we catch with `try/catch` and log to our terminal.

```javascript
        reader.readAsText(file);
    }
}
```
**Explanation (Lines 206 - 208):**
- `reader.readAsText(file)` tells the `FileReader` to actually start reading the file from the hard drive. 
- Because JavaScript is single-threaded, it starts the read process in the background and immediately moves on to the next file in the `for` loop, keeping the website responsive!

---

## 3. Parsing and Routing Data (Lines 213 - 310)
```javascript
function processJSONData(filename, data) {
    if (filename.includes("graph")) {
        loadedFilesStatus.graph = true;
        loadGraph(data);
        logToConsole("Graph loaded successfully.", "success");
        document.getElementById("load-status").innerText = "Graph: Loaded";
        document.querySelector(".status-dot").className = "status-dot green";
    }
```
**Explanation (Lines 213 - 221):**
- Checks if the string `filename` contains `"graph"` (e.g., `"graph.json"`).
- Updates the `loadedFilesStatus` tracker.
- Calls `loadGraph(data)` to boot up the D3 physics engine (explained in Section 4).
- Updates the HTML status text to "Graph: Loaded" and changes the CSS class of the status dot from red to green.

**Corresponding HTML (index.html):**
```html
<div class="status-indicator">
    <span class="status-dot red"></span>
    <span class="status-label" id="load-status">Graph: Not Loaded</span>
</div>
```
**Corresponding CSS (style.css):**
```css
.status-dot.green {
    background-color: var(--success); /* Green color */
    box-shadow: 0 0 8px var(--success); /* Green glowing drop-shadow */
}
```

```javascript
    else if (filename.includes("bfs")) {
        algorithmData.bfs = data;
        loadedFilesStatus.bfs = true;
        logToConsole("BFS steps loaded.", "success");
    }
    // ... (Lines 228 - 253 repeat this exact logic for DFS, Dijkstra, A*, Kruskal, Tarjan) ...
```
**Explanation (Lines 223 - 253):**
- Checks for specific filenames (like `"bfs"`, `"dfs"`, `"dijkstra"`).
- Saves the parsed JSON object directly into our global `algorithmData` dictionary (e.g., `algorithmData.bfs = data;`).
- Logs success to the terminal.

```javascript
    updateAlgorithmDropdown();
    updateFileListUI();
}
```
**Explanation (Lines 255 - 258):**
- After processing the file, it calls two UI functions to visually update the screen.

```javascript
function updateAlgorithmDropdown() {
    const select = document.getElementById("algorithm-select");
    select.disabled = !loadedFilesStatus.graph;
    
    // Enable/disable individual options based on what files were uploaded
    for (let i = 0; i < select.options.length; i++) {
        const opt = select.options[i];
        if (opt.value && loadedFilesStatus[opt.value] !== undefined) {
            opt.disabled = !loadedFilesStatus[opt.value];
            opt.text = opt.text.replace(" (Not Loaded)", "");
            if (!loadedFilesStatus[opt.value]) {
                opt.text += " (Not Loaded)";
            }
        }
    }
}
```
**Explanation (Lines 260 - 273):**
- Disables the entire dropdown if the main `graph.json` hasn't been loaded yet.
- Loops through every `<option>` in the `<select>` dropdown.
- `opt.value` is the internal ID of the option (e.g., `"bfs"`).
- `opt.disabled = !loadedFilesStatus[opt.value]` instantly grays out the option in the browser if the corresponding JSON file hasn't been uploaded yet!
- It appends the text `"(Not Loaded)"` to the visible option text if missing.

```javascript
function updateFileListUI() {
    const list = document.getElementById("uploaded-files-list");
    list.innerHTML = "";
    
    const requiredFiles = [
        { key: "graph", name: "graph.json" },
        { key: "bfs", name: "bfs_steps.json" },
        // ...
    ];

    requiredFiles.forEach(f => {
        const isLoaded = loadedFilesStatus[f.key];
        const statusClass = isLoaded ? "loaded" : "missing";
        const statusIcon = isLoaded ? "✓" : "✗";
        
        list.innerHTML += `
            <div class="file-item">
                <span class="file-name">${f.name}</span>
                <span class="file-status ${statusClass}">${statusIcon}</span>
            </div>
        `;
    });
}
```
**Explanation (Lines 275 - 310):**
- `list.innerHTML = ""` clears out all existing HTML inside the `uploaded-files-list` container.
- We loop through an array of all expected files.
- Using Template Literals (` \` \` `), we inject raw HTML into the container.
- Notice we dynamically inject `${statusClass}`. This assigns the class `"loaded"` or `"missing"` to the span.

**Corresponding CSS (style.css):**
```css
.file-status.loaded { color: var(--success); } /* Green */
.file-status.missing { color: var(--accent); } /* Pink/Red */
```

---

## 4. Server Auto-Loader (Lines 312 - 333)
```javascript
async function tryAutoLoadFromServer() {
    const files = [
        { name: "graph.json", key: "graph" },
        { name: "bfs_steps.json", key: "bfs" },
        // ...
    ];
```
**Explanation (Lines 312 - 323):**
- `async` marks this function as asynchronous. It allows us to use the `await` keyword inside it.

```javascript
    for (const file of files) {
        try {
            const response = await fetch(file.name);
            if (response.ok) {
                const data = await response.json();
                processJSONData(file.name, data);
            }
        } catch (e) {
            // Silently fail: CORS or 404 is expected under file:// protocol
        }
    }
```
**Explanation (Lines 325 - 335):**
- `fetch(file.name)` is modern JavaScript's network request tool (it sends an HTTP GET request to the local web server to download the file).
- `await fetch(...)` says: "Pause the execution of this specific function here until the download finishes. Let the rest of the website keep running normally."
- `response.ok` checks if the HTTP status is 200 (OK). If we get a 404 (Not Found), it skips it.
- `await response.json()` parses the downloaded text into a JSON object automatically.
- The `try/catch` block prevents the website from crashing if the user opened the file directly from their hard drive (`file://` protocol), which causes `fetch` to throw a CORS security error.
