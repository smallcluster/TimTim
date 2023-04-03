cmake_minimum_required(VERSION 3.24)
project(timtim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.2.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v4.2.6
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

# tmx (Tiled parser for raylib)
find_package(raylib-tileson QUIET)
if (NOT raylib-tileson_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib-tileson
            GIT_REPOSITORY https://github.com/RobLoach/raylib-tileson.git
            GIT_TAG v4.2.0
    )
    FetchContent_MakeAvailable(raylib-tileson)
endif()

# General Json parser
find_package(nlohmann_json 3.2.0 QUIET)
if (NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG 3.2.0
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# FMT
find_package(fmt QUIET)
if (NOT fmt_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 9.1.0
    )
    FetchContent_MakeAvailable(fmt)
endif()


# This is the main part:
set(ENGINE_SRC
        # LIBS
        src/external/raygui/raygui.h
        src/external/raygui/raygui.cpp


        # CORE
        src/core/Scene.cpp
        src/core/Scene.h
        src/core/Resource.cpp
        src/core/Resource.h
        src/core/GameObject.h

        # RENDERING
        src/render/sprite.cpp
        src/render/sprite.h
        src/render/particles.cpp
        src/render/particles.h

        # GUI Widget
        src/gui/ContextMenu.cpp
        src/gui/ContextMenu.h
        src/gui/CurveEditor.cpp
        src/gui/CurveEditor.h
        src/gui/ColorRampEditor.cpp
        src/gui/ColorRampEditor.h
        src/gui/ColorPopup.h

        src/gui/gui.h)

set(GAME_SRC
        src/GameMap.h
        src/Player.h
        src/main.cpp)

# MAIN GAME
add_executable(${PROJECT_NAME} ${ENGINE_SRC} ${GAME_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib raylib_cpp nlohmann_json::nlohmann_json raylib-tileson fmt::fmt)


# Editor tools
set(PARTICLE_EDIT_NAME PEdit)
set(PARTICLE_EDITOR_SRC tools/particle_editor.cpp)
add_executable(${PARTICLE_EDIT_NAME} ${ENGINE_SRC} ${PARTICLE_EDITOR_SRC})
target_link_libraries(${PARTICLE_EDIT_NAME} PRIVATE raylib raylib_cpp nlohmann_json::nlohmann_json fmt::fmt)
