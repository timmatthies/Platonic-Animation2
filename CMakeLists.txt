cmake_minimum_required(VERSION 3.16)
project(PlatonicAnimation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
include_directories(external/eigen)
include_directories(external/save-bmp)

# Source files
set(SOURCES
    src/main.cpp
    src/Camera.cpp
    src/Object.cpp
    src/ImageGenerator.cpp
    src/Line.cpp
    src/LineSet.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)