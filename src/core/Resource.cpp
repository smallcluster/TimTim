//
// Created by pjaff on 24/03/2023.
//

#include "Resource.h"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

//----------------------------------------------------------------------------------------------------------------------
// ANIMATIONS
//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------
// CURVE PARAMETER
//----------------------------------------------------------------------------------------------------------------------

CurveParameter::CurveParameter(std::vector<CurveParameterPoint> data) : points(std::move(data)) {
}

void CurveParameter::SetData(std::vector<CurveParameterPoint> data) {
    points = std::move(data);
}

CurveParameter::CurveParameter(const std::string &path) {
    LoadData(path);
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
        float scale = (p2.position.x-p1.position.x);
        float T = (t-p1.position.x)/scale;
        float startTangent = scale * p1.tangents.y;
        float endTangent = scale * p2.tangents.x;
        float T2 = T*T;
        float T3 = T*T*T;
        return (2*T3-3*T2+1)*p1.position.y+(T3-2*T2+T)*startTangent+(3*T2-2*T3)*p2.position.y+(T3-T2)*endTangent;
    }
}

json CurveParameter::GetJsonRep() const {
    json j;
    j["data"] = json::array();
    for(auto& p : points){
        json d;
        d["position"] = json::array({p.position.x, p.position.y});
        d["tangents"] = json::array({p.tangents.x, p.tangents.y});
        j["data"].push_back(d);
    }
    return std::move(j);
}

void CurveParameter::SaveData(const std::string &path) const {
    std::ofstream file(path);
    file << GetJsonRep();
}

void CurveParameter::LoadData(const std::string &path) {
    std::ifstream file(path);
    json j = json::parse(file);
    points.clear();
    for(auto& d : j["data"]){
        CurveParameterPoint p;
        p.position = raylib::Vector2{d["position"][0], d["position"][1]};
        p.tangents = raylib::Vector2{d["tangents"][0], d["tangents"][1]};
        points.push_back(p);
    }
}

std::vector<CurveParameterPoint> *CurveParameter::GetData() {
    return &points;
}






