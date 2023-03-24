//
// Created by pjaff on 24/03/2023.
//

#ifndef TIMTIM_SCENE_H
#define TIMTIM_SCENE_H

#include "GameObject.h"
#include <vector>

// Basic scene
// A scene will update and draw registered gameobject.
// All registered game object are managed by the scene.
// All registered game object are freed on scene destruction.
class Scene {
public:
    virtual void Update(float delta);
    virtual void Draw();
    void RegisterGameObject(GameObject* obj);
    ~Scene();

private:
    std::vector<GameObject*> _gameObjects;
};

#endif //TIMTIM_SCENE_H