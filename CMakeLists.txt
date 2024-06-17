cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MySDL2Project VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include directories
include_directories(${SDL2_INCLUDE_DIRS} include)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2 library
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

