cmake_minimum_required(VERSION 3.14)
project(timtim)

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
        src/render/particles.h)

set(GAME_SRC
        src/GameMap.h
        src/Player.h
        src/main.cpp
        )

# MAIN GAME
add_executable(${PROJECT_NAME} ${ENGINE_SRC} ${GAME_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib raylib_cpp nlohmann_json::nlohmann_json raylib-tileson)


# Editor tools
set(PARTICLE_EDIT_NAME PEdit)
set(PARTICLE_EDITOR_SRC
        tools/particle_editor.cpp)
add_executable(${PARTICLE_EDIT_NAME} ${ENGINE_SRC} ${PARTICLE_EDITOR_SRC})
set_target_properties(${PARTICLE_EDIT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries(${PARTICLE_EDIT_NAME} PRIVATE raylib raylib_cpp nlohmann_json::nlohmann_json)
