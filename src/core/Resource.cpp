//
// Created by pjaff on 24/03/2023.
//

#include "Resource.h"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

std::map<std::string, AnimationData>  Resource::LoadAnimationsData(const std::string& path){
    std::map<std::string, AnimationData> map;
    std::ifstream file(path);

    json animations = json::parse(file);

    for(auto& anim : animations["animations"]){
        std::string name = anim["name"];

        AnimationData animData;
        animData.playback = Resource::ParsePlayback(anim["playback"]);
        animData.framerate = anim["framerate"];

        for(auto& frame : anim["frames"]){
            int x = frame["x"];
            int y = frame["y"];
            int w = frame["width"];
            int h = frame["height"];
            animData.frames.emplace_back(x,y,w,h);
        }
        map[name] = animData;
    }
    return map;
}

ANIMATION_PLAYBACK Resource::ParsePlayback(const std::string &str) {
    if(str == "ONCE")
        return ANIMATION_PLAYBACK::ONCE;
    if(str == "LOOP")
        return ANIMATION_PLAYBACK::LOOP;
    if(str == "PING_PONG")
        return ANIMATION_PLAYBACK::PING_PONG;
    return ANIMATION_PLAYBACK::ONCE; //TODO: Warn on wrong value
}

float Resource::Lerp(float t, float start, float end){
    return start + t * (end-start);
}
raylib::Vector2 Resource::Lerp(float t, raylib::Vector2 start, raylib::Vector2 end){
    return start + (end-start) * t;
}
raylib::Vector2 Resource::Bezier(float t, raylib::Vector2 start, raylib::Vector2 control1, raylib::Vector2 control2, raylib::Vector2 end){
    return start*(1.f-t)*(1.f-t)*(1.f-t)+control1*3.f*t*(1.f-t)*(1.f-t)+control2*3.f*t*t*(1-t)+end*t*t*t;
}

void CurveParameter::AddPoint(CurveParameterPoint p) {
    points.push_back(p);
}

float CurveParameter::Eval(float t) {
    // trivial cases
    if(points.empty())
        return 0;

    // Constants
    if(t <= points[0].position.x)
        return points[0].position.y;
    if(t >= points[points.size()-1].position.x)
        return points[points.size()-1].position.y;

    // Find curve portion
    for(int i=0; i < points.size()-1; i++){
        const CurveParameterPoint& p1 = points[i];
        const CurveParameterPoint& p2 = points[i+1];

        // Skip this range
        if(!(t >= p1.position.x && t < p2.position.x) || p1.position.x == p2.position.x)
            continue;

        float T = (t-p1.position.x)/(p2.position.x-p1.position.x);
        // Lerp
        if(p1.rightLinear && p2.leftLinear){
            return Resource::Lerp(T, p1.position.y, p2.position.y);
        }
        // Cubic bezier
        else {
            float tangentLength = (p2.position.x-p1.position.x)/2.f;
            raylib::Vector2 control1;
            if(p1.rightLinear)
                control1 = ((p2.position+p1.position)/2).Normalize();
            else{
                raylib::Vector2 controlOffset1{std::cos(p1.rightAngle), std::sin(p1.rightAngle)};
                control1 = p1.position+controlOffset1*tangentLength;
            }
            raylib::Vector2 control2;
            if(p2.leftLinear)
                control2 = (p2.position+p1.position)/2;
            else{
                raylib::Vector2 controlOffset2{-std::cos(p2.leftAngle), std::sin(p2.leftAngle)};
                control2 = p2.position+controlOffset2*tangentLength;
            }
            return Resource::Bezier(T, p1.position, control1, control2, p2.position).y;
        }
    }
}
