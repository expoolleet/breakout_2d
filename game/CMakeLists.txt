cmake_minimum_required(VERSION 3.31.x)

set(Project Breakout2D)

set(CREATE_PACKAGE NO)

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
    src/canvas.cpp
    src/powerup.cpp
    src/powerup_factory.cpp
    src/audio_manager.cpp
    src/game_level_generator.cpp
    src/engine_context.cpp
)

add_executable(${Project}
    ${Sources}
)

target_include_directories(${Project} PRIVATE
    include
    ${EXTERNAL_DIR}/fmod/core/inc
    ${EXTERNAL_DIR}/fmod/studio/inc
)

set(FMOD_CORE_LIB_PATH ${EXTERNAL_DIR}/fmod/core/lib/x64)
set(FMOD_STUDIO_LIB_PATH ${EXTERNAL_DIR}/fmod/studio/lib/x64)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
message(STATUS "Linking static import libraries (.lib)...")
target_link_libraries(${Project} PRIVATE
    $<$<CONFIG:Debug>:${FMOD_CORE_LIB_PATH}/fmodL_vc.lib>
    $<$<CONFIG:Release>:${FMOD_CORE_LIB_PATH}/fmod_vc.lib>
    $<$<CONFIG:Debug>:${FMOD_STUDIO_LIB_PATH}/fmodstudioL_vc.lib>
    $<$<CONFIG:Release>:${FMOD_STUDIO_LIB_PATH}/fmodstudio_vc.lib>
)
else()
message(STATUS "Linking static import libraries (.a)...")
target_link_libraries(${Project} PRIVATE
    $<$<CONFIG:Debug>:${FMOD_CORE_LIB_PATH}/libfmodL.a>
    $<$<CONFIG:Release>:${FMOD_CORE_LIB_PATH}/libfmod.a>
    $<$<CONFIG:Debug>:${FMOD_STUDIO_LIB_PATH}/libfmodstudioL.a>
    $<$<CONFIG:Release>:${FMOD_STUDIO_LIB_PATH}/libfmodstudio.a>
)
endif()

add_custom_command(TARGET ${Project} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${FMOD_CORE_LIB_PATH}/fmodL.dll,${FMOD_CORE_LIB_PATH}/fmod.dll>"
    $<TARGET_FILE_DIR:${Project}>
)

add_custom_command(TARGET ${Project} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<CONFIG:Debug>,${FMOD_STUDIO_LIB_PATH}/fmodstudioL.dll,${FMOD_STUDIO_LIB_PATH}/fmodstudio.dll>"
    $<TARGET_FILE_DIR:${Project}>
)


include(FetchContent)
### stb header files
message(STATUS "Looking for stb lib...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        f1c79c02822848a9bed4315b12c8c8f3761e1296 # master
)
FetchContent_MakeAvailable(stb)
target_include_directories(${Project} PRIVATE ${stb_SOURCE_DIR})
###

### glm header files
message(STATUS "Looking for glm lib...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        8d1fd52e5ab5590e2c81768ace50c72bae28f2ed # 1.0.3
)
FetchContent_MakeAvailable(glm)
target_include_directories(${Project} PRIVATE ${glm_SOURCE_DIR})
###

### nlohmann/json header files
message(STATUS "Looking for nlohmann/json lib...")
FetchContent_Declare(
    hlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        55f93686c01528224f448c19128836e7df245f72 # 3.12.0
)
FetchContent_MakeAvailable(hlohmann_json)
target_include_directories(${Project} PRIVATE ${hlohmann_json_SOURCE_DIR}/include)
###

### GLFW3 lib
message(STATUS "Looking for glfw3 package...")
find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()
target_link_libraries(${Project} PRIVATE glfw)
###

### Freetype lib
message(STATUS "Looking for Freetype package...")
find_package(Freetype QUIET)
if (Freetype_FOUND)
    target_link_libraries(${Project} PRIVATE Freetype::Freetype)
else()
    message(STATUS "Freetype package was not found, fetching from repo...")
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG        526ec5c47b9ebccc4754c85ac0c0cdf7c85a5e9b # 2.14.1
    )
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

# project packing
if (${CREATE_PACKAGE})
    if (WIN32)
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            message(STATUS "Project packing (compiled with Clang) for current system (WIN32 based) is started...")
            set(MSYS_BIN "C:/msys64/clang64/bin")
            set(DLL_LIST
                "${MSYS_BIN}/libc++.dll"
                "${MSYS_BIN}/libbrotlidec.dll"
                "${MSYS_BIN}/libbrotlicommon.dll"
            )
        endif()
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            message(STATUS "Project packing (compiled with GNU) for current system (WIN32 based) is started...")
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
        endif()


        foreach(dll ${DLL_LIST})
            add_custom_command(TARGET ${Project} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" "$<TARGET_FILE_DIR:${Project}>"
            )
        endforeach()

        install(TARGETS ${Project}
            RUNTIME DESTINATION . COMPONENT runtime
        )
        install(FILES ${DLL_LIST} DESTINATION . COMPONENT runtime)
        install(DIRECTORY ${RESOURCES_DIR} DESTINATION . COMPONENT runtime)

        set(CPACK_PACKAGE_NAME ${Project})
        set(CPACK_PACKAGE_VERSION "1.0.0")
        set(CPACK_PACKAGE_VENDOR "expool")
        set(CPACK_NSIS_COMPONENT_INSTALL ON)
        set(CPACK_COMPONENTS_ALL
            runtime
        )
        set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/packaging)
        set(CPACK_GENERATOR "NSIS")
        include(CPack)
        cpack_add_component(runtime
            DISPLAY_NAME ${Project}
            DESCRIPTION "Required files for the game."
            REQUIRED
        )

        message(STATUS "Project packing is completed. Use command to get packet: cpack --config build/CPackConfig.cmake")
    endif(WIN32)
    if (UNIX)
        message(STATUS "Project packing for current system (UNIX based) is started...")
        #
        message(STATUS "Project packing is completed. Use command to get packet: cpack --config build/CPackConfig.cmake")
    endif(UNIX)
endif()
