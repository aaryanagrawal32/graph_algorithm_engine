/* ============================================================================
   GRAPH ENGINE VISUALIZER - D3.js ANIMATION CORE
   ============================================================================ */

// Global State
let graphData = null;       // Holds { nodes: [...], links: [...] }
let algorithmData = {
    bfs: null,             // { steps: [...] }
    dfs: null,             // { steps: [...] }
    dijkstra: null,        // { steps: [...] } (shortest path nodes)
    astar: null,           // { steps: [...] } (shortest path nodes)
    kruskal: null,         // { mst: [...] } (list of edges)
    tarjan: null           // { sccs: [...] } (list of component lists)
};

// Simulation State
let activeAlgorithm = "";
let currentStep = 0;
let maxSteps = 0;
let isPlaying = false;
let playbackInterval = null;
let playbackSpeed = 500; // ms per step

// D3 Selections & Simulation
let svg, containerG, linkG, nodeG;
let simulation = null;
let nodeSelection = null;
let linkSelection = null;
let labelSelection = null;
let weightSelection = null;
let zoomBehavior = null;
let showLabels = true;

// File Upload Tracker
const loadedFilesStatus = {
    graph: false,
    bfs: false,
    dfs: false,
    dijkstra: false,
    astar: false,
    kruskal: false,
    tarjan: false
};

/* ============================================================================
   INITIALIZATION & DOM EVENTS
   ============================================================================ */
document.addEventListener("DOMContentLoaded", () => {
    setupUI();
    setupSVG();
    setupDragAndDrop();
    
    // Attempt auto-loading from server (fails gracefully if opened via file://)
    tryAutoLoadFromServer();
});

function setupUI() {
    // Dropdown events
    const algoSelect = document.getElementById("algorithm-select");
    algoSelect.addEventListener("change", (e) => {
        setAlgorithm(e.target.value);
    });

    // Control Buttons
    document.getElementById("btn-play").addEventListener("click", togglePlay);
    document.getElementById("btn-next").addEventListener("click", stepForward);
    document.getElementById("btn-prev").addEventListener("click", stepBackward);
    document.getElementById("btn-reset").addEventListener("click", resetSimulation);

    // Speed Slider
    const speedSlider = document.getElementById("speed-slider");
    const speedVal = document.getElementById("speed-value");
    speedSlider.addEventListener("input", (e) => {
        playbackSpeed = parseInt(e.target.value);
        speedVal.innerText = `${(1000 / playbackSpeed).toFixed(1)}x`;
        if (isPlaying) {
            pause();
            play();
        }
    });

    // Zoom Buttons
    document.getElementById("btn-zoom-in").addEventListener("click", () => {
        svg.transition().duration(300).call(zoomBehavior.scaleBy, 1.3);
    });
    document.getElementById("btn-zoom-out").addEventListener("click", () => {
        svg.transition().duration(300).call(zoomBehavior.scaleBy, 1/1.3);
    });
    document.getElementById("btn-zoom-fit").addEventListener("click", fitGraphToScreen);
    
    // Toggle Labels
    const btnLabels = document.getElementById("btn-toggle-labels");
    btnLabels.addEventListener("click", () => {
        showLabels = !showLabels;
        btnLabels.classList.toggle("active", showLabels);
        svg.selectAll(".node-label").style("opacity", showLabels ? 1 : 0);
    });
}

function setupSVG() {
    svg = d3.select("#graph-svg");
    containerG = svg.append("g").attr("class", "zoom-container");
    
    // Define markers for directed edge arrowheads
    svg.append("defs").append("marker")
        .attr("id", "arrowhead")
        .attr("viewBox", "0 -5 10 10")
        .attr("refX", 22) // Place arrowhead just at the border of node circle (radius 15 + padding)
        .attr("refY", 0)
        .attr("markerWidth", 6)
        .attr("markerHeight", 6)
        .attr("orient", "auto")
        .append("path")
        .attr("d", "M0,-4L9,0L0,4")
        .attr("class", "arrowhead");

    svg.append("defs").append("marker")
        .attr("id", "arrowhead-path")
        .attr("viewBox", "0 -5 10 10")
        .attr("refX", 22)
        .attr("refY", 0)
        .attr("markerWidth", 6)
        .attr("markerHeight", 6)
        .attr("orient", "auto")
        .append("path")
        .attr("d", "M0,-4L9,0L0,4")
        .attr("class", "arrowhead path-arrow");

    linkG = containerG.append("g").attr("class", "links-group");
    nodeG = containerG.append("g").attr("class", "nodes-group");

    // Setup zoom & pan
    zoomBehavior = d3.zoom()
        .scaleExtent([0.1, 4])
        .on("zoom", (event) => {
            containerG.attr("transform", event.transform);
        });

    svg.call(zoomBehavior);
}

/* ============================================================================
   CONSOLE / TERMINAL HELPERS
   ============================================================================ */
function logToConsole(message, type = "info") {
    const consoleLog = document.getElementById("console-log");
    const line = document.createElement("div");
    line.className = `console-line ${type}`;
    
    const timestamp = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    line.innerHTML = `<span style="color: #6B7280; font-size: 9px; margin-right: 6px;">[${timestamp}]</span>${message}`;
    
    consoleLog.appendChild(line);
    consoleLog.scrollTop = consoleLog.scrollHeight;
}

/* ============================================================================
   FILE UPLOAD & DRAG-AND-DROP HANDLERS
   ============================================================================ */
function setupDragAndDrop() {
    const dropZone = document.getElementById("drop-zone");
    const fileInput = document.getElementById("file-input");

    dropZone.addEventListener("click", () => fileInput.click());
    
    dropZone.addEventListener("dragover", (e) => {
        e.preventDefault();
        dropZone.classList.add("dragover");
    });

    dropZone.addEventListener("dragleave", () => {
        dropZone.classList.remove("dragover");
    });

    dropZone.addEventListener("drop", (e) => {
        e.preventDefault();
        dropZone.classList.remove("dragover");
        handleFiles(e.dataTransfer.files);
    });

    fileInput.addEventListener("change", (e) => {
        handleFiles(e.target.files);
    });
}

function handleFiles(files) {
    for (let i = 0; i < files.length; ++i) {
        const file = files[i];
        const name = file.name.toLowerCase();
        const reader = new FileReader();

        reader.onload = (event) => {
            try {
                const data = JSON.parse(event.target.result);
                processJSONData(name, data);
            } catch (err) {
                logToConsole(`Error parsing ${file.name}: ${err.message}`, "error");
            }
        };
        reader.readAsText(file);
    }
}

function processJSONData(filename, data) {
    if (filename.includes("graph.json")) {
        loadGraph(data);
        loadedFilesStatus.graph = true;
        logToConsole("Loaded graph.json structure successfully.", "success");
    } else if (filename.includes("bfs_steps.json")) {
        algorithmData.bfs = data;
        loadedFilesStatus.bfs = true;
        logToConsole("Loaded bfs_steps.json trace.", "success");
    } else if (filename.includes("dfs_steps.json")) {
        algorithmData.dfs = data;
        loadedFilesStatus.dfs = true;
        logToConsole("Loaded dfs_steps.json trace.", "success");
    } else if (filename.includes("dijkstra_path.json")) {
        algorithmData.dijkstra = data;
        loadedFilesStatus.dijkstra = true;
        logToConsole("Loaded dijkstra_path.json trace.", "success");
    } else if (filename.includes("astar_path.json")) {
        algorithmData.astar = data;
        loadedFilesStatus.astar = true;
        logToConsole("Loaded astar_path.json trace.", "success");
    } else if (filename.includes("kruskal_mst.json")) {
        algorithmData.kruskal = data;
        loadedFilesStatus.kruskal = true;
        logToConsole("Loaded kruskal_mst.json trace.", "success");
    } else if (filename.includes("tarjan_sccs.json")) {
        algorithmData.tarjan = data;
        loadedFilesStatus.tarjan = true;
        logToConsole("Loaded tarjan_sccs.json trace.", "success");
    } else {
        logToConsole(`Unrecognized JSON file ignored: ${filename}`, "warning");
    }

    updateFilesUI();
}

function updateFilesUI() {
    const list = document.getElementById("uploaded-files-list");
    list.innerHTML = "";

    const addStatus = (label, isLoaded) => {
        const item = document.createElement("div");
        item.className = "file-item";
        item.innerHTML = `
            <span class="file-name">${label}</span>
            <span class="file-status ${isLoaded ? 'loaded' : 'missing'}">${isLoaded ? '✓ Active' : '✗ Missing'}</span>
        `;
        list.appendChild(item);
    };

    addStatus("graph.json", loadedFilesStatus.graph);
    addStatus("bfs_steps.json", loadedFilesStatus.bfs);
    addStatus("dfs_steps.json", loadedFilesStatus.dfs);
    addStatus("dijkstra_path.json", loadedFilesStatus.dijkstra);
    addStatus("astar_path.json", loadedFilesStatus.astar);
    addStatus("kruskal_mst.json", loadedFilesStatus.kruskal);
    addStatus("tarjan_sccs.json", loadedFilesStatus.tarjan);

    // Enable algorithm selector if graph is loaded
    const select = document.getElementById("algorithm-select");
    const statusLabel = document.getElementById("load-status");

    if (loadedFilesStatus.graph) {
        select.disabled = false;
        statusLabel.innerText = `Graph: ${graphData.nodes.length} Nodes, ${graphData.links.length} Edges`;
        statusLabel.parentNode.querySelector(".status-dot").className = "status-dot green";
    } else {
        select.disabled = true;
        statusLabel.innerText = "Graph: Not Loaded";
        statusLabel.parentNode.querySelector(".status-dot").className = "status-dot red";
    }
}

// Attempts to load all JSON files directly from the server
async function tryAutoLoadFromServer() {
    logToConsole("Attempting to auto-load files from workspace server...", "system");
    const files = [
        { name: "graph.json", key: "graph" },
        { name: "bfs_steps.json", key: "bfs" },
        { name: "dfs_steps.json", key: "dfs" },
        { name: "dijkstra_path.json", key: "dijkstra" },
        { name: "astar_path.json", key: "astar" },
        { name: "kruskal_mst.json", key: "kruskal" },
        { name: "tarjan_sccs.json", key: "tarjan" }
    ];

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
    
    if (loadedFilesStatus.graph) {
        logToConsole("Auto-load complete. Ready to visualize.", "success");
    } else {
        logToConsole("Auto-load failed or incomplete (normal for direct file:// openings). Please drag & drop the generated JSON files into the drop zone.", "warning");
    }
}

/* ============================================================================
   D3 FORCE DIRECTED SIMULATION
   ============================================================================ */
function loadGraph(data) {
    graphData = data;
    
    // Stop previous simulation
    if (simulation) simulation.stop();

    // Clear svg elements
    linkG.selectAll("*").remove();
    nodeG.selectAll("*").remove();

    // If nodes have coordinate coordinates, scale them to fit nicely on the canvas
    const hasCoordinates = graphData.nodes.length > 0 && graphData.nodes[0].x !== undefined;
    
    if (hasCoordinates) {
        // Find min/max coordinate ranges
        const minX = d3.min(graphData.nodes, d => d.x);
        const maxX = d3.max(graphData.nodes, d => d.x);
        const minY = d3.min(graphData.nodes, d => d.y);
        const maxY = d3.max(graphData.nodes, d => d.y);

        const svgBounds = document.getElementById("graph-svg").getBoundingClientRect();
        const padding = 120;
        const width = svgBounds.width - padding * 2;
        const height = svgBounds.height - padding * 2;

        const scaleX = width / (maxX - minX || 1);
        const scaleY = height / (maxY - minY || 1);
        
        // Scale and map nodes
        graphData.nodes.forEach(node => {
            node.fx = padding + (node.x - minX) * scaleX;
            node.fy = padding + (node.y - minY) * scaleY;
        });
    }

    // Initialize Force layout
    simulation = d3.forceSimulation(graphData.nodes)
        .force("link", d3.forceLink(graphData.links).id(d => d.id).distance(120))
        .force("charge", d3.forceManyBody().strength(hasCoordinates ? -50 : -350))
        .force("collide", d3.forceCollide().radius(25))
        .force("center", d3.forceCenter(
            document.getElementById("graph-svg").clientWidth / 2,
            document.getElementById("graph-svg").clientHeight / 2
        ));

    // Render Links
    linkSelection = linkG.selectAll(".graph-link")
        .data(graphData.links)
        .enter().append("line")
        .attr("class", "graph-link")
        .attr("stroke-width", 2)
        .attr("marker-end", "url(#arrowhead)"); // Use arrowhead marker

    // Render Link Weights
    weightSelection = linkG.selectAll(".link-weight")
        .data(graphData.links)
        .enter().append("text")
        .attr("class", "link-weight")
        .attr("fill", "#64748B")
        .attr("font-size", "9px")
        .attr("font-weight", "500")
        .attr("text-anchor", "middle")
        .text(d => d.weight);

    // Render Nodes
    nodeSelection = nodeG.selectAll(".node-group")
        .data(graphData.nodes)
        .enter().append("g")
        .attr("class", "node-group")
        .call(d3.drag()
            .on("start", dragstarted)
            .on("drag", dragged)
            .on("end", dragended));

    nodeSelection.append("circle")
        .attr("class", "node-circle")
        .attr("r", 15);

    // Render Node Labels (IDs)
    labelSelection = nodeSelection.append("text")
        .attr("class", "node-label")
        .attr("text-anchor", "middle")
        .attr("dy", ".3em")
        .text(d => d.id);

    // Bind tick event
    simulation.on("tick", () => {
        linkSelection
            .attr("x1", d => d.source.x)
            .attr("y1", d => d.source.y)
            .attr("x2", d => d.target.x)
            .attr("y2", d => d.target.y);

        weightSelection
            .attr("x", d => (d.source.x + d.target.x) / 2)
            .attr("y", d => (d.source.y + d.target.y) / 2 - 4);

        nodeSelection.attr("transform", d => `translate(${d.x}, ${d.y})`);
    });

    // Run simulation a bit and fit to screen
    setTimeout(() => {
        fitGraphToScreen();
    }, 200);
}

/* ============================================================================
   ZOOM & DRAG HELPERS
   ============================================================================ */
function dragstarted(event, d) {
    if (!event.active && simulation) simulation.alphaTarget(0.3).restart();
    d.fx = d.x;
    d.fy = d.y;
}

function dragged(event, d) {
    d.fx = event.x;
    d.fy = event.y;
}

function dragended(event, d) {
    if (!event.active && simulation) simulation.alphaTarget(0);
    // If graph has fixed coordinates, keep it pinned, otherwise release
    const hasCoordinates = graphData.nodes.length > 0 && graphData.nodes[0].x !== undefined;
    if (!hasCoordinates) {
        d.fx = null;
        d.fy = null;
    }
}

function fitGraphToScreen() {
    if (!graphData || graphData.nodes.length === 0) return;

    const bounds = containerG.node().getBBox();
    const parent = svg.node();
    const fullWidth = parent.clientWidth;
    const fullHeight = parent.clientHeight;
    
    const width = bounds.width;
    const height = bounds.height;
    const midX = bounds.x + width / 2;
    const midY = bounds.y + height / 2;
    
    if (width === 0 || height === 0) return; 
    
    const scale = 0.85 / Math.max(width / fullWidth, height / fullHeight);
    const translate = [fullWidth / 2 - scale * midX, fullHeight / 2 - scale * midY];

    svg.transition().duration(750).call(
        zoomBehavior.transform,
        d3.zoomIdentity.translate(translate[0], translate[1]).scale(scale)
    );
}

/* ============================================================================
   ALGORITHM SELECTION & CONTROL
   ============================================================================ */
function setAlgorithm(algo) {
    pause();
    
    // Check if the requested algorithm trace is loaded
    if (!algorithmData[algo]) {
        logToConsole(`Algorithm trace not loaded for "${algo.toUpperCase()}"! Run main.cpp and upload the generated steps file.`, "error");
        document.getElementById("algorithm-select").value = activeAlgorithm || "";
        return;
    }

    activeAlgorithm = algo;
    currentStep = 0;
    
    // Set up step bounds based on algorithm data format
    if (algo === "bfs" || algo === "dfs") {
        maxSteps = algorithmData[algo].steps.length;
    } else if (algo === "dijkstra" || algo === "astar") {
        maxSteps = algorithmData[algo].path ? algorithmData[algo].path.length : 0;
    } else if (algo === "kruskal") {
        maxSteps = algorithmData[algo].mst ? algorithmData[algo].mst.length : 0;
    } else if (algo === "tarjan") {
        maxSteps = algorithmData[algo].sccs ? algorithmData[algo].sccs.length : 0;
    }

    logToConsole(`Switched to ${algo.toUpperCase()}. Total steps: ${maxSteps}`, "system");

    // Enable/disable buttons
    document.getElementById("btn-play").disabled = maxSteps === 0;
    document.getElementById("btn-next").disabled = maxSteps === 0;
    document.getElementById("btn-prev").disabled = true;
    document.getElementById("btn-reset").disabled = false;

    // Show path node selector only for pathfinding
    const pathGroup = document.querySelector(".path-nodes-select");
    if (algo === "dijkstra" || algo === "astar") {
        pathGroup.style.display = "flex";
        populatePathSelector();
    } else {
        pathGroup.style.display = "none";
    }

    updateLegend();
    resetVisualStates();
}

function populatePathSelector() {
    const startSelect = document.getElementById("start-node-select");
    const endSelect = document.getElementById("end-node-select");
    
    startSelect.innerHTML = "";
    endSelect.innerHTML = "";

    // Sort nodes alphabetically or numerically
    const sortedNodes = [...graphData.nodes].sort((a, b) => {
        const numA = parseInt(a.id);
        const numB = parseInt(b.id);
        if (!isNaN(numA) && !isNaN(numB)) return numA - numB;
        return a.id.localeCompare(b.id);
    });

    sortedNodes.forEach(node => {
        const opt1 = document.createElement("option");
        opt1.value = node.id;
        opt1.innerText = node.id;
        startSelect.appendChild(opt1);

        const opt2 = document.createElement("option");
        opt2.value = node.id;
        opt2.innerText = node.id;
        endSelect.appendChild(opt2);
    });

    // Default selection matching main.cpp
    startSelect.value = "0";
    endSelect.value = "11";
}

function updateLegend() {
    const list = document.getElementById("legend-items-list");
    list.innerHTML = "";

    const addLegend = (colorClass, label) => {
        const item = document.createElement("div");
        item.className = "legend-item";
        item.innerHTML = `<span class="legend-color ${colorClass}"></span> ${label}`;
        list.appendChild(item);
    };

    addLegend("node-default", "Default Node");
    addLegend("edge-default", "Unvisited Edge");

    if (activeAlgorithm === "bfs" || activeAlgorithm === "dfs") {
        addLegend("node-circle visited", "Visited Node");
        addLegend("node-circle active", "Current Active Node");
    } else if (activeAlgorithm === "dijkstra" || activeAlgorithm === "astar") {
        addLegend("node-circle visited", "Explored Nodes");
        addLegend("node-circle path-node", "Shortest Path Node");
        addLegend("graph-link path-link", "Shortest Path Edge");
    } else if (activeAlgorithm === "kruskal") {
        addLegend("graph-link mst-link", "Selected MST Edge");
    } else if (activeAlgorithm === "tarjan") {
        addLegend("node-circle scc-0", "Component 1");
        addLegend("node-circle scc-1", "Component 2");
        addLegend("node-circle scc-2", "Component 3");
        addLegend("node-circle scc-3", "Component 4");
    }
}

/* ============================================================================
   PLAYBACK ENGINE
   ============================================================================ */
function togglePlay() {
    if (isPlaying) {
        pause();
    } else {
        play();
    }
}

function play() {
    if (currentStep >= maxSteps) {
        currentStep = 0;
    }
    
    isPlaying = true;
    const btn = document.getElementById("btn-play");
    btn.innerText = "⏸";
    btn.className = "control-btn play paused";
    
    logToConsole("Playback started.", "system");

    playbackInterval = setInterval(() => {
        if (currentStep < maxSteps) {
            stepForward();
        } else {
            pause();
        }
    }, playbackSpeed);
}

function pause() {
    isPlaying = false;
    const btn = document.getElementById("btn-play");
    btn.innerText = "▶";
    btn.className = "control-btn play";
    
    if (playbackInterval) {
        clearInterval(playbackInterval);
        playbackInterval = null;
    }
    logToConsole("Playback paused.", "system");
}

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

function stepBackward() {
    if (currentStep <= 0) return;

    currentStep--;
    updateSimulationFrame();

    document.getElementById("btn-next").disabled = false;
    if (currentStep <= 0) {
        document.getElementById("btn-prev").disabled = true;
    }
}

function resetSimulation() {
    pause();
    currentStep = 0;
    resetVisualStates();
    
    document.getElementById("btn-prev").disabled = true;
    document.getElementById("btn-next").disabled = maxSteps === 0;
    logToConsole("Simulation reset.", "system");
}

function resetVisualStates() {
    svg.selectAll(".node-circle")
        .attr("class", "node-circle")
        .attr("style", null);
        
    svg.selectAll(".graph-link")
        .attr("class", "graph-link")
        .attr("marker-end", "url(#arrowhead)");
}

/* ============================================================================
   FRAME RENDER LOGIC BY ALGORITHM
   ============================================================================ */
function updateSimulationFrame() {
    resetVisualStates();

    if (activeAlgorithm === "bfs" || activeAlgorithm === "dfs") {
        renderBFSDFSFrame();
    } else if (activeAlgorithm === "dijkstra" || activeAlgorithm === "astar") {
        renderPathfindingFrame();
    } else if (activeAlgorithm === "kruskal") {
        renderKruskalFrame();
    } else if (activeAlgorithm === "tarjan") {
        renderTarjanFrame();
    }
}

function renderBFSDFSFrame() {
    const steps = algorithmData[activeAlgorithm].steps;
    
    // Highlight all visited up to currentStep - 1
    const visited = new Set(steps.slice(0, currentStep));
    const activeNode = currentStep > 0 ? steps[currentStep - 1] : null;

    svg.selectAll(".node-circle")
        .classed("visited", d => visited.has(d.id.toString()) && d.id.toString() !== activeNode)
        .classed("active", d => d.id.toString() === activeNode);

    if (activeNode) {
        logToConsole(`Step ${currentStep}/${maxSteps}: Visiting Node <strong>${activeNode}</strong>`, "success");
    }
}

function renderPathfindingFrame() {
    const path = algorithmData[activeAlgorithm].path;
    if (!path || path.length === 0) return;

    // Reveal path nodes up to currentStep
    const pathNodes = new Set(path.slice(0, currentStep));

    svg.selectAll(".node-circle")
        .classed("path-node", d => pathNodes.has(d.id.toString()));

    // Highlight path links connecting those nodes
    svg.selectAll(".graph-link")
        .classed("path-link", d => {
            const srcId = d.source.id.toString();
            const dstId = d.target.id.toString();
            
            // Check if both endpoints are in our path and are adjacent in the path list
            const srcIndex = path.indexOf(srcId);
            const dstIndex = path.indexOf(dstId);
            
            return srcIndex !== -1 && dstIndex !== -1 && 
                   dstIndex === srcIndex + 1 && 
                   srcIndex < currentStep - 1 && dstIndex < currentStep;
        })
        .attr("marker-end", function(d) {
            return d3.select(this).classed("path-link") ? "url(#arrowhead-path)" : "url(#arrowhead)";
        });

    if (currentStep > 0) {
        const currNode = path[currentStep - 1];
        if (currentStep === maxSteps) {
            logToConsole(`Step ${currentStep}/${maxSteps}: Destination reached! Shortest path completely highlighted.`, "success");
        } else {
            logToConsole(`Step ${currentStep}/${maxSteps}: Path extends to Node <strong>${currNode}</strong>`, "info");
        }
    }
}

function renderKruskalFrame() {
    const mst = algorithmData.kruskal.mst;
    if (!mst || mst.length === 0) return;

    // Build a set of active MST links up to currentStep
    const activeMSTLinks = new Set();
    for (let i = 0; i < currentStep; ++i) {
        const edge = mst[i];
        // Store as undirected keys to match regardless of source/target order
        const key1 = `${edge.source}-${edge.target}`;
        const key2 = `${edge.target}-${edge.source}`;
        activeMSTLinks.add(key1);
        activeMSTLinks.add(key2);
    }

    svg.selectAll(".graph-link")
        .classed("mst-link", d => {
            const key = `${d.source.id}-${d.target.id}`;
            return activeMSTLinks.has(key);
        });

    if (currentStep > 0) {
        const lastEdge = mst[currentStep - 1];
        logToConsole(`Step ${currentStep}/${maxSteps}: Selected MST Edge <strong>${lastEdge.source} &mdash; ${lastEdge.target}</strong> (weight: ${lastEdge.weight})`, "success");
    }
}

function renderTarjanFrame() {
    const sccs = algorithmData.tarjan.sccs;
    if (!sccs || sccs.length === 0) return;

    // Map each node to its SCC index if its component has been revealed
    const nodeSCCMap = new Map();
    for (let c = 0; c < currentStep; ++c) {
        sccs[c].forEach(nodeId => {
            nodeSCCMap.set(nodeId.toString(), c);
        });
    }

    // Color code nodes according to their SCC class
    svg.selectAll(".node-circle")
        .attr("class", d => {
            const sccIndex = nodeSCCMap.get(d.id.toString());
            if (sccIndex !== undefined) {
                return `node-circle scc-${sccIndex % 8}`; // Loop color choices
            }
            return "node-circle";
        });

    if (currentStep > 0) {
        const componentNodes = sccs[currentStep - 1];
        logToConsole(`Step ${currentStep}/${maxSteps}: Identified Strongly Connected Component: { <strong>${componentNodes.join(", ")}</strong> }`, "success");
    }
}
