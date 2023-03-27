//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_RESOURCE_H
#define TIMTIM_RESOURCE_H

#include "raylib-cpp.hpp"
#include <vector>
#include <map>
#include <string>


enum class ANIMATION_PLAYBACK {
    ONCE, // Animation stop at last frame
    LOOP, // Animation auto restart on end
    PING_PONG // Animation goes back and forth (loop)
};

struct AnimationData {
    float framerate = 0;
    ANIMATION_PLAYBACK playback = ANIMATION_PLAYBACK::ONCE;
    std::vector<raylib::Rectangle> frames;
};



struct CurveParameterPoint {
    raylib::Vector2 position{0,0};
    float leftAngle{0};
    float rightAngle{0};
    bool leftLinear{false};
    bool rightLinear{false};
};

// A curve to interpolate a float value form 0 to 1
class CurveParameter {
public:
    CurveParameter() = default;
    CurveParameter(std::vector<CurveParameterPoint> data);
    float Eval(float t);
    void SetData(std::vector<CurveParameterPoint> data);
private:
    // points are sorted along x axis
    std::vector<CurveParameterPoint> points;
};


//TODO: usefulness as a class ?
class Resource {
public:
    // Load all animations described in a json file (see tux.json)
    static std::map<std::string, AnimationData> LoadAnimationsData(const std::string& path);
    // Helper to convert back the string representation of an ANIMATION_PLAYBACK value
    static ANIMATION_PLAYBACK ParsePlayback(const std::string& str);
    static float Lerp(float t, float start, float end);
    static raylib::Vector2 Lerp(float t, raylib::Vector2 start, raylib::Vector2 end);
    static raylib::Vector2 Bezier(float t, raylib::Vector2 start, raylib::Vector2 control1, raylib::Vector2 control2, raylib::Vector2 end);
private:
    Resource()= default;
};


#endif //TIMTIM_RESOURCE_H
