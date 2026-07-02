# Visualizer Documentation Navigation

Welcome to the documentation for the Web Visualizer! This folder contains everything you need to understand how the web interface works. 

If you are a C++ developer new to web development, start with the conceptual overviews in the **Web Stack Tutorials**, then read through the HTML and CSS guides, and finally dive into the exhaustive JavaScript and D3.js documentation.

## Conceptual Overviews (Web Stack Tutorials)
* 📖 [JavaScript vs C++ Guide](./web_stack_tutorials/00_JS_vs_CPP.md) - Explains web concepts mapping them to C++ equivalents.
* 🧱 [01. Index HTML Breakdown (Legacy)](./web_stack_tutorials/01_IndexHTML.md) - A high-level overview of the webpage structure.
* 🎨 [02. Style CSS Breakdown (Legacy)](./web_stack_tutorials/02_StyleCSS.md) - A high-level overview of the modern UI styling and CSS variables.
* ⚡ [03. Graph JS Breakdown (Legacy)](./web_stack_tutorials/03_GraphJS.md) - The original, high-level summary of the visualizer logic.

---

## Exhaustive HTML and CSS Documentation
If you want to understand absolutely every single tag in `index.html` and every single property in `style.css`, this section breaks down the webpage into 6 distinct sections and explains them line-by-line!

* 🏗️ [01. Base Layout and Global CSS](./html_css/01_Base_and_Layout.md)
* 🪟 [02. Control Panel & Typography](./html_css/02_ControlPanel_and_Typography.md)
* 🎛️ [03. UI Controls (Dropdowns, Buttons, Sliders)](./html_css/03_UI_Controls.md)
* 📁 [04. Upload Zone and Status Indicators](./html_css/04_Upload_and_Status.md)
* 💻 [05. The Execution Console](./html_css/05_Execution_Console.md)
* ⚛️ [06. The Main Canvas and SVG Physics](./html_css/06_SVG_Canvas.md)

---

## D3.js Complete Course (Ground-Up Tutorial)
D3.js is the powerful physics and rendering engine behind the canvas. This is a ground-up course — start with the prerequisites and read in order!

> **Each file explicitly states which terms are built-in D3/JS keywords and which are variables we invented.**

* 📘 [00. Prerequisites: JavaScript & SVG Math](./d3js/00_Prerequisite_JavaScript_and_SVG.md) ← **Start here!**
* 📘 [00b. The SVG Marker Tag (Arrowheads)](./d3js/00b_SVG_Marker_Tag.md)
* 📘 [01. Initialization and Canvas Setup](./d3js/01_Initialization_and_Canvas.md)
* 📘 [02. Data Binding and The Join Pattern](./d3js/02_Data_Binding_and_The_Join_Pattern.md)
* 📘 [03. The Physics Engine (Force Simulation)](./d3js/03_The_Physics_Engine.md)
* 📘 [03b. Physics vs Geographic Coordinates (`fx` and `fy`)](./d3js/03b_Physics_vs_Geographic_Coordinates.md)
* 📘 [04. Interactivity: Zoom, Pan, and Drag](./d3js/04_Interactivity_Zoom_and_Drag.md)


---

## Exhaustive Line-by-Line JavaScript Documentation
If you want to understand absolutely everything about `graph.js`, this section breaks down all 800+ lines. It also embeds the exact HTML snippets and CSS rules that correspond to each line of JavaScript, showing exactly how the three languages connect!

* 📖 [JavaScript Overview](./javascript/00_Overview.md) 
* 🖥️ [01. Global State and UI Initialization](./javascript/01_GlobalState_and_UI.md)
* 💻 [02. The Execution Console (Terminal)](./javascript/02_ConsoleLogging.md)
* 📂 [03. File Loading and Asynchronous API](./javascript/03_FileLoading.md)
* ⚛️ [04. The D3 Physics Engine](./javascript/04_PhysicsSimulation.md)
* ▶️ [05. Playback and Media Controls](./javascript/05_PlaybackControls.md)
* 🎨 [06. Algorithm Frame Rendering (Colorization)](./javascript/06_AlgorithmRendering.md)
