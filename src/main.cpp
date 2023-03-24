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
#include "external/raygui/raygui.h"
#include "GameMap.h"
#include "Player.h"
#include "Sprite.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const int screenWidth = 800;
const int screenHeight = 450;

raylib::Window window;

GameMap map = GameMap(20,20);
Player player = Player(0,0);
AnimatedSprite test;
raylib::Camera2D cam;

json testJson = R"(
{
    "pi": 3.141,
    "happy": true
}
)"_json;


//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame
//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    window.Init(screenWidth, screenHeight, "TimTim");
    // After windows init !!!!
    std::shared_ptr<raylib::Texture2D> texture = std::make_shared<raylib::Texture2D>("./data/tux.png");
    test.position = raylib::Vector2(screenWidth/2,screenHeight/2);
    auto animations = LoadAnimationFrames("./data/tux.txt");
    for(auto& pair : animations){
        SpriteAnimation anim;
        anim.frames = pair.second;
        anim.spriteSheet = texture;
        anim.speed = 8;
        test.SetAnimation(pair.first, anim);
    }
    test.PlayAnimation("run");
    cam.offset = {0, 0};
    cam.rotation = 0;
    cam.zoom = 1.0f;

    //--------------------------------------------------------------------------------------
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    window.SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose())    // Detect window close button or ESC key
    {
        window.ClearBackground(RAYWHITE);
        UpdateDrawFrame();
        window.DrawFPS(10, 10);
    }
#endif

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{

    // Draw
    //----------------------------------------------------------------------------------
    window.BeginDrawing();
        // DRAW WORLD
        cam.BeginMode();
            map.Draw();
            player.Draw();
            player.UpdatePosition();
            player.DrawPosition();

            test.Draw(GetFrameTime(), true);
        cam.EndMode();

        // DRAW GUI
        float y = 0;
        if(GuiButton({0, y,128,64}, "idle")){
            test.PlayAnimation("idle");
        }
        y += 64+8;
        if(GuiButton({0, y,128,64}, "walk")){
            test.PlayAnimation("walk");
        }
        y += 64+8;
        if(GuiButton({0, y,128,64}, "run")){
            test.PlayAnimation("run");
        }
    window.EndDrawing();
    //----------------------------------------------------------------------------------
}
