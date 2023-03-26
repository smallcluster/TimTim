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
            animations[pair.first] = animatedSprite;
        }
        // Set default animation
        animationName = "idle";
    }
    Player(float posX, float posY) : Player() {
        position = raylib::Vector2(posX, posY);
    }
    explicit Player(raylib::Vector2 pos) : Player() {
        position = pos;
    }

    void Update(float delta) override {
        UpdatePosition(delta);
        // Update sprite position and advance its animation
        animations[animationName]->position = position;
        animations[animationName]->Update(delta);
    }
    void Draw() override {
        animations[animationName]->Draw(); // Draw sprite
        DrawPosition();
    }


private:
    std::string animationName;
    std::map<std::string, std::shared_ptr<AnimatedSprite>> animations;

    void SwitchAnimation(const std::string& name){
        auto next_animation = std::make_pair(name, animations[name]);
        // Restart animation only if there is a real change
        if(animationName != name){
            animationName = name;
            animations[animationName]->StartAt(0.f); // playback at 0% <-> frame 0 <-> reset animation
        }
    }

    void UpdatePosition(float delta){
        raylib::Vector2 dir{0,0};
        if (IsKeyDown(KEY_RIGHT)) dir.x += 1;
        if (IsKeyDown(KEY_LEFT)) dir.x -= 1;
        if (IsKeyDown(KEY_DOWN)) dir.y += 1;
        if (IsKeyDown(KEY_UP)) dir.y -= 1;
        position += dir* 64 * delta;

        float len = dir.LengthSqr();

        // Flip sprite
        if(dir.x != 0)
            animations[animationName]->scale.x = dir.x;

        if(len > 1)
            SwitchAnimation("run"); // Diagonals
        else if(len == 1)
            SwitchAnimation("walk");
        else
            SwitchAnimation("idle");
    }

    void DrawPosition() const {
        std::string posString = ("X: " + std::to_string(position.x) + " Y: " + std::to_string(position.y));
        raylib::DrawText(posString, position.x, position.y + 30,12, raylib::RED);
    }
};

#endif //TIMTIM_PLAYER_H
