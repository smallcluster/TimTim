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
    int framerate = 0;
    ANIMATION_PLAYBACK playback = ANIMATION_PLAYBACK::ONCE;
    std::vector<raylib::Rectangle> frames;
};


//TODO: usefulness as a class ?
class Resource {
public:
    // Load all animations described in a json file (see tux.json)
    static std::map<std::string, AnimationData> LoadAnimationsData(const std::string& path);
    // Helper to convert back the string representation of an ANIMATION_PLAYBACK value
    static ANIMATION_PLAYBACK ParsePlayback(const std::string& str);
private:
    Resource()= default;
};


#endif //TIMTIM_RESOURCE_H
