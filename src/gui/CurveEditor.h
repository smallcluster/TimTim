//
// Created by pjaff on 27/03/2023.
//

#ifndef TIMTIM_CURVEEDITOR_H
#define TIMTIM_CURVEEDITOR_H

#include "raylib-cpp.hpp"
#include "../external/raygui/raygui.h"
#include "../core/Resource.h"
#include "ContextMenu.h"
#include <vector>
#include <functional>
#include <optional>
#include <iostream>

class CurveEditor {
public:

    CurveEditor(){
        // Initial point
        CurveParameterPoint p;
        p.position = raylib::Vector2{0.5, 1};
        p.tangents = raylib::Vector2{0, 0};
        points.push_back(p);
    }

    void DrawAndUpdate(raylib::Rectangle bounds, float fontsize){
        innerBounds = raylib::Rectangle{bounds.x+fontsize, bounds.y+fontsize, bounds.width-2*fontsize, bounds.height-2*fontsize};
        raylib::Vector2 mouse{GetMousePosition()};
        const float handleLength = fontsize*3;
        const float handleSize = fontsize/1.5f;

        auto hoveredPoint = GetHoveredPoint(fontsize);

        BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);
        // Draw bg
        bounds.Draw(WHITE);

        // Draw grid
        // H lines
        LocalToScreen({0,0}).DrawLine(LocalToScreen({1,0}), LIGHTGRAY); // top
        LocalToScreen({0,1}).DrawLine(LocalToScreen({1,1}), LIGHTGRAY); // bottom
        LocalToScreen({0,0.5}).DrawLine(LocalToScreen({1,0.5}), LIGHTGRAY); // middle
        // V lines
        LocalToScreen({0,0}).DrawLine(LocalToScreen({0,1}), LIGHTGRAY); // left
        LocalToScreen({1,0}).DrawLine(LocalToScreen({1,1}), LIGHTGRAY); // right
        LocalToScreen({0.5,0}).DrawLine(LocalToScreen({0.5,1}), LIGHTGRAY); // middle
        LocalToScreen({0.25,0}).DrawLine(LocalToScreen({0.25,1}), LIGHTGRAY); // 0.25
        LocalToScreen({0.75,0}).DrawLine(LocalToScreen({0.75,1}), LIGHTGRAY); // 0.75
        // V labels
        float y = bounds.y+bounds.height-fontsize;
        float x = innerBounds.x;
        raylib::DrawText("0", x, y, fontsize, LIGHTGRAY);
        raylib::DrawText("0.25", x+innerBounds.width/4.f, y, fontsize, LIGHTGRAY);
        raylib::DrawText("0.5", x+innerBounds.width/2.f, y, fontsize, LIGHTGRAY);
        raylib::DrawText("0.75", x+3.f*innerBounds.width/4.f, y, fontsize, LIGHTGRAY);
        raylib::DrawText("1", x+innerBounds.width, y, fontsize, LIGHTGRAY);

        // H labels
        x = innerBounds.x;
        y = innerBounds.y-fontsize;
        raylib::DrawText("0", x, y+innerBounds.height, fontsize, LIGHTGRAY);
        raylib::DrawText("0.5", x, y+innerBounds.height/2.f, fontsize, LIGHTGRAY);
        raylib::DrawText("1", x, y+fontsize, fontsize, LIGHTGRAY);

        if(points.empty())
            return;

        auto sortedPoint = GetSortedPointsPtr();

        // Update linear handles positions
        for(int i=0; i < sortedPoint.size(); i++){
            auto* p = sortedPoint[i];
            // left
            if(i > 0 && p->leftLinear){
                auto* p2 = sortedPoint[i-1];
                auto dir = p2->position-p->position;
                p->tangents.x = dir.x == 0 ? 0 : dir.y/dir.x;
            }
            // right
            if(i < sortedPoint.size()-1 && p->rightLinear){
                auto* p2 = sortedPoint[i+1];
                auto dir = p2->position-p->position;
                p->tangents.y = dir.x == 0 ? 0 : dir.y/dir.x;
            }
        }

        // Draw points
        for(int i=0; i < sortedPoint.size(); i++){
            const auto* p = sortedPoint[i];
            raylib::Vector2 screenPos = LocalToScreen(p->position);

            // Draw point
            if(selectedPoint && selectedPoint.value() == p){
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, BLUE);
                // Draw left handle
                if(i > 0){
                    raylib::Vector2 controlOffset{-1, -p->tangents.x};
                    auto target = LocalToScreen(p->position+controlOffset);
                    auto dir = target - screenPos;
                    raylib::Vector2 control = screenPos + dir.Normalize() * handleLength;
                    raylib::Rectangle controlRect{control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};
                    if(mouse.CheckCollision(controlRect) || leftControlSelected){
                        controlRect.Draw(BLUE);
                        screenPos.DrawLine(control, BLUE);
                    }else{
                        controlRect.Draw(GRAY);
                        screenPos.DrawLine(control, GRAY);
                    }

                }
                // Draw right handle
                if(i < points.size()-1){
                    raylib::Vector2 controlOffset{1, p->tangents.y};
                    auto target = LocalToScreen(p->position+controlOffset);
                    auto dir = target - screenPos;
                    raylib::Vector2 control = screenPos + dir.Normalize() * handleLength;
                    raylib::Rectangle controlRect{control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};
                    if(mouse.CheckCollision(controlRect) || leftControlSelected){
                        controlRect.Draw(BLUE);
                        screenPos.DrawLine(control, BLUE);
                    }else{
                        controlRect.Draw(GRAY);
                        screenPos.DrawLine(control, GRAY);
                    }
                }
            } else {
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, GRAY);
            }

            // point hover
            if(hoveredPoint && hoveredPoint.value() == p)
                DrawRectangleLines(screenPos.x-(fontsize+margin)/2.f, screenPos.y-(fontsize+margin)/2.f, fontsize+margin, fontsize+margin, BLACK);
        }

        // Draw Curve
        if(sortedPoint.size() == 1){
            auto* p = sortedPoint[0];
            auto pos = LocalToScreen(p->position);
            DrawLine(innerBounds.x, pos.y, innerBounds.x+innerBounds.width, pos.y, BLACK);
        }else {
            for(int i=0; i < sortedPoint.size()-1; i++){
                const CurveParameterPoint* p1 = sortedPoint[i];
                const CurveParameterPoint* p2 = sortedPoint[i+1];
                auto pos1 = LocalToScreen(p1->position);
                auto pos2 = LocalToScreen(p2->position);
                // Constant on edge
                if(pos1.x > innerBounds.x && i == 0){
                    DrawLine(innerBounds.x, pos1.y, pos1.x, pos1.y, BLACK);
                }
                if(pos2.x < innerBounds.x+innerBounds.width && i == sortedPoint.size()-2){
                    DrawLine(pos2.x, pos2.y, innerBounds.x+innerBounds.width, pos2.y, BLACK);
                }
                DrawHermiteNormalized(p1->position, p2->position, p1->tangents.y, p2->tangents.x);
            }
        }

        EndScissorMode();


        //Draw Menu && update choice
        if(menu.IsVisible()){
            menu.DrawAndUpdate(fontsize);
            auto item = menu.GetSelected();
            if(item){
                switch (item.value()->index+(points.size() == 1)) {
                    case 0:{
                        menu.Hide();
                        if(points.size() > 1){
                            CurveParameterPoint* selected = selectedPoint.value();
                            auto it = std::find_if(points.begin(), points.end(), [selected](const CurveParameterPoint& p){return &p == selected;});
                            points.erase(it);
                            selectedPoint = {};
                        }
                        break;
                    }
                    case 1:{
                        selectedPoint.value()->rightLinear = item.value()->checked;
                        break;
                    }
                    case 2:{
                        selectedPoint.value()->leftLinear = item.value()->checked;
                        break;
                    }
                }
            }
        }


        // Close/open menu
        if(!menu.IsVisible()){

            // Select handle
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && selectedPoint){

                auto* p = selectedPoint.value();
                auto screenPos = LocalToScreen(p->position);

                // Left control
                raylib::Vector2 offsetLeft{-1, -p->tangents.x};
                auto dirLeft = LocalToScreen(p->position+offsetLeft)- screenPos;
                raylib::Vector2 controlLeft = screenPos + dirLeft.Normalize() * handleLength;

                if(mouse.CheckCollision({controlLeft.x-handleSize/2.f, controlLeft.y-handleSize/2.f, handleSize, handleSize})){
                    leftControlSelected = true;
                    p->leftLinear = false;
                }

                // Right control
                raylib::Vector2 offsetRight{1, p->tangents.y};
                auto dirRight = LocalToScreen(p->position+offsetRight)- screenPos;
                raylib::Vector2 controlRight = screenPos + dirRight.Normalize() * handleLength;

                if(mouse.CheckCollision({controlRight.x-handleSize/2.f, controlRight.y-handleSize/2.f, handleSize, handleSize})){
                    rightControlSelected = true;
                    p->rightLinear = false;
                }
            }

            // Unselect handles
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                rightControlSelected = false;
                leftControlSelected = false;
            }

            // move handle
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && rightControlSelected){
                auto* p = selectedPoint.value();
                auto dir = ScreenToLocal(mouse)-p->position;
                p->tangents.y = dir.x < 0.001f ? dir.y/0.001f : dir.y/dir.x;
            } else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && leftControlSelected){
                auto* p = selectedPoint.value();
                auto dir = ScreenToLocal(mouse)-p->position;
                p->tangents.x = dir.x > -0.001f ? dir.y/(-0.001f) : dir.y/dir.x;
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredPoint){
                selectedPoint = hoveredPoint;
                auto* p = selectedPoint.value();
                mouseOffest = LocalToScreen(p->position) - mouse;
            }
            else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hoveredPoint.has_value())
                Deselect();
            else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredPoint.has_value()){
                selectedPoint = hoveredPoint;
                menu = ContextMenu{};
                menu.position = mouse;
                if(points.size() > 1)
                    menu.AddItem("Remove");
                menu.AddItem("Right linear", true, selectedPoint.value()->rightLinear);
                menu.AddItem("Left linear", true, selectedPoint.value()->leftLinear);
                menu.Show();

            } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && mouse.CheckCollision(innerBounds)){
                CurveParameterPoint p;
                p.position =  ScreenToLocal(mouse);
                points.push_back(p);
            } else if(selectedPoint && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                selectedPoint.value()->position = Vector2Clamp(ScreenToLocal(mouse+mouseOffest), {0,0}, {1,1});
            }
        }
    }

    [[nodiscard]] CurveParameter GetCurveParameter() const {
        return {GetSortedPoints()};
    }

    void SaveCurve(const std::string& path) const{
        GetCurveParameter().SaveData(path);
    }

    void LoadCurve(const std::string& path) {
        CurveParameter c(path);
        auto data = *c.GetData();
        points = std::vector{data};
        selectedPoint = {};
    }

    [[nodiscard]] bool IsMenuVisible() const {
        return menu.IsVisible();
    }

    auto GetSelectedPoint(){
        return selectedPoint;
    }

    void Deselect(){
        selectedPoint = {};
        rightControlSelected = false;
        leftControlSelected = false;
    }

private:
    constexpr static const float margin = 4;
    ContextMenu menu;
    raylib::Rectangle innerBounds;
    raylib::Vector2 mouseOffest{0,0};
    std::vector<CurveParameterPoint> points;
    std::optional<CurveParameterPoint*> selectedPoint;
    bool rightControlSelected = false;
    bool leftControlSelected = false;

    void DrawHermiteNormalized(raylib::Vector2 start, raylib::Vector2 end, float startTangent, float endTangent){
        float scale = (end.x-start.x)/3.f;
        raylib::Vector2 offset1{scale, scale*startTangent};
        // negative endTangent => top part => need to invert value to calculate offset
        raylib::Vector2 offset2{-scale, -scale*endTangent};
        auto c1 = start + offset1;
        auto c2 = end + offset2;
        DrawLineBezierCubic(LocalToScreen(start),LocalToScreen(end), LocalToScreen(c1), LocalToScreen(c2), 1, BLACK);
    }

    std::optional<CurveParameterPoint*> GetHoveredPoint(float fontsize) {
        raylib::Vector2 mouse{GetMousePosition()};
        for(auto& p : points){
            raylib::Vector2 screenPos = LocalToScreen(p.position);
            raylib::Rectangle pRect{screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize};
            if(mouse.CheckCollision(pRect))
                return {&p};
        }
        return {};
    }

    [[nodiscard]] raylib::Vector2 LocalToScreen(const raylib::Vector2& position) const {
        return {position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-position.y*innerBounds.height};
    }
    [[nodiscard]] raylib::Vector2 ScreenToLocal(const raylib::Vector2& position) const {
        return {(position.x-innerBounds.x)/innerBounds.width, (innerBounds.y+innerBounds.height-position.y)/innerBounds.height};
    }

    [[nodiscard]] std::vector<CurveParameterPoint> GetSortedPoints() const {
        std::vector<CurveParameterPoint> sortedPoint(points);
        std::sort(sortedPoint.begin(), sortedPoint.end(), [](const CurveParameterPoint& p1, const CurveParameterPoint& p2){return p1.position.x < p2.position.x;});
        return std::move(sortedPoint);
    }

    std::vector<CurveParameterPoint*> GetSortedPointsPtr(){
        std::vector<CurveParameterPoint*> sortedPoint(points.size());
        for(int i=0; i < points.size(); i++) sortedPoint[i] = &points[i];
        std::sort(sortedPoint.begin(), sortedPoint.end(), [](const CurveParameterPoint* p1, const CurveParameterPoint* p2){return p1->position.x < p2->position.x;});
        return std::move(sortedPoint);
    }


};


#endif //TIMTIM_CURVEEDITOR_H
