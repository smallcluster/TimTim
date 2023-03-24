
#include "core/Scene.h"
#include "Player.h"
#include "core/Resource.h"
#include "GameMap.h"

//----------------------------------------------------------------------------------------------------------------------
// TEST SCENE
//----------------------------------------------------------------------------------------------------------------------

class TestScene : public Scene {
public:

    TestScene() {
        // Player setup
        _player = new Player(0,0);
        // Map setup
        auto* _map = new GameMap(50, 50);

        // Camera setup
        _cam.offset = {0};
        _cam.target = {0};
        _cam.zoom = 2.0f;
        _cam.rotation = 0;

        // -- register gameobjects --
        // Update & drawing from back to front
        RegisterGameObject(_map);
        RegisterGameObject(_player);

    }

    void Update(float delta) override {
        _cam.target = _player->position;
        Scene::Update(delta);
    }

    void Draw() override{
        ClearBackground(WHITE);
        // Draw Scene from camera
        _cam.BeginMode();
            Scene::Draw();
        _cam.EndMode();
    }

    void SetCameratoScreen(int screenWidth, int screenHeight){
        _cam.offset = {screenWidth/2.f,screenHeight/2.f};
    }

private:
    Player* _player;
    raylib::Camera2D _cam;
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