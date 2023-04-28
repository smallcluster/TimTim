//
// Created by pjaff on 22/03/2023.
//

#ifndef TIMTIM_SPRITE_H
#define TIMTIM_SPRITE_H

#include "raylib-cpp.hpp"
#include "../core/GameObject.h"
#include "../core/Resource.h"
#include <vector>
#include <memory>

// Object to display a centered texture
// Supports positive/negative (flip) scaling & rotations
class Sprite : public GameObject {
public:
    explicit Sprite(std::shared_ptr<raylib::Texture2D> texture);
    void Draw(const Transform2D& parentGlobalTransform) override;

    void SetTexture(std::shared_ptr<raylib::Texture2D> texture);

    int GetWidth();
    int GetHeight();

protected:
    std::shared_ptr<raylib::Texture2D> texture;
};

// Object to display an animation centered on its position
// Same support as Sprite
// Supports framerate manipulation, playback modes and reversing its animation
class AnimatedSprite : public Sprite {
public:
    bool reverse = false;
    float framerate = 0;
    ANIMATION_PLAYBACK playback = ANIMATION_PLAYBACK::LOOP;

    AnimatedSprite(std::shared_ptr<raylib::Texture2D> texture, AnimationData animData);
    void Update(float delta) override;
    void Draw(const Transform2D& parentGlobalTransform) override;
    void StartAt(float t);

    void SetAnimData(const AnimationData& animData);

    void SetFrames(std::vector<raylib::Rectangle> frames);
    bool Finished();

protected:
    float timer = 0;
    std::vector<raylib::Rectangle> frames;
};


#endif //TIMTIM_SPRITE_H
