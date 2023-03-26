
#include "core/Scene.h"
#include "Player.h"
#include "GameMap.h"

//----------------------------------------------------------------------------------------------------------------------
// TEST SCENE
//----------------------------------------------------------------------------------------------------------------------

class TestScene : public Scene {
public:

    TestScene() {
        // Player setup
        player = std::make_shared<Player>(0, 0);
        // Map setup
        auto map = std::make_shared<GameMap>("./data/maps/test.json");

        // Camera setup
        cam.offset = {0};
        cam.target = {0};
        cam.zoom = 2.0f;
        cam.rotation = 0;

        // -- register gameobjects --
        // Update & drawing from back to front
        RegisterGameObject(map);
        RegisterGameObject(player);

    }

    void Update(float delta) override {
        cam.target = player->position;
        Scene::Update(delta);
    }

    void Draw() override{
        ClearBackground(WHITE);
        // Draw Scene from camera
        cam.BeginMode();
            Scene::Draw();
        cam.EndMode();
    }

    void SetCameratoScreen(int screenWidth, int screenHeight){
        cam.offset = {screenWidth / 2.f, screenHeight / 2.f};
    }

private:
    std::shared_ptr<Player> player;
    raylib::Camera2D cam;
};


//----------------------------------------------------------------------------------------------------------------------
// START GAME
//----------------------------------------------------------------------------------------------------------------------
int main(){
    const int screenWidth = 800;
    const int screenHeight = 450;
    raylib::Window window(screenWidth, screenHeight, "TimTim");
    TestScene scene; // Default scene
    scene.SetCameratoScreen(screenWidth, screenHeight);
    while(!window.ShouldClose()){
        window.BeginDrawing();
        scene.Update(window.GetFrameTime());
        scene.Draw();
        window.EndDrawing();
    }
    return 0;
}