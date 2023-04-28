//
// Created by pjaff on 24/03/2023.
//

#include "Scene.h"

void Scene::Update(float delta) {
    root.Update(delta);
}

void Scene::Draw() {
    root.Draw(Transform2D::Identity());
}

GameObject* Scene::GetRoot(){
    return &root;
}

