//
// Created by donov on 22/03/2023.
//

#ifndef TIMTIM_PLAYER_H
#define TIMTIM_PLAYER_H
#include "raylib-cpp.hpp"

class Player {
private:
    float posX;
    float posY;
//    Sptite

public:
    Player() {

    }

    void UpdateMouvements(void){
        // Update Mouvements
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) {

        };
        if (IsKeyDown(KEY_LEFT)) {

        };
        if (IsKeyDown(KEY_UP)) {

        };
        if (IsKeyDown(KEY_DOWN)) {

        };
    }
};

#endif //TIMTIM_PLAYER_H
