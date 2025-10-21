# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(TSP_SimulatedAnnealing VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the SFML package
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

# ----------------------------------------------------
# 1. DEFINE TARGET: Creates the executable named TSP_App
# This must be done BEFORE modifying its properties.
add_executable(TSP_App
    src/main.cpp
    src/City.cpp
    src/SimulatedAnnealing.cpp
    src/Tour.cpp
)
# ----------------------------------------------------

# 2. INCLUDE PATHS: Tells the compiler where to find your header files (e.g., "City.h").
# The path points to your 'include' directory.
target_include_directories(TSP_App PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 3. LINK LIBRARIES: Tells the linker to "glue" the SFML code into your executable.
target_link_libraries(TSP_App PRIVATE sfml-system sfml-window sfml-graphics)