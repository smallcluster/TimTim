//
// Created by donov on 22/03/2023.
//
#include "raylib-cpp.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#ifndef TIMTIM_GAMEMAP_H
#define TIMTIM_GAMEMAP_H

const int MAP_TILE_SIZE = 50;
const int MAP_LINE_SIZE = 1.f;

class GameMap {
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

    void setTilesX(int tilesX) {
        GameMap::tilesX = tilesX;
    }

    int getTilesY() const {
        return tilesY;
    }

    void setTilesY(int tilesY) {
        GameMap::tilesY = tilesY;
    }

    void Draw() {
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
