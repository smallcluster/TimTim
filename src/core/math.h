//
// Created by pjaff on 29/04/2023.
//

#ifndef TIMTIM_MATH_H
#define TIMTIM_MATH_H

#include "raylib-cpp.hpp"

struct Transform2D{
    raylib::Vector2 position{0,0};
    raylib::Vector2 scale{1,1};
    float rotation{0}; // in deg
    static Transform2D Identity();
    Transform2D operator*(const Transform2D& other) const;
    void DrawDebug(const Transform2D& parentGlobalTransform);
};


#endif //TIMTIM_MATH_H
