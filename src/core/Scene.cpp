//
// Created by pjaff on 24/03/2023.
//

#include "Scene.h"

void Scene::Update(float delta) {
    for(auto& obj : gameObjects){
        obj->Update(delta);
    }
}

void Scene::Draw() {
    for(auto& obj : gameObjects){
        obj->Draw();
    }
}

void Scene::RegisterGameObject(std::shared_ptr<GameObject> obj) {
    gameObjects.push_back(obj);
}

