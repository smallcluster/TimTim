//
// Created by pjaff on 24/03/2023.
//

#include "sprite.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// CLASS: sprite
//----------------------------------------------------------------------------------------------------------------------
sprite::sprite(std::shared_ptr<raylib::Texture2D> texture) : _texture(std::move(texture)) {}
void sprite::Draw(){
    raylib::Vector2 positiveScale(std::abs(scale.x), std::abs(scale.y));
    raylib::Vector2 offset(_texture->width/2.0f, _texture->height/2.0f);
    offset *= positiveScale;

    float srcWidth = scale.x < 0 ? -_texture->width : _texture->width;
    float srcHeight = scale.y < 0 ? -_texture->height : _texture->height;

    raylib::Rectangle src(0,0, srcWidth, srcHeight);
    raylib::Rectangle dest(position.x, position.y, _texture->width*positiveScale.x, _texture->height*positiveScale.y);
    _texture->Draw(src, dest, offset);
}
int sprite::GetWidth(){return  _texture->width;}
int sprite::GetHeight(){return  _texture->height;}


//----------------------------------------------------------------------------------------------------------------------
// CLASS: AnimatedSprite
//----------------------------------------------------------------------------------------------------------------------
AnimatedSprite::AnimatedSprite(std::shared_ptr<raylib::Texture2D> texture, AnimationData animData) :
sprite(std::move(texture)),
_frames(std::move(animData.frames)),
playback(animData.playback),
framerate(animData.framerate) {}

void AnimatedSprite::Update(float delta) {
    _timer  += framerate * delta * (reverse ? -1.f : 1.f);

    int maxFrames = _frames.size()-1;
    switch (playback) {
        case ANIMATION_PLAYBACK::ONCE:
            _timer = _timer > maxFrames ? maxFrames : _timer; // clamp timer
            break;
        case ANIMATION_PLAYBACK::LOOP:
            if(reverse)
                _timer = _timer < 0 ? maxFrames : _timer;
            else
                _timer = _timer > maxFrames ? 0 : _timer; // reset timer
            break;
        case ANIMATION_PLAYBACK::PING_PONG:
            if(_timer < 0){
                _timer = 0;
                reverse = !reverse;
            } else if(_timer > maxFrames){
                _timer = maxFrames;
                reverse = !reverse;
            }
            break;
    }
}

void AnimatedSprite::Draw() {
    int index = std::round(_timer);
    raylib::Rectangle& rect = _frames[index];
    raylib::Vector2 positiveScale(std::abs(scale.x), std::abs(scale.y));
    raylib::Vector2 offset(rect.width/2, rect.height/2);
    offset *= positiveScale;

    float srcWidth = scale.x < 0 ? -rect.width : rect.width;
    float srcHeight = scale.y < 0 ? -rect.height : rect.height;

    raylib::Rectangle src(rect.x,rect.y, srcWidth, srcHeight);
    raylib::Rectangle dest(position.x, position.y, rect.width*positiveScale.x, rect.width*positiveScale.y);
    _texture->Draw(src, dest, offset);
}

void AnimatedSprite::SetFrames(std::vector<raylib::Rectangle> frames){
    _frames = std::move(frames);
}

void AnimatedSprite::StartAt(float t){
    _timer = Clamp((_frames.size()-1) * t, 0, (_frames.size()-1));
}
