//
// Created by pjaff on 29/04/2023.
//

#include "./math.h"

Transform2D Transform2D::Identity() {
    return {{0,0}, {1,1}, 0};
}

Transform2D Transform2D::operator*(const Transform2D& other) const{
    float x = position.x + (std::cos(rotation * DEG2RAD)*other.position.x - std::sin(rotation * DEG2RAD)*other.position.y) * scale.x;
    float y = position.y + (std::sin(rotation * DEG2RAD)*other.position.x + std::cos(rotation * DEG2RAD)*other.position.y) * scale.y;
    return {{x, y}, scale * other.scale, rotation + other.rotation};
}

void Transform2D::DrawDebug(const Transform2D& parentGlobalTransform){
    Transform2D gTransform = parentGlobalTransform * (*this);
    DrawLine(gTransform.position.x,gTransform.position.y, gTransform.position.x + std::cos(gTransform.rotation*DEG2RAD)*16*gTransform.scale.x, gTransform.position.y + std::sin(gTransform.rotation*DEG2RAD)*16*gTransform.scale.y, RED);
    DrawLine(gTransform.position.x,gTransform.position.y, gTransform.position.x - std::sin(gTransform.rotation*DEG2RAD)*16*gTransform.scale.x, gTransform.position.y + std::cos(gTransform.rotation*DEG2RAD)*16*gTransform.scale.y, LIME);
}