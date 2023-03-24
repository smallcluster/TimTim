//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_GAMEOBJECT_H
#define TIMTIM_GAMEOBJECT_H

#include "raylib-cpp.hpp"

// Base class for all game object in a scene
class GameObject {
public:
    raylib::Vector2 position{0,0};
    raylib::Vector2 scale{1,1};
    float rotation{0};

    virtual void Update(float delta){}
    virtual void Draw(){}
};


#endif //TIMTIM_GAMEOBJECT_H
