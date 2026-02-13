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

find_package(glfw3)
if (glfw3_FOUND)
    message(STATUS "GLFW3 package was found")
    target_link_libraries(${Project} PRIVATE ${EXTERNAL_DIR}/libglfw3.a)
    target_link_libraries(${Project} PRIVATE glfw)
else()
    target_link_libraries(${Project} PRIVATE ${EXTERNAL_DIR}/libglfw3.a)
endif()

find_package(Freetype)
if (Freetype_FOUND)
    message(STATUS "Freetype package was found")
   target_link_libraries(${Project} PRIVATE Freetype::Freetype)
else()
    target_link_libraries(${Project} PRIVATE ${EXTERNAL_DIR}/libfreetype.a)
endif()
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
