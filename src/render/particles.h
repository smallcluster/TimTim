//
// Created by pjaff on 26/03/2023.
//

#ifndef TIMTIM_PARTICLES_H
#define TIMTIM_PARTICLES_H

#include <utility>
#include <optional>
#include <random>
#include <chrono>
#include "../core/Resource.h"
#include "../core/GameObject.h"
#include "../render/sprite.h"



struct Particle {
    float life = 0;
    raylib::Vector2 position{0.f,0.f};
    raylib::Vector2 velocity{0.f, 0.f};
    float rotation{0};
    raylib::Color color{raylib::WHITE};
    float angularVelocity{0};
    bool active{false};
};

// Shapes are in local coordinate

// Default is a point
class EmissionShape {
public:
    raylib::Vector2 position;
    virtual raylib::Vector2 GetNextPoint(){
        return position;
    };

    static float GetRandom(){
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(0, 1); // rage 0 - 1
        return dis(e);
    }
};

class RectangleShape : public EmissionShape{
public:
    raylib::Vector2 size;
    raylib::Vector2 GetNextPoint() override {
        return {position.x-size.x/2.f+GetRandom()*size.x,position.y-size.y/2.f+GetRandom()*size.y};
    }
};

class DiscShape : public EmissionShape{
public:
    float r = 16;
    raylib::Vector2 GetNextPoint() override {
        raylib::Vector2 dir{-1+2*GetRandom(), -1+2*GetRandom()};
        float norm = GetRandom()*r;
        return dir.Normalize() * norm;
    }
};

class CircleShape : public EmissionShape{
public:
    float r = 16;
    raylib::Vector2 GetNextPoint() override {
        raylib::Vector2 dir{-1+2*GetRandom(), -1+2*GetRandom()};
        return dir.Normalize() * r;
    }
};

// System
class ParticlesSystem : public GameObject {

    void Update(float delta) override {
        // update timer
        timer += delta;
        timer = timer > lifeTime ? 0 : timer;

        // Deploy particle
        if(count > 0 && timer >= lifeTime / (float) count){
            for(auto& p : particles){
                if(p.active)
                    continue;

                // Deploy particle
                if(localCoords){
                    p.position = Vector2Zero();
                } else {
                    p.position = position;
                }

            }
        }

        // update particle
        for(auto& p : particles){
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
                //raylib::Rectangle src(0,0, initialScale, initialScale);
                //raylib::Rectangle dest(position.x + p.position.x, position.y + p.position.y, initialScale, initialScale);
                //texture.value()->Draw(src, dest, {initialScale/2.f, initialScale/2.f}, rotation, p.color);
            }else{
                //DrawRectanglePro({position.x + p.position.x, position.y + p.position.y, initialScale, initialScale}, {initialScale/2.f, initialScale/2.f}, p.rotation, p.color);
            }
        }
    }

private:
    std::vector<Particle> particles;

    // BASE
    bool emitting = true;
    int amount = 8;

    // TIME
    float lifeTime = 1.f;
    bool oneShot = false;
    float speedScale = 1.f;
    float Explosiveness = 0.f;
    float lifeTimeRandomness = 0.f;

    // DRAWING
    bool localCoords{false};
    std::optional<std::shared_ptr<raylib::Texture2D>> texture;

    // Shape
    EmissionShape emissionShape;

    // Particles flags
    bool alignY{false};

    // Emission direction
    raylib::Vector2 direction{1,0};
    float directionSpread{0}; // Define the direction cone (-spread to +spread) in degree

    // Gravity
    raylib::Vector2 gravity{0,980};

    // Velocity min-max
    raylib::Vector2 minInitialVelocity{0,0};
    raylib::Vector2 angularVelocity{0,0};
    std::optional<CurveParameter> angularVelocityCurve;
    //TODO: Add orbital velocity
    //TODO: Add acceleration

    // Damping
    raylib::Vector2 damping{0,0};
    std::optional<CurveParameter> dampingCurve;

    // Angle min-max
    raylib::Vector2 angle{0,0};
    std::optional<CurveParameter> angleCurve;

    // Scale min-max
    raylib::Vector2 scale{1,1};
    std::optional<CurveParameter> scaleCurve;

    // Color
    raylib::Color color;
    //TODO: add color ramp & initial color ramp

    float timer = 0.f;
    int count = 0;

};


#endif //TIMTIM_PARTICLES_H
