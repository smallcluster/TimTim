//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_GAMEOBJECT_H
#define TIMTIM_GAMEOBJECT_H

#include <optional>
#include <vector>
#include <memory>

#include "raylib-cpp.hpp"
#include "./math.h"


// Base class for all game object in a scene
class GameObject {
public:
    std::optional<GameObject*> parent{};
    Transform2D transform;
    std::string name;
    void AddChild(const std::shared_ptr<GameObject>& child);
    void RemoveChild(const std::shared_ptr<GameObject>& child);
    std::optional<std::shared_ptr<GameObject>> GetChild(const std::string& childName);
    std::shared_ptr<GameObject> GetChild(int index);
    std::vector<std::shared_ptr<GameObject>> GetChildren();
    [[nodiscard]] Transform2D GetGlobalTransform() const;
    virtual void Update(float delta);
    virtual void Draw(const Transform2D& parentGlobalTransform);

private:
    std::vector<std::shared_ptr<GameObject>> children{};
};


#endif //TIMTIM_GAMEOBJECT_H
