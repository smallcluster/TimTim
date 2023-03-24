//
// Created by donov on 22/03/2023.
//

#ifndef TIMTIM_PLAYER_H
#define TIMTIM_PLAYER_H
#include "raylib-cpp.hpp"

class Player {
private:
    raylib::Vector2 playerPosition;

public:
    Player(float posX, float posY) {;
        this->playerPosition = raylib::Vector2(posX, posY);
    }

    void Draw() {
        DrawRectangleV(this->playerPosition, (Vector2){ 16, 16 }, RED);
    }

    const raylib::Vector2 &getPlayerPosition() const {
        return playerPosition;
    }

    void UpdatePosition(){
        // Update Position
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) playerPosition.x += 3;
        if (IsKeyDown(KEY_LEFT)) playerPosition.x -= 3;
        if (IsKeyDown(KEY_DOWN)) playerPosition.y += 3;
        if (IsKeyDown(KEY_UP)) playerPosition.y -= 3;
    }

    void DrawPosition() const {
        std::string posString = ("X: " + std::to_string(this->playerPosition.x) + " Y: " + std::to_string(this->playerPosition.y));
        raylib::DrawText(posString, 150, 10,20, raylib::YELLOW);
    }
};

#endif //TIMTIM_PLAYER_H
