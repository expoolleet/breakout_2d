cmake_minimum_required(VERSION 4.1)

set(Project game)

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

target_link_libraries(${Project} PRIVATE
    ${GladLib}
    ${EXTERNAL_DIR}/libglfw3.a
    ${EXTERNAL_DIR}/libfreetype.a
)

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
