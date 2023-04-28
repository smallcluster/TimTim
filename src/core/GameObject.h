//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_GAMEOBJECT_H
#define TIMTIM_GAMEOBJECT_H

#include <optional>
#include <vector>
#include <memory>

#include "raylib-cpp.hpp"

struct Transform2D{
    raylib::Vector2 position{0,0};
    raylib::Vector2 scale{1,1};
    float rotation{0}; // in deg

    static Transform2D Identity() {
        return {{0,0}, {1,1}, 0};
    }

    Transform2D operator*(const Transform2D& other) const{
        float x = position.x + (std::cos(rotation * DEG2RAD)*other.position.x - std::sin(rotation * DEG2RAD)*other.position.y) * scale.x;
        float y = position.y + (std::sin(rotation * DEG2RAD)*other.position.x + std::cos(rotation * DEG2RAD)*other.position.y) * scale.y;
        return {{x, y}, scale * other.scale, rotation + other.rotation};
    }

    void DrawDebug(const Transform2D& parentGlobalTransform){
        Transform2D gTransform = parentGlobalTransform * (*this);
        DrawLine(gTransform.position.x,gTransform.position.y, gTransform.position.x + std::cos(gTransform.rotation*DEG2RAD)*16*gTransform.scale.x, gTransform.position.y + std::sin(gTransform.rotation*DEG2RAD)*16*gTransform.scale.y, RED);
        DrawLine(gTransform.position.x,gTransform.position.y, gTransform.position.x - std::sin(gTransform.rotation*DEG2RAD)*16*gTransform.scale.x, gTransform.position.y + std::cos(gTransform.rotation*DEG2RAD)*16*gTransform.scale.y, LIME);
    }
};


// Base class for all game object in a scene
class GameObject {
public:
    std::optional<GameObject*> parent{};
    Transform2D transform;
    std::string name;

    void AddChild(const std::shared_ptr<GameObject>& child){
        child->parent = {this};
        children.push_back(child);
    }

    void RemoveChild(const std::shared_ptr<GameObject>& child){
        child->parent = {};
        children.erase(std::find_if(children.begin(), children.end(), [child](const std::shared_ptr<GameObject>& c){return c == child;}));
    }
    std::optional<std::shared_ptr<GameObject>> GetChild(const std::string& childName){
        auto it = std::find_if(children.begin(), children.end(),[&childName](const std::shared_ptr<GameObject>& c){return c->name == childName;});
        if(it != children.end())
            return {children[it - children.begin()]};
        return {};
    }
    std::shared_ptr<GameObject> GetChild(int index){
        return children[index];
    }
    std::vector<std::shared_ptr<GameObject>> GetChildren(){
        return children;
    }

    [[nodiscard]] Transform2D GetGlobalTransform() const {
        Transform2D t = transform;
        auto p = parent;
        while (p){
            t = t * p.value()->transform;
            p = p.value()->parent;
        }
        return t;
    }

    virtual void Update(float delta){
        for(const std::shared_ptr<GameObject>& c : children){
            c->Update(delta);
        }
    }

    virtual void Draw(const Transform2D& parentGlobalTransform){
        Transform2D gTransform = parentGlobalTransform*transform;
        for(const std::shared_ptr<GameObject>& c : children){
            c->Draw(gTransform);
        }
    }

private:
    std::vector<std::shared_ptr<GameObject>> children{};
};


#endif //TIMTIM_GAMEOBJECT_H
