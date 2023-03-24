//
// Created by donov on 24/03/2023.
//
#include "raylib-cpp.hpp"

#ifndef TIMTIM_CAMERA_H
#define TIMTIM_CAMERA_H

class CameraScene : public raylib::Camera2D{
private:
    raylib::Vector2 positions;


public:
    CameraScene(raylib::Vector2 positions) {
        this->positions = positions;
    }

    void InitConfig() {
        this->rotation = 0;
        this->zoom = 1.0f;
        this->target = this->positions;
    }

    void UpdatePositionByPlayer(raylib::Vector2 playerPositions) {

        this->target =  playerPositions;
    }
};

#endif //TIMTIM_CAMERA_H
