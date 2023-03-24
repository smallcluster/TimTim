//
// Created by donov on 22/03/2023.
//

#ifndef TIMTIM_PLAYER_H
#define TIMTIM_PLAYER_H

#include "raylib-cpp.hpp"
#include "core/GameObject.h"
#include "render/sprite.h"
#include "core/Resource.h"

#include <map>
#include <memory>

class Player : public GameObject{

public:
    Player(){
        // Load animations
        std::shared_ptr<raylib::Texture2D> texture = std::make_shared<raylib::Texture2D>("./data/tux.png");
        auto anims = Resource::LoadAnimationsData("./data/tux.json");
        for(auto& pair : anims){
            auto animatedSprite = std::make_shared<AnimatedSprite>(texture, pair.second);
            _animations[pair.first] = animatedSprite;
        }
        // Set default animation
        _animation_name = "idle";
    }
    Player(float posX, float posY) : Player() {
        position = raylib::Vector2(posX, posY);
    }
    Player(raylib::Vector2 pos) : Player() {
        position = pos;
    }

    void Update(float delta) override {
        _UpdatePosition(delta);
        // Update sprite position and advance its animation
        _animations[_animation_name]->position = position;
        _animations[_animation_name]->Update(delta);
    }
    void Draw() override {
        _animations[_animation_name]->Draw(); // Draw sprite
        _DrawPosition();
    }


private:
    std::string _animation_name;
    std::map<std::string, std::shared_ptr<AnimatedSprite>> _animations;

    void _SwitchAnimation(const std::string& name){
        auto next_animation = std::make_pair(name, _animations[name]);
        // Restart animation only if there is a real change
        if(_animation_name != name){
            _animation_name = name;
            _animations[_animation_name]->StartAt(0.f); // playback at 0% <-> frame 0 <-> reset animation
        }
    }

    void _UpdatePosition(float delta){
        raylib::Vector2 dir{0,0};
        if (IsKeyDown(KEY_RIGHT)) dir.x += 1;
        if (IsKeyDown(KEY_LEFT)) dir.x -= 1;
        if (IsKeyDown(KEY_DOWN)) dir.y += 1;
        if (IsKeyDown(KEY_UP)) dir.y -= 1;
        position += dir* 64 * delta;

        float len = dir.LengthSqr();

        // Flip sprite
        if(dir.x != 0)
            _animations[_animation_name]->scale.x = dir.x;

        if(len > 1)
            _SwitchAnimation("run"); // Diagonals
        else if(len == 1)
            _SwitchAnimation("walk");
        else
            _SwitchAnimation("idle");
    }

    void _DrawPosition() const {
        std::string posString = ("X: " + std::to_string(position.x) + " Y: " + std::to_string(position.y));
        raylib::DrawText(posString, position.x, position.y + 30,12, raylib::RED);
    }
};

#endif //TIMTIM_PLAYER_H
