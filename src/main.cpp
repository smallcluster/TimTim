/*******************************************************************************************
*
*   raylib-cpp [core] example - Basic window (adapted for HTML5 platform)
*
*   This example is prepared to compile for PLATFORM_WEB, PLATFORM_DESKTOP and PLATFORM_RPI
*   As you will notice, code structure is slightly diferent to the other examples...
*   To compile it for PLATFORM_WEB just uncomment #define PLATFORM_WEB at beginning
*
*   This example has been created using raylib-cpp (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib-cpp.hpp"    // Raylib
#include "external/raygui/raygui.h" // GUI lib for debug


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    raylib::Window window(screenWidth, screenHeight, "Test sprite animations");
    window.SetTargetFPS(60);

    raylib::Camera2D cam;
    cam.offset = {screenWidth/2, screenHeight/2};
    cam.rotation = 0;
    cam.zoom = 4;


    // Main game loop
    while (!window.ShouldClose())
    {
        window.BeginDrawing();
        window.ClearBackground(WHITE);

            // Cam view
            cam.BeginMode();


            cam.EndMode();
        window.EndDrawing();
    }
    return 0;
}
