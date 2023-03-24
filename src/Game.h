//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_GAME_H
#define TIMTIM_GAME_H

#include "raylib-cpp.hpp"

class Game {
public:
    Game(int screenWidth, int screenHeight, std::string title);
    virtual void MainLoop(float delta);

private:
    raylib::Window _window;
    int _screenWidth;
    int _screenHeight;
};

#endif //TIMTIM_GAME_H
