cmake_minimum_required(VERSION 3.10)

# Set the project name
project(pong VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Manually specify source files
set(SOURCES
    src/main.cpp
    src/controller.cpp
    src/view.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})