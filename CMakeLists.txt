# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(TSP_SimulatedAnnealing VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SFML Configuration ---

# 1. Manually set the path to SFML if necessary (Keep this line if CMake fails to find SFML automatically)
#    You must ensure this path is correct for your system (e.g., C:/libs/SFML/lib/cmake/SFML)
set(SFML_DIR "C:/libs/SFML/lib/cmake/SFML" CACHE PATH "Path to SFML CMake config file") 

# 2. Find the SFML package, requesting specific components
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# 3. List all source files in one place
set(SOURCES
    src/main.cpp
    src/City.cpp
    src/Tour.cpp
    src/SimulatedAnnealing.cpp
    src/SolverWindow.cpp # Include the GUI file
)

# ----------------------------------------------------
# 4. DEFINE THE EXECUTABLE TARGET ONCE
# ----------------------------------------------------
add_executable(TSP_App ${SOURCES})

# 5. INCLUDE PATHS: Tells the compiler where to find your header files (e.g., "City.h").
target_include_directories(TSP_App PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ----------------------------------------------------
# 6. LINK LIBRARIES: Use the older, direct library names (The reliable MinGW method)
# ----------------------------------------------------
target_link_libraries(TSP_App PRIVATE 
    sfml-system 
    sfml-window 
    sfml-graphics
)

# ----------------------------------------------------