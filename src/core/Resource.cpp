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
