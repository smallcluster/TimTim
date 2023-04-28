//
// Created by pjaff on 29/04/2023.
//

#include "GameObject.h"

void GameObject::AddChild(const std::shared_ptr<GameObject>& child){
    child->parent = {this};
    children.push_back(child);
}

void GameObject::RemoveChild(const std::shared_ptr<GameObject>& child){
    child->parent = {};
    children.erase(std::find_if(children.begin(), children.end(), [child](const std::shared_ptr<GameObject>& c){return c == child;}));
}
std::optional<std::shared_ptr<GameObject>> GameObject::GetChild(const std::string& childName){
    auto it = std::find_if(children.begin(), children.end(),[&childName](const std::shared_ptr<GameObject>& c){return c->name == childName;});
    if(it != children.end())
        return {children[it - children.begin()]};
    return {};
}
std::shared_ptr<GameObject> GameObject::GetChild(int index){
    return children[index];
}
std::vector<std::shared_ptr<GameObject>> GameObject::GetChildren(){
    return children;
}

[[nodiscard]] Transform2D GameObject::GetGlobalTransform() const {
    Transform2D t = transform;
    auto p = parent;
    while (p){
        t = t * p.value()->transform;
        p = p.value()->parent;
    }
    return t;
}

void GameObject::Update(float delta){
    for(const std::shared_ptr<GameObject>& c : children){
        c->Update(delta);
    }
}

void GameObject::Draw(const Transform2D& parentGlobalTransform){
    Transform2D gTransform = parentGlobalTransform*transform;
    for(const std::shared_ptr<GameObject>& c : children){
        c->Draw(gTransform);
    }
}