# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(TSP_SimulatedAnnealing VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------
# --- Manual SFML Configuration (Bypassing find_package) ---
# -----------------------------------------------------------------

# 1. Define the SFML root directory
# Adjust this path if your SFML installation is NOT in C:/msys64/mingw64
# We assume MSYS2 default location:
set(SFML_ROOT "C:/msys64/mingw64")

# 2. Define the Include/Library directories based on the root
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
set(SFML_LIB_DIR "${SFML_ROOT}/lib")

# List all source files
set(SOURCES
    src/main.cpp
    src/City.cpp
    src/Tour.cpp
    src/SimulatedAnnealing.cpp
    src/SolverWindow.cpp
)

# Define the executable target
add_executable(TSP_App ${SOURCES})

# 3. Add SFML Include Paths
target_include_directories(TSP_App PRIVATE 
    ${PROJECT_SOURCE_DIR}/include 
    ${SFML_INCLUDE_DIR}
)

# 4. Link SFML libraries manually (using MinGW names)
target_link_directories(TSP_App PRIVATE ${SFML_LIB_DIR})

target_link_libraries(TSP_App PRIVATE 
    sfml-system 
    sfml-window 
    sfml-graphics
)

# -----------------------------------------------------------------
# --- Post-Build Steps (For running the executable) ---
# -----------------------------------------------------------------

# Set output directory
set_target_properties(TSP_App PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows-specific settings
if(WIN32)
    # Define where the SFML DLLs are located
    set(SFML_BIN_DIR "${SFML_ROOT}/bin")
    
    # Copy each SFML DLL individually (copy_if_different only works with one file at a time)
    add_custom_command(TARGET TSP_App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-system-2.dll"
            $<TARGET_FILE_DIR:TSP_App>
        COMMENT "Copying sfml-system-2.dll"
    )
    
    add_custom_command(TARGET TSP_App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-window-2.dll"
            $<TARGET_FILE_DIR:TSP_App>
        COMMENT "Copying sfml-window-2.dll"
    )
    
    add_custom_command(TARGET TSP_App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-graphics-2.dll"
            $<TARGET_FILE_DIR:TSP_App>
        COMMENT "Copying sfml-graphics-2.dll"
    )
    
    # Copy font file (assuming you placed it in the project root)
    if(EXISTS "${CMAKE_SOURCE_DIR}/arial.ttf")
        add_custom_command(TARGET TSP_App POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/arial.ttf"
                $<TARGET_FILE_DIR:TSP_App>
            COMMENT "Copying arial.ttf to output directory"
        )
    endif()
endif()