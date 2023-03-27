//
// Created by pjaff on 27/03/2023.
//

#ifndef TIMTIM_CURVEEDITOR_H
#define TIMTIM_CURVEEDITOR_H

#include "raylib-cpp.hpp"
#include "../core/Resource.h"
#include "ContextMenu.h"
#include <vector>
#include <functional>
#include <optional>

class CurveEditor {
public:

    CurveEditor(){
        CurveParameterPoint p;
        p.position = raylib::Vector2(0,0);
        points.push_back(p);
    }

    void DrawAndUpdate(raylib::Rectangle bounds, float fontsize){
        raylib::Rectangle innerBounds{bounds.x+fontsize, bounds.y+fontsize, bounds.width-2*fontsize, bounds.height-2*fontsize};
        raylib::Vector2 mouse{GetMousePosition()};

        // Draw points
        for(auto& p : points){
            raylib::Rectangle pRect{p.position.x-fontsize/2.f, p.position.y-fontsize/2.f, fontsize, fontsize};
            p.position.DrawRectangle({fontsize, fontsize}, BLACK);

            if(mouse.CheckCollision(pRect))
                DrawRectangle(p.position.x-(fontsize+margin)/2.f, p.position.y-(fontsize+margin)/2.f, fontsize+margin, fontsize+margin, BLACK);

        }
    }

    auto GetCurveParameter(){

        return;
    }


private:
    constexpr static const float margin = 4;
    ContextMenu menu;
    std::vector<CurveParameterPoint> points;


//    void Draw(raylib::Rectangle bounds){
//        updateMouse();
//        updateBounds(bounds);
//        hoveredPointIndex = GetHoveredIndex();
//
//        DrawGrid();
//        DrawCurve();
//        DrawPoints();
//
//        raylib::Rectangle contextRect{0,0,0,0};
//
//        if(contextMenu){
//            bool remove = false;
//            contextRect = DrawContextMenu(remove);
//            if(remove && points.size()>1){
//                contextMenu = false;
//                points.erase(points.begin()+selectedPointIndex.value());
//                selectedPointIndex = {};
//            } else if(!mouse.CheckCollision(contextRect) && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)||IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))){
//                contextMenu = false;
//                selectedPointIndex = {};
//            }
//            return;
//        }
//
//        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredPointIndex.has_value()){
//            selectedPointIndex = hoveredPointIndex;
//            contextMenu = true;
//            contextMenuPos = mouse;
//        } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && mouse.CheckCollision(innerBounds)) {
//            CurvePoint p;
//            p.position = ScreenToLocal(mouse);
//            points.push_back(p);
//            contextMenu = false;
//            selectedPointIndex = {};
//        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredPointIndex.has_value()){
//            selectedPointIndex = hoveredPointIndex;
//            contextMenu = false;
//        } else if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) && !hoveredPointIndex.has_value()){
//            selectedPointIndex = {};
//            contextMenu = false;
//        }
//
//
//        if(selectedPointIndex.has_value() && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
//            auto& p = points[selectedPointIndex.value()];
//            p.position = ScreenToLocal(mouse);
//        }
//
//    }
//
//
//private:
//    CurveParameter curveParameter;
//
//
//    float fontSize = 12;
//    float handleLength = 2.5*fontSize;
//    float hoverSize = fontSize + 4;
//    raylib::Color lineColor = raylib::GRAY;
//
//    raylib::Vector2 mouse;
//    std::vector<CurvePoint> points;
//    std::optional<int> selectedPointIndex;
//    std::optional<int> hoveredPointIndex;
//    raylib::Rectangle innerBounds;
//    raylib::Rectangle bounds;
//    bool contextMenu = false;
//    raylib::Vector2 contextMenuPos{0,0};
//
//    raylib::Rectangle DrawContextMenu(bool& removeSelected){
//        CurvePoint& p = points[selectedPointIndex.value()];
//        raylib::Rectangle pannel{contextMenuPos.x, contextMenuPos.y, 128, 3*24};
//        raylib::Rectangle removeRect{contextMenuPos.x, contextMenuPos.y, 128, 24};
//        raylib::Rectangle rightLinearRect{contextMenuPos.x, contextMenuPos.y+24, 128, 24};
//        raylib::Rectangle leftLinearRect{contextMenuPos.x, contextMenuPos.y+2*24, 128, 24};
//        float txtDy = (24-12)/2.f;
//
//        removeSelected = false;
//        pannel.Draw(LIGHTGRAY);
//        pannel.DrawLines(BLACK);
//
//        // Remove
//        raylib::Color drawColor{BLACK};
//        if(mouse.CheckCollision(removeRect)){
//            drawColor = BLUE;
//            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
//                removeSelected = true;
//        }
//
//        removeRect.DrawLines(drawColor);
//        DrawText("Remove", removeRect.x+4, removeRect.y+txtDy, 12, drawColor);
//        drawColor = BLACK;
//
//        if(mouse.CheckCollision(rightLinearRect)){
//            drawColor = BLUE;
//            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
//                p.rightLinear = !p.rightLinear;
//        }
//        rightLinearRect.DrawLines(drawColor);
//        if(p.rightLinear)
//            DrawRectangle(rightLinearRect.x+4, rightLinearRect.y+4, 16, 16, BLUE);
//        else
//            DrawRectangle(rightLinearRect.x+4, rightLinearRect.y+4, 16, 16, GRAY);
//
//        DrawText("Right linear", rightLinearRect.x+24, rightLinearRect.y+txtDy, 12, drawColor);
//        drawColor = BLACK;
//
//        if(mouse.CheckCollision(leftLinearRect)){
//            drawColor = BLUE;
//            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
//                p.leftLinear = !p.leftLinear;
//        }
//        leftLinearRect.DrawLines(drawColor);
//        if(p.leftLinear)
//            DrawRectangle(leftLinearRect.x+4, leftLinearRect.y+4, 16, 16, BLUE);
//        else
//            DrawRectangle(leftLinearRect.x+4, leftLinearRect.y+4, 16, 16, GRAY);
//
//        DrawText("Left linear", leftLinearRect.x+24, leftLinearRect.y+txtDy, 12, drawColor);
//        drawColor = BLACK;
//
//        return pannel;
//    }
//
//    raylib::Vector2 LocalToScreen(const raylib::Vector2& p){
//        raylib::Vector2 res{innerBounds.x+innerBounds.width*p.x, innerBounds.y+innerBounds.height*(1.f-p.y)};
//        return res;
//    }
//    raylib::Vector2 ScreenToLocal(const raylib::Vector2& p){
//        float x = std::clamp(p.x, innerBounds.x, innerBounds.x+innerBounds.width);
//        float y = std::clamp(p.y, innerBounds.y, innerBounds.y+innerBounds.height);
//        raylib::Vector2 res{(x-innerBounds.x) / innerBounds.width, (innerBounds.y+innerBounds.height-y) / innerBounds.height};
//        return res;
//    }
//
//    std::optional<int> GetHoveredIndex(){
//        for(int i=0; i < points.size(); i++){
//            auto& p = points[i];
//            if(mouse.CheckCollision(GetPointRect(p)))
//                return {i};
//        }
//        return {};
//    }
//
//    raylib::Rectangle GetPointRect(CurvePoint p){
//        auto sp = LocalToScreen(p.position);
//        return {sp.x-fontSize/2.f, sp.y-fontSize/2.f, fontSize, fontSize};
//    }
//
//    void updateBounds(raylib::Rectangle bounds) {
//        this->bounds = bounds;
//        innerBounds.x = bounds.x+fontSize;
//        innerBounds.y = bounds.y + fontSize;
//        innerBounds.width = bounds.width-2*fontSize;
//        innerBounds.height = bounds.height-2*fontSize;
//    }
//    void updateMouse(){
//        mouse.x = GetMouseX();
//        mouse.y = GetMouseY();
//    }
//    void DrawGrid(){
//        bounds.Draw(WHITE); // BG
//        LocalToScreen({0,0}).DrawLine(LocalToScreen({1,0}), lineColor);
//        LocalToScreen({0,1}).DrawLine(LocalToScreen({1,1}), lineColor);
//        LocalToScreen({0,0.5}).DrawLine(LocalToScreen({1,0.5}), lineColor);
//        LocalToScreen({0,0}).DrawLine(LocalToScreen({0,1}), lineColor);
//        LocalToScreen({1,0}).DrawLine(LocalToScreen({1,1}), lineColor);
//        LocalToScreen({0.5,0}).DrawLine(LocalToScreen({0.5,1}), lineColor);
//        LocalToScreen({0.75,0}).DrawLine(LocalToScreen({0.75,1}), lineColor);
//        LocalToScreen({0.25,0}).DrawLine(LocalToScreen({0.25,1}), lineColor);
//        raylib::DrawText("0.0", bounds.x+fontSize, bounds.y+bounds.height-fontSize, fontSize, lineColor);
//        raylib::DrawText("0.5", bounds.x+bounds.width/2.f, bounds.y+bounds.height-fontSize, fontSize, lineColor);
//        raylib::DrawText("1.0", bounds.x+bounds.width-fontSize, bounds.y+bounds.height-fontSize, fontSize, lineColor);
//        raylib::DrawText("0.25", innerBounds.x+innerBounds.width/4.f, bounds.y+bounds.height-fontSize, fontSize, lineColor);
//        raylib::DrawText("0.75", innerBounds.x+3*innerBounds.width/4.f, bounds.y+bounds.height-fontSize, fontSize, lineColor);
//        raylib::DrawText("0", bounds.x+fontSize, bounds.y+bounds.height-2*fontSize, fontSize, lineColor);
//        raylib::DrawText("1", bounds.x+fontSize, bounds.y, fontSize, lineColor);
//        raylib::DrawText("0.5", bounds.x+fontSize, bounds.y+bounds.height/2.f-fontSize, fontSize, lineColor);
//    }
//
//    void DrawPoints(){
//
//
//        auto sortedPoints = GetSortedPoints();
//        float minX = sortedPoints[0].position.x;
//        float maxX = sortedPoints[sortedPoints.size()-1].position.x;
//
//
//        for(int i=0; i < points.size(); i++){
//            auto& p = points[i];
//            raylib::Vector2 drawPos = LocalToScreen(p.position);
//
//            if(selectedPointIndex.has_value() && selectedPointIndex.value() == i){
//                DrawRectangle(drawPos.x-fontSize/2.f,drawPos.y-fontSize/2.f,  fontSize, fontSize, BLUE);
//                // draw anchors
//                if(!p.rightLinear && p.position.x < maxX)
//                    DrawLine(drawPos.x, drawPos.y, drawPos.x+std::cos(p.rightAngle)*handleLength, drawPos.y+std::sin(p.rightAngle)*handleLength, BLUE);
//                if(!p.leftLinear && p.position.x > minX)
//                    DrawLine(drawPos.x, drawPos.y, drawPos.x-std::cos(p.leftAngle)*handleLength, drawPos.y+std::sin(p.leftAngle)*handleLength, BLUE);
//            } else {
//                DrawRectangle(drawPos.x-fontSize/2.f,drawPos.y-fontSize/2.f,  fontSize, fontSize, BLACK);
//            }
//            if(hoveredPointIndex.has_value() && hoveredPointIndex.value() == i){
//                DrawRectangleLines(drawPos.x-hoverSize/2.f,drawPos.y-hoverSize/2.f,  hoverSize, hoverSize, BLACK);
//            }
//        }
//    }
//
//    std::vector<CurvePoint> GetSortedPoints(bool addEdges = false){
//        std::vector<CurvePoint> sortedPoints(points.size());
//        for(int i=0; i < points.size(); i++) sortedPoints[i] = points[i];
//        std::sort(sortedPoints.begin(), sortedPoints.end(), [](const CurvePoint& p1, const CurvePoint& p2) {return p1.position.x < p2.position.x;});
//
//        if(sortedPoints[0].position.x > 0 && addEdges){
//            sortedPoints[0].leftLinear = true;
//            CurvePoint edge;
//            edge.rightLinear = true;
//            edge.position.x = 0;
//            edge.position.y = sortedPoints[0].position.y;
//            sortedPoints.insert(sortedPoints.begin(), edge);
//        }
//        if(sortedPoints[sortedPoints.size()-1].position.x < 1 && addEdges){
//            CurvePoint edge;
//            sortedPoints[sortedPoints.size()-1].rightLinear = true;
//            edge.leftLinear = true;
//            edge.position.x = 1;
//            edge.position.y = sortedPoints[sortedPoints.size()-1].position.y;
//            sortedPoints.push_back(edge);
//        }
//        return std::move(sortedPoints);
//    }
//
//    void DrawCurve(){
//        // Sort points on x axis
//        auto sortedPoints = GetSortedPoints(true);
//
//        for(int i=0; i < sortedPoints.size()-1; i++){
//            const CurvePoint& p = sortedPoints[i];
//            const CurvePoint& p2 = sortedPoints[i+1];
//            auto pos1 = LocalToScreen(p.position);
//            auto pos2 = LocalToScreen(p2.position);
//
//
//            float tangentLength = (p2.position.x-p.position.x)/2.f;
//
//            raylib::Vector2 control1;
//            if(p.rightLinear)
//                control1 = LocalToScreen((p2.position+p.position)/2);
//            else{
//                raylib::Vector2 controlOffset1{std::cos(p.rightAngle), std::sin(p.rightAngle)};
//                control1 = LocalToScreen(p.position+controlOffset1*tangentLength);
//            }
//
//            raylib::Vector2 control2;
//            if(p2.leftLinear)
//                control2 = LocalToScreen((p2.position+p.position)/2);
//            else{
//                raylib::Vector2 controlOffset2{-std::cos(p2.leftAngle), std::sin(p2.leftAngle)};
//                control2 = LocalToScreen(p2.position+controlOffset2*tangentLength);
//            }
//
//            DrawLineBezierCubic(pos1,pos2, control1, control2, 1, BLACK);
//        }
//    }
};


#endif //TIMTIM_CURVEEDITOR_H
