//
// Created by donov on 22/03/2023.
//

#ifndef TIMTIM_GAMEMAP_H
#define TIMTIM_GAMEMAP_H

#include "raylib-cpp.hpp"
#include "core/GameObject.h"
#include "raylib-tileson.h"

const int MAP_TILE_SIZE = 32;

class GameMap : public GameObject {
private:
    // Map data type
    int width;            // Number of tiles in X axis
    int height;            // Number of tiles in Y axis
    Map map;

public:
    GameMap(const char * mapPath) {
        map = LoadTiled(mapPath);
        width = map.width;
        height = map.height;
    }

    int GetWidth() const {
        return width;
    }

    int GetHeight() const {
        return height;
    }

    void Draw() override {
        DrawTiled(map, 0, 0, WHITE);
    }
};


#endif //TIMTIM_GAMEMAP_H
