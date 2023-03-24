//
// Created by pjaff on 24/03/2023.
//

#include "Scene.h"

void Scene::Update(float delta) {
    for(GameObject* obj : _gameObjects){
        obj->Update(delta);
    }
}

void Scene::Draw() {
    for(GameObject* obj : _gameObjects){
        obj->Draw();
    }
}

void Scene::RegisterGameObject(GameObject *obj) {
    _gameObjects.push_back(obj);
}

Scene::~Scene() {
    for(GameObject* obj : _gameObjects){
        delete obj;
    }
}

