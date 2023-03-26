//
// Created by pjaff on 24/03/2023.
//

#include "sprite.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// CLASS: sprite
//----------------------------------------------------------------------------------------------------------------------
sprite::sprite(std::shared_ptr<raylib::Texture2D> texture) : texture(std::move(texture)) {}
void sprite::Draw(){
    raylib::Vector2 positiveScale(std::abs(scale.x), std::abs(scale.y));
    raylib::Vector2 offset(texture->width / 2.0f, texture->height / 2.0f);
    offset *= positiveScale;

    float srcWidth = scale.x < 0 ? -texture->width : texture->width;
    float srcHeight = scale.y < 0 ? -texture->height : texture->height;

    raylib::Rectangle src(0,0, srcWidth, srcHeight);
    raylib::Rectangle dest(position.x, position.y, texture->width * positiveScale.x, texture->height * positiveScale.y);
    texture->Draw(src, dest, offset, rotation);
}
int sprite::GetWidth(){return  texture->width;}
int sprite::GetHeight(){return  texture->height;}


//----------------------------------------------------------------------------------------------------------------------
// CLASS: AnimatedSprite
//----------------------------------------------------------------------------------------------------------------------
AnimatedSprite::AnimatedSprite(std::shared_ptr<raylib::Texture2D> texture, AnimationData animData) :
        sprite(std::move(texture)),
        frames(std::move(animData.frames)),
        playback(animData.playback),
        framerate(animData.framerate) {}

void AnimatedSprite::Update(float delta) {
    timer  += framerate * delta * (reverse ? -1.f : 1.f);

    int maxFrames = frames.size() - 1;
    switch (playback) {
        case ANIMATION_PLAYBACK::ONCE:
            timer = timer > maxFrames ? maxFrames : timer; // clamp timer
            break;
        case ANIMATION_PLAYBACK::LOOP:
            if(reverse)
                timer = timer < 0 ? maxFrames : timer;
            else
                timer = timer > maxFrames ? 0 : timer; // reset timer
            break;
        case ANIMATION_PLAYBACK::PING_PONG:
            if(timer < 0){
                timer = 0;
                reverse = !reverse;
            } else if(timer > maxFrames){
                timer = maxFrames;
                reverse = !reverse;
            }
            break;
    }
}

void AnimatedSprite::Draw() {
    int index = std::round(timer);
    raylib::Rectangle& rect = frames[index];
    raylib::Vector2 positiveScale(std::abs(scale.x), std::abs(scale.y));
    raylib::Vector2 offset(rect.width/2, rect.height/2);
    offset *= positiveScale;

    float srcWidth = scale.x < 0 ? -rect.width : rect.width;
    float srcHeight = scale.y < 0 ? -rect.height : rect.height;

    raylib::Rectangle src(rect.x,rect.y, srcWidth, srcHeight);
    raylib::Rectangle dest(position.x, position.y, rect.width*positiveScale.x, rect.width*positiveScale.y);
    texture->Draw(src, dest, offset, rotation);
}

void AnimatedSprite::SetFrames(std::vector<raylib::Rectangle> frames){
    frames = std::move(frames);
}

void AnimatedSprite::StartAt(float t){
    timer = Clamp((frames.size() - 1) * t, 0, (frames.size() - 1));
}
