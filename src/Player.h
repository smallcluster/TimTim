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
        animations = Resource::LoadAnimationsData("./data/tux.json");

        // Set default animation
        animationName = "idle";

        // Add animated sprite
        animatedSprite = std::make_shared<AnimatedSprite>(texture, animations[animationName]);
        AddChild(animatedSprite);
    }
    Player(float posX, float posY) : Player() {
        transform.position = raylib::Vector2(posX, posY);
    }
    explicit Player(raylib::Vector2 pos) : Player() {
        transform.position = pos;
    }

    void Update(float delta) override {
        UpdatePosition(delta);

        GameObject::Update(delta);
    }
    void Draw(const Transform2D& parentGlobalTransform) override {
        const Transform2D gTransform = parentGlobalTransform * transform;

        //TODO: REMOVE THIS DEBUG (Draw debug position)
        std::string posString = ("X: " + std::to_string(gTransform.position.x) + " Y: " + std::to_string(gTransform.position.y));
        raylib::DrawText(posString, gTransform.position.x, gTransform.position.y + 30,12, raylib::RED);

        GameObject::Draw(parentGlobalTransform);

        transform.DrawDebug(parentGlobalTransform);
    }


private:
    std::string animationName;
    std::map<std::string, AnimationData> animations;
    std::shared_ptr<AnimatedSprite> animatedSprite;

    //std::map<std::string, std::shared_ptr<AnimatedSprite>> animations;

    void SwitchAnimation(const std::string& name){
        // Restart animation only if there is a real change
        if(animationName != name){
            animationName = name;
            animatedSprite->SetAnimData(animations[name]);
            animatedSprite->StartAt(0.f); // playback at 0% <-> frame 0 <-> reset animation
        }
    }

    void UpdatePosition(float delta){
        raylib::Vector2 dir{0,0};
        if (IsKeyDown(KEY_RIGHT)) dir.x += 1;
        if (IsKeyDown(KEY_LEFT)) dir.x -= 1;
        if (IsKeyDown(KEY_DOWN)) dir.y += 1;
        if (IsKeyDown(KEY_UP)) dir.y -= 1;
        transform.position += dir* 64 * delta;

        float len = dir.LengthSqr();

        // Flip sprite
        if(dir.x != 0)
            animatedSprite->transform.scale.x = dir.x;

        if(len > 1)
            SwitchAnimation("run"); // Diagonals
        else if(len == 1)
            SwitchAnimation("walk");
        else
            SwitchAnimation("idle");
    }
};

#endif //TIMTIM_PLAYER_H
