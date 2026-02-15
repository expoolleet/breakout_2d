cmake_minimum_required(VERSION 3.25.x)

set(Project Breakout2D)

project(${Project} LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # disable modules because of clangd

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_PATH "C:/Program Files/OpenGL") # path to includes(headers) and libs on your system

set(GladLib glad)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external) # using g++ compiled libs
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

add_subdirectory(${GladLib})
add_subdirectory(game)
