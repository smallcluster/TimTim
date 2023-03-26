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
    int _width;            // Number of tiles in X axis
    int _height;            // Number of tiles in Y axis
    Map _map;

public:
    GameMap(const char * mapPath) {
        _map = LoadTiled(mapPath);
        _width = _map.width;
        _height = _map.height;
    }

    int getWidth() const {
        return _width;
    }

    int getHeight() const {
        return _height;
    }

    void Draw() override {
        DrawTiled(_map, 0, 0, WHITE);
    }
};


#endif //TIMTIM_GAMEMAP_H
