cmake_minimum_required(VERSION 3.31.x)

set(Project Breakout2D)

project(${Project} LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # disable modules because of clangd

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# path to includes and libs on your system
if(WIN32)
    set(SYSTEM_EXTERNAL_PATH "C:/Program Files/OpenGL")
endif()

if(UNIX)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  set(CMAKE_INSTALL_RPATH "\$ORIGIN/libs")
endif()

set(GladLib glad)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

add_subdirectory(${GladLib})
add_subdirectory(game)
