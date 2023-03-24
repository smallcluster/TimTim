//
// Created by pjaff on 24/03/2023.
//

#include "Game.h"
#include "raylib-cpp.hpp"

Game::Game(int screenWidth, int screenHeight, std::string title) : _screenWidth(screenWidth), _screenHeight(screenHeight){
    _window.Init(_screenWidth, _screenHeight, title);
    while(!_window.ShouldClose()){
        _window.BeginDrawing();
        MainLoop(_window.GetFrameTime());
        _window.EndDrawing();
    }
}