//
// Created by pjaff on 22/03/2023.
//

#ifndef TIMTIM_SPRITE_H
#define TIMTIM_SPRITE_H

#include "raylib-cpp.hpp"
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <fstream>
#include <string>



typedef struct SpriteAnimation {
    std::shared_ptr<raylib::Texture2D> spriteSheet;
    int speed;
    std::vector<raylib::Rectangle> frames;
};

std::map<std::string, std::vector<raylib::Rectangle>> LoadAnimationFrames(const std::string& path){
    std::map<std::string, std::vector<raylib::Rectangle>> map;

    std::ifstream file(path);
    std::string delimiter = ",";
    char const* digits = "0123456789";

    while (file){
        // Read line
        std::string line;
        std::getline(file, line);

        if(line.empty())
            break;

        // Extract animation name
        std::string frameName = line.substr(0, line.find(delimiter));
        std::string animationName = frameName.substr(0, frameName.find_first_of(digits));
        // Extract frame rect
        line.erase(0, line.find(delimiter) + delimiter.length());
        int x = std::stoi(line.substr(0, line.find(delimiter)));
        line.erase(0, line.find(delimiter) + delimiter.length());
        int y = std::stoi(line.substr(0, line.find(delimiter)));
        line.erase(0, line.find(delimiter) + delimiter.length());
        int w = std::stoi(line.substr(0, line.find(delimiter)));
        line.erase(0, line.find(delimiter) + delimiter.length());
        int h = std::stoi(line);
        raylib::Rectangle rect(x,y,w,h);

        // Save data
        map[animationName].push_back(rect);
    }
    file.close();
    return map;
}

class AnimatedSprite {
public:
    raylib::Vector2 position;
    bool loop = true;

    void Draw(float delta, bool debug = false){
        if(!_currentAnimation.has_value())
            return;

        int frame = int(_timer);
        // Draw frame
        raylib::Rectangle& rect = _currentAnimation.value()->frames[frame];

        raylib::Vector2 offset(rect.width/2, rect.height/2);
        _currentAnimation.value()->spriteSheet->Draw(rect, position-offset);

        if(debug){
            DrawRectangleLines(position.x-offset.x, position.y-offset.y, rect.width, rect.height, RED);
        }

        // Update timer
        _timer += (float) _currentAnimation.value()->speed * delta;
        int maxFrame = _currentAnimation.value()->frames.size()-1;
        if(_timer > maxFrame) _timer = loop ? 0 : maxFrame;
    }

    void PlayAnimation(const std::string& name){
        _currentAnimation = {&(_animations[name])};
        _timer = 0; // Reset timer
    }

    void SetAnimation(const std::string& name, SpriteAnimation animation){
        _animations[name] = std::move(animation);
    }

private:
    float _timer;
    std::optional<SpriteAnimation*> _currentAnimation;
    std::map<std::string, SpriteAnimation> _animations;
};


#endif //TIMTIM_SPRITE_H
