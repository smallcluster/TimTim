//
// Created by donov on 22/03/2023.
//

#ifndef TIMTIM_GAMEMAP_H
#define TIMTIM_GAMEMAP_H

#include "raylib-cpp.hpp"
#include "core/GameObject.h"

const int MAP_TILE_SIZE = 32;

class GameMap : public GameObject {
private:
    // Map data type
    int tilesX;            // Number of tiles in X axis
    int tilesY;            // Number of tiles in Y axis

public:
    GameMap(int tilesX, int tilesY) {
        this->tilesX = tilesX;
        this->tilesY = tilesY;
    }

    int getTilesX() const {
        return tilesX;
    }

    int getTilesY() const {
        return tilesY;
    }

    void Draw() override {
        for (int y = 0; y < this->tilesY; y++)
        {
            for (int x = 0; x < this->tilesX; x++)
            {
                // Draw tiles from id (and tile borders)
                DrawRectangle(x*MAP_TILE_SIZE, y*MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE, Fade(BLUE, 0.9f));
                DrawRectangleLines(x*MAP_TILE_SIZE, y*MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE, Fade(DARKBLUE, 0.5f));
            }
        }
    }
};


#endif //TIMTIM_GAMEMAP_H
