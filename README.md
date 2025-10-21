# TSP Solver with Simulated Annealing

A desktop application for solving the Traveling Salesman Problem using the Simulated Annealing algorithm with a modern Qt-based GUI.

## Features
- Interactive city placement
- Visualization of the solution process
- Start, pause, and reset controls
- Distance calculation and display
- Modern UI with intuitive controls

## Requirements
- C++17 compiler (GCC, Clang, or MSVC)
- Qt 6.x
- CMake 3.16 or higher

## Environment Setup

### Windows
1. Install Visual Studio Community with C++ development tools
2. Install Qt 6.x from https://www.qt.io/download
3. Install CMake from https://cmake.org/download/

### macOS
1. Install Xcode command line tools: `xcode-select --install`
2. Install Qt 6.x: `brew install qt`
3. Install CMake: `brew install cmake`

### Linux (Ubuntu/Debian)
1. Install build tools: `sudo apt install build-essential`
2. Install Qt 6.x: `sudo apt install qt6-base-dev`
3. Install CMake: `sudo apt install cmake`

## Building
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage
1. Place cities on the map by clicking or entering coordinates
2. Adjust algorithm parameters if needed
3. Click "Start" to begin the solving process
4. Use "Pause" to temporarily stop and "Reset" to start over

## Console Version
If you want to test the algorithm without GUI, you can compile the console version:

```bash
g++ -std=c++17 src/tsp_solver.cpp src/console_app.cpp -o tsp_solver
./tsp_solver
```

## Algorithm Explanation

The Simulated Annealing algorithm is a probabilistic technique for approximating the global optimum of a given function. In the context of TSP:

1. **Initialization**: Start with a random tour and high temperature
2. **Iteration**: Generate neighbor solutions by swapping cities
3. **Acceptance**: Accept better solutions always, worse solutions with probability based on temperature
4. **Cooling**: Gradually reduce temperature to converge to optimal solution

### Parameters
- **Initial Temperature**: Higher values allow more exploration
- **Cooling Rate**: Controls how quickly temperature decreases (0.8-0.999)
- **Minimum Temperature**: Stopping condition
- **Maximum Iterations**: Alternative stopping condition

### Advantages
- Can escape local optima
- Good for large problem instances
- Simple to implement and modify

### Comparison with Other Algorithms

1. **Nearest Neighbor**:
   - Fast but often suboptimal
   - No ability to improve solution
   - Greedy approach

2. **Genetic Algorithm**:
   - Population-based approach
   - More complex implementation
   - Can be slower to converge

Simulated Annealing provides a good balance between solution quality and implementation complexity for the TSP.