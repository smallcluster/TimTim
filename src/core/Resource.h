//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_RESOURCE_H
#define TIMTIM_RESOURCE_H

#include "raylib-cpp.hpp"
#include <vector>
#include <map>
#include <string>
#include "nlohmann/json.hpp"
using json = nlohmann::json;


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
    raylib::Vector2 tangents{0,0};
    bool rightLinear{false};
    bool leftLinear{false};
};

// A curve to interpolate a float value form 0 to 1
class CurveParameter {
public:
    float min{0};
    float max{1};
    CurveParameter() = default;
    CurveParameter(std::vector<CurveParameterPoint> data);
    CurveParameter(const std::string& path);

    void SetData(std::vector<CurveParameterPoint> data);
    void SaveData(const std::string& path) const;
    void LoadData(const std::string& path);

    float Eval(float t);
    std::vector<CurveParameterPoint>* GetData();
private:
    [[nodiscard]] json GetJsonRep() const;
    // points are sorted along x axis
    std::vector<CurveParameterPoint> points;
};

// ColorRamp
class ColorRamp{
public:
    ColorRamp() = default;
    ColorRamp(std::vector<std::pair<raylib::Color, float>> data);
    ColorRamp(const std::string& path);

    void SetData(std::vector<std::pair<raylib::Color, float>> data);
    void SaveData(const std::string& path) const;
    void LoadData(const std::string& path);

    raylib::Color Eval(float t);
    std::vector<std::pair<raylib::Color, float>>* GetData();
private:
    [[nodiscard]] json GetJsonRep() const;
    std::vector<std::pair<raylib::Color, float>> colors;
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
