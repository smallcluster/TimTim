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
#include <fstream>
#include <string>

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

class Sprite {
public:
    raylib::Vector2 position;
    explicit Sprite(std::shared_ptr<raylib::Texture2D> texture) : _texture(std::move(texture)){}
    virtual void Draw(float delta){
        raylib::Vector2 offset(-_texture->width/2.0f, -_texture->height/2.0f);
        _texture->Draw(position+offset);
    }
    int GetWidth(){return  _texture->width;}
    int GetHeight(){return  _texture->height;}

protected:
    std::shared_ptr<raylib::Texture2D> _texture;
};

class AnimatedSprite : public Sprite {
public:
    bool loop = true;
    int frameRate = 0;

    AnimatedSprite(std::shared_ptr<raylib::Texture2D> texture, std::vector<raylib::Rectangle> frames) : Sprite(std::move(texture)), _frames(std::move(frames)) {}

    void Draw(float delta) override {
        int index = int(_timer);
        raylib::Rectangle& rect = _frames[index];
        raylib::Vector2 offset(rect.width/2, rect.height/2);
        _texture->Draw(rect, position-offset);
        // Update timer
        _timer += (float) frameRate * delta;
        int maxFrame = _frames.size()-1;
        if(_timer > maxFrame) _timer = loop ? 0 : maxFrame;
    }

    void SetFrames(std::vector<raylib::Rectangle> frames){
        _frames = std::move(frames);
    }

private:
    float _timer = 0;
    std::vector<raylib::Rectangle> _frames;
};


#endif //TIMTIM_SPRITE_H
