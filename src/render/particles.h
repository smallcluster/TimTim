//
// Created by pjaff on 26/03/2023.
//

#ifndef TIMTIM_PARTICLES_H
#define TIMTIM_PARTICLES_H

#include <utility>
#include <optional>

#include "../core/GameObject.h"
#include "../render/sprite.h"

struct Particle {
    float life = 0;
    raylib::Vector2 position{0.f,0.f};
    float rotation{0};
    raylib::Color color{raylib::WHITE};
    bool active{false};
};

class ParticlesSystem : public GameObject {

    void Update(float delta) override {
        // update living particle
        for(auto& p : particles){
            if(!p.active)
                continue;
            p.life -= delta;
            p.active = p.life > 0;
        }

    }
    void Draw() override {
        // draw living particles
        for(auto& p : particles){
            if(!p.active)
                continue;
            if(texture.has_value()){
                raylib::Rectangle src(0,0, initialScale, initialScale);
                raylib::Rectangle dest(position.x + p.position.x, position.y + p.position.y, initialScale, initialScale);
                texture.value()->Draw(src, dest, {initialScale/2.f, initialScale/2.f}, rotation, p.color);
            }else{
                DrawRectanglePro({position.x + p.position.x, position.y + p.position.y, initialScale, initialScale}, {initialScale/2.f, initialScale/2.f}, p.rotation, p.color);
            }
        }
    }

private:
    std::optional<std::shared_ptr<raylib::Texture2D>> texture;
    std::vector<Particle> particles;

    float initialScale = 16.f;
    float lifeTime = 1.f;

};


#endif //TIMTIM_PARTICLES_H
