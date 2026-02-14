cmake_minimum_required(VERSION 4.1)

set(Project Breakout2D)

project(${Project} LANGUAGES CXX)

set(Sources
    src/program.cpp
    src/game.cpp
    src/game_level.cpp
    src/game_object.cpp
    src/ball.cpp
    src/brick.cpp
    src/event_dispatcher.cpp
    src/particle_emitter.cpp
    src/path_manager.cpp
    src/player.cpp
    src/shader.cpp
    src/shader_manager.cpp
    src/shader_observer.cpp
    src/sprite_renderer.cpp
    src/text_renderer.cpp
    src/texture_2d.cpp
    src/texture_manager.cpp
    src/window.cpp
)

set(Headers
    include/game.hpp
    include/game_level.hpp
    include/game_object.hpp
    include/ball.hpp
    include/brick.hpp
    include/brick_data.hpp
    include/brick_type.hpp
    include/collision_detection.hpp
    include/collision_type.hpp
    include/event_dispatcher.hpp
    include/event_type.hpp
    include/fast_random.hpp
    include/logging.hpp
    include/particle.hpp
    include/particle_emitter.hpp
    include/path_manager.hpp
    include/player.hpp
    include/render_config.hpp
    include/shader.hpp
    include/shader_manager.hpp
    include/shader_observer.hpp
    include/shader_parser.hpp
    include/sprite_renderer.hpp
    include/string_operators.hpp
    include/text_renderer.hpp
    include/texture_2d.hpp
    include/texture_manager.hpp
    include/window.hpp
)

add_executable(${Project}
    ${Sources}
    ${Headers}
)

target_include_directories(${Project} PRIVATE
    include
    ${EXTERNAL_PATH}/includes
)

include(FetchContent)

### stb header files
message(STATUS "Looking for stb lib...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)
target_include_directories(${Project} PRIVATE ${stb_SOURCE_DIR})
###

### glm header files
message(STATUS "Looking for glm lib...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)
target_include_directories(${Project} PRIVATE ${glm_SOURCE_DIR})
###

### nlohmann/json header files
message(STATUS "Looking for nlohmann/json lib...")
FetchContent_Declare(
    hlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(hlohmann_json)
target_include_directories(${Project} PRIVATE ${hlohmann_json_SOURCE_DIR})
###

### GLFW3 lib
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
find_package(glfw3 QUIET)
if (glfw3_FOUND)
    message(STATUS "GLFW3 package was found")
else()
    message(STATUS "GLFW3 package was not found, fetching from repo...")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()
target_link_libraries(${Project} PRIVATE glfw)
###

### Freetype lib
FetchContent_Declare(
    freetype
    URL https://download.savannah.gnu.org/releases/freetype/freetype-2.14.1.tar.gz
    URL_HASH SHA256=174d9e53402e1bf9ec7277e22ec199ba3e55a6be2c0740cb18c0ee9850fc8c34
)
find_package(Freetype QUIET)
if (Freetype_FOUND)
    message(STATUS "Freetype package was found")
    target_link_libraries(${Project} PRIVATE Freetype::Freetype)
else()
    message(STATUS "Freetype package was not found, fetching from repo...")
    set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(freetype)
    target_link_libraries(${Project} PRIVATE freetype)
endif()
###

target_link_libraries(${Project} PRIVATE ${GladLib})

target_precompile_headers(${Project} PRIVATE
    <algorithm>
    <array>
    <atomic>
    <cassert>
    <chrono>
    <cmath>
    <condition_variable>
    <ctime>
    <filesystem>
    <format>
    <fstream>
    <iostream>
    <map>
    <memory>
    <mutex>
    <random>
    <ranges>
    <set>
    <sstream>
    <stdexcept>
    <string>
    <string_view>
    <thread>
    <tuple>
    <type_traits>
    <unordered_map>
    <unordered_set>
    <utility>
    <variant>
    <vector>

    <glad/glad.h>
    <GLFW/glfw3.h>
    <glm/glm.hpp>
    <glm/gtc/matrix_transform.hpp>
    <glm/gtc/type_ptr.hpp>
    <nlohmann/json.hpp>
)

target_compile_definitions(${Project} PRIVATE $<$<CONFIG:Debug>:DEBUG_MODE>)

set(CREATE_PACKAGE NO)
if (${CREATE_PACKAGE})
set(MSYS_BIN "C:/msys64/ucrt64/bin")
set(DLL_LIST
    "${MSYS_BIN}/glfw3.dll"
    "${MSYS_BIN}/libfreetype-6.dll"
    "${MSYS_BIN}/libgcc_s_seh-1.dll"
    "${MSYS_BIN}/libwinpthread-1.dll"
    "${MSYS_BIN}/libstdc++-6.dll"
    "${MSYS_BIN}/libbz2-1.dll"
    "${MSYS_BIN}/libbrotlidec.dll"
    "${MSYS_BIN}/libharfbuzz-0.dll"
    "${MSYS_BIN}/zlib1.dll"
    "${MSYS_BIN}/libpng16-16.dll"
    "${MSYS_BIN}/libbrotlicommon.dll"
    "${MSYS_BIN}/libglib-2.0-0.dll"
    "${MSYS_BIN}/libgraphite2.dll"
    "${MSYS_BIN}/libintl-8.dll"
    "${MSYS_BIN}/libpcre2-8-0.dll"
    "${MSYS_BIN}/libiconv-2.dll"
)

foreach(dll ${DLL_LIST})
    add_custom_command(TARGET ${Project} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" "$<TARGET_FILE_DIR:${Project}>"
    )
endforeach()
install(TARGETS ${Project} DESTINATION .)
install(FILES ${DLL_LIST} DESTINATION .)
install(DIRECTORY ${RESOURCES_DIR} DESTINATION .)

set(CPACK_PACKAGE_NAME ${Project})
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "expool")
set(CPACK_GENERATOR "NSIS")
include(CPack)
endif()
