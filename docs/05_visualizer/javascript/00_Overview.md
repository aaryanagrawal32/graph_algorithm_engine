# 00. The Web Trinity: HTML, CSS, and JS

Welcome to the JavaScript Deep Dive! This folder contains a line-by-line explanation of the entire web visualizer, broken down section by section. Before diving into the specific parts of the code, it's important to understand how web development differs from C++ development.

## 1. The Web Trinity
In C++, you usually write everything (logic, graphics, window management) in one place using a library like SFML or OpenGL. In web development, responsibilities are strictly separated into three languages:

1. **HTML (The Structure):** Defines *what* is on the page. It's like the scaffolding of a house. You use tags like `<button>`, `<div>` (a generic box), and `<svg>` (a math canvas) to create elements.
2. **CSS (The Styling):** Defines *how* things look. It's the paint and interior design. You write rules like "make all buttons blue" or "give this box a glowing drop-shadow." 
3. **JavaScript (The Logic):** Defines *behavior*. It's the electricity and plumbing. JS is used to fetch data, handle button clicks, run math simulations, and dynamically tell HTML and CSS to update.

> [!IMPORTANT]
> **The Golden Rule of this Visualizer**: Our JavaScript almost **never** manually styles elements (e.g., it does not say "turn this circle red"). Instead, JavaScript simply adds or removes CSS *classes* (labels) on HTML elements. The CSS engine sees the new label and instantly applies the pre-written styling. This separation keeps the code extremely clean!

## 2. JavaScript Execution Model
As a C++ developer, you are used to multithreading (`std::thread`), manual memory management (`new`/`delete`), and blocking `while` loops for game logic.

JavaScript is different:
- **Single-Threaded**: All JavaScript runs on a single main thread. You cannot block this thread with a `while(true)` loop, or the entire web page will freeze!
- **Event-Driven**: Instead of a massive `switch` statement polling for input inside a game loop, JS uses an **Event Loop**. You tell the browser: "When the user clicks this button, run this function." The browser handles the rest.
- **Asynchronous**: Because JS is single-threaded, slow operations (like reading a file or waiting for a timer) must happen in the background. We use terms like `async`/`await` and Callbacks to say: "Start this task, and when it finishes, run this block of code."

## 3. Reading Guide
To understand how the visualizer works, read the files in this folder in order. They map directly to the sections of `visualizer/graph.js`:

1. **[01_GlobalState_and_UI.md](./01_GlobalState_and_UI.md)**: How the app starts up and listens for user input.
2. **[02_ConsoleLogging.md](./02_ConsoleLogging.md)**: How we build a fake "terminal" inside the web page.
3. **[03_FileLoading.md](./03_FileLoading.md)**: How we read the C++ JSON files using drag-and-drop.
4. **[04_PhysicsSimulation.md](./04_PhysicsSimulation.md)**: The math behind the D3 force-directed graph.
5. **[05_PlaybackControls.md](./05_PlaybackControls.md)**: How the animation timer (Play/Pause) works.
6. **[06_AlgorithmRendering.md](./06_AlgorithmRendering.md)**: How we change colors to visualize algorithms.
