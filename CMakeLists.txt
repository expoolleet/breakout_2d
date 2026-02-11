cmake_minimum_required(VERSION 4.1)

set(Project Breakout2D)

project(${Project} LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # disable modules because clangd yells

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_PATH "C:/Program Files/OpenGL") # path to includes(headers) and libs on your system

set(GladLib glad)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external) # using g++ compiled libs

add_subdirectory(${GladLib})
add_subdirectory(game)
