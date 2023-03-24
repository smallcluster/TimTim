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
// Supports positive & negative (flip) scaling
// TODO: handle rotations
class sprite : public GameObject {
public:
    sprite(std::shared_ptr<raylib::Texture2D> texture);
    void Draw() override;

    int GetWidth();
    int GetHeight();

protected:
    std::shared_ptr<raylib::Texture2D> _texture;
};

// Object to display an animation centered on its position
// Same support as Sprite
// Supports framerate manipulation, playback modes and reversing its animation
class AnimatedSprite : public sprite {
public:
    bool reverse = false;
    float framerate = 0;
    ANIMATION_PLAYBACK playback = ANIMATION_PLAYBACK::LOOP;

    AnimatedSprite();

    AnimatedSprite(std::shared_ptr<raylib::Texture2D> texture, AnimationData animData);
    void Update(float delta) override;
    void Draw() override;
    void StartAt(float t);

    void SetFrames(std::vector<raylib::Rectangle> frames);
    bool Finished();

protected:
    float _timer = 0;
    std::vector<raylib::Rectangle> _frames;
};


#endif //TIMTIM_SPRITE_H
