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

#include "raylib-cpp.hpp"

//-------------------------------------------------------
//  RAYGUI
//-------------------------------------------------------
#define RAYGUI_IMPLEMENTATION // THIS FIRST
#include "external/raygui.h"
//-------------------------------------------------------

#include "Sprite.h"
#include <memory>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    raylib::Window window(screenWidth, screenHeight, "Test sprite animations");
    window.SetTargetFPS(60);

    std::shared_ptr<raylib::Texture2D> texture = std::make_shared<raylib::Texture2D>("./data/tux.png");

    AnimatedSprite player;
    player.position = raylib::Vector2(0,0);

    auto animations = LoadAnimationFrames("./data/tux.txt");

    for(auto& pair : animations){
        SpriteAnimation anim;
        anim.frames = pair.second;
        anim.spriteSheet = texture;

        anim.speed = 8;

        player.SetAnimation(pair.first, anim);
    }

    player.PlayAnimation("run");

    raylib::Camera2D cam;
    cam.offset = {screenWidth/2, screenHeight/2};
    cam.rotation = 0;
    cam.zoom = 4;


    // Main game loop
    while (!window.ShouldClose())
    {
        window.BeginDrawing();
        window.ClearBackground(WHITE);
        float y = 0;
        if(GuiButton({0, y,128,64}, "idle")){
            player.PlayAnimation("idle");
        }
        y += 64+8;
        if(GuiButton({0, y,128,64}, "walk")){
            player.PlayAnimation("walk");
        }
        y += 64+8;
        if(GuiButton({0, y,128,64}, "run")){
            player.PlayAnimation("run");
        }
            cam.BeginMode();

                player.Draw(window.GetFrameTime(), true);

            cam.EndMode();
        window.EndDrawing();
    }
    return 0;
}
