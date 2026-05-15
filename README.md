# Smart-Timetable-Generator-with-Optimal-Class-Routing
📖 Overview

SmartSched is a console-based C++ system designed to automatically generate conflict-free academic timetables and compute optimal routing paths between locations.

The system applies core principles from discrete mathematics and algorithm design to ensure logical correctness, efficiency, and scalability in scheduling.

🎯 Key Capabilities
Automated timetable generation with zero conflicts
Constraint validation using logical rules
Conflict resolution via graph coloring
Optimized routing using Minimum Spanning Tree algorithms
Coverage verification using Euler and Hamiltonian paths
Recursive backtracking for accurate scheduling
File-based data handling using C++
🧠 Core Techniques
Graph Modeling for conflict detection
Graph Coloring for time slot assignment
Minimum Spanning Tree (Prim’s & Kruskal’s) for route optimization
Backtracking Algorithms for schedule generation
Euler & Hamiltonian Path Algorithms for coverage validation
Logical Constraints for rule enforcement
⚙️ System Workflow
Read structured input data from text files
Construct a conflict graph representing scheduling constraints
Apply graph coloring to assign valid time slots
Validate constraints using logical checks
Compute optimal routes between locations
Verify traversal properties using path algorithms
Export results to output files
📂 File Structure
SmartSched/
│
├── main.cpp
├── input/
│   ├── teachers.txt
│   ├── classes.txt
│   ├── rooms.txt
│   ├── timeslots.txt
│   ├── distances.txt
│   └── constraints.txt
│
├── output/
│   ├── timetable.txt
│   ├── validation_report.txt
│   ├── mst.txt
│   ├── shortest_paths.txt
│   ├── euler_path.txt
│   └── hamilton_path.txt
│
└── README.md
▶️ Execution
Compile
g++ main.cpp -o smartsched
Run
./smartsched
📊 Output

The system generates:

A conflict-free timetable
Validation report for constraints
Optimized routing paths
Graph traversal results (Euler & Hamiltonian)

All outputs are saved as .txt files.

💡 Design Highlights
Modular architecture for scalability
Efficient use of data structures (graphs, sets, mappings)
Algorithm-driven approach ensuring correctness
Fully console-based with file I/O integration
⚠️ Disclaimer

This project is intended for educational and demonstration purposes, showcasing the application of discrete mathematics and algorithmic problem-solving in scheduling systems.
