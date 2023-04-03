//
// Created by pjaff on 27/03/2023.
//

#ifndef TIMTIM_CURVEEDITOR_H
#define TIMTIM_CURVEEDITOR_H

#include "raylib-cpp.hpp"
#include "../external/raygui/raygui.h"
#include "../core/Resource.h"
#include <vector>
#include <functional>
#include <optional>
#include <iostream>
#include "fmt/format.h"

class CurveEditor {
public:

    float min{0};
    float max{1};

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
        raylib::Color bgColor = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
        bounds.Draw(bgColor);

        // Draw grid
        // H lines
        raylib::Color lineColor = GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL);

        LocalToScreen({0,0}).DrawLine(LocalToScreen({1,0}), lineColor); // top
        LocalToScreen({0,1}).DrawLine(LocalToScreen({1,1}), lineColor); // bottom
        LocalToScreen({0,0.5}).DrawLine(LocalToScreen({1,0.5}), lineColor); // middle
        // V lines
        LocalToScreen({0,0}).DrawLine(LocalToScreen({0,1}), lineColor); // left
        LocalToScreen({1,0}).DrawLine(LocalToScreen({1,1}), lineColor); // right
        LocalToScreen({0.5,0}).DrawLine(LocalToScreen({0.5,1}), lineColor); // middle
        LocalToScreen({0.25,0}).DrawLine(LocalToScreen({0.25,1}), lineColor); // 0.25
        LocalToScreen({0.75,0}).DrawLine(LocalToScreen({0.75,1}), lineColor); // 0.75
        // V labels
        float y = bounds.y+bounds.height-fontsize;
        float x = innerBounds.x;
        raylib::DrawText("0", x, y, fontsize, lineColor);
        raylib::DrawText("0.25", x+innerBounds.width/4.f, y, fontsize, lineColor);
        raylib::DrawText("0.5", x+innerBounds.width/2.f, y, fontsize, lineColor);
        raylib::DrawText("0.75", x+3.f*innerBounds.width/4.f, y, fontsize, lineColor);
        raylib::DrawText("1", x+innerBounds.width, y, fontsize, lineColor);

        // H labels
        x = innerBounds.x;
        y = innerBounds.y-fontsize;

        std::string minStr = fmt::format("{:.2f}", min);
        std::string maxStr = fmt::format("{:.2f}", max);
        std::string avgStr = fmt::format("{:.2f}", min + (max-min)/2.f);

        raylib::DrawText(minStr, x, y+innerBounds.height, fontsize, lineColor);
        raylib::DrawText(avgStr, x, y+innerBounds.height/2.f, fontsize, lineColor);
        raylib::DrawText(maxStr, x, y+fontsize, fontsize, lineColor);

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
        raylib::Color pointNormal = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
        raylib::Color pointFocused = GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED);
        raylib::Color pointPressed = GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED);

        raylib::Color pointBorderNormal = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
        raylib::Color pointBorderFocused = GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL);
        raylib::Color pointBorderPressed = GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL);
        for(int i=0; i < sortedPoint.size(); i++){
            const auto* p = sortedPoint[i];
            raylib::Vector2 screenPos = LocalToScreen(p->position);

            // Draw point
            if(selectedPoint && selectedPoint.value() == p){
                // Draw left handle
                if(i > 0){
                    raylib::Vector2 controlOffset{-1, -p->tangents.x};
                    auto target = LocalToScreen(p->position+controlOffset);
                    auto dir = target - screenPos;
                    raylib::Vector2 control = screenPos + dir.Normalize() * handleLength;
                    raylib::Rectangle controlRect{control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};
                    if(leftControlSelected){
                        screenPos.DrawLine(control, pointBorderPressed);
                        controlRect.Draw(pointPressed);
                        controlRect.DrawLines(pointBorderPressed);
                    } else if(mouse.CheckCollision(controlRect)){
                        screenPos.DrawLine(control, pointBorderFocused);
                        controlRect.Draw(pointFocused);
                        controlRect.DrawLines(pointBorderFocused);

                    }else{
                        screenPos.DrawLine(control, pointBorderNormal);
                        controlRect.Draw(pointNormal);
                        controlRect.DrawLines(pointBorderNormal);

                    }

                }
                // Draw right handle
                if(i < points.size()-1){
                    raylib::Vector2 controlOffset{1, p->tangents.y};
                    auto target = LocalToScreen(p->position+controlOffset);
                    auto dir = target - screenPos;
                    raylib::Vector2 control = screenPos + dir.Normalize() * handleLength;
                    raylib::Rectangle controlRect{control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};
                    if(rightControlSelected){
                        screenPos.DrawLine(control, pointBorderPressed);
                        controlRect.Draw(pointPressed);
                        controlRect.DrawLines(pointBorderPressed);

                    } else if(mouse.CheckCollision(controlRect)){
                        screenPos.DrawLine(control, pointBorderFocused);
                        controlRect.Draw(pointFocused);
                        controlRect.DrawLines(pointBorderFocused);

                    }else{
                        screenPos.DrawLine(control, pointBorderNormal);
                        controlRect.Draw(pointNormal);
                        controlRect.DrawLines(pointBorderNormal);

                    }
                }
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, pointPressed);
                DrawRectangleLines(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, pointBorderPressed);
            } else if(hoveredPoint && hoveredPoint.value() == p) {
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, pointFocused);
                DrawRectangleLines(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, pointBorderFocused);
            } else {
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, pointNormal);
                DrawRectangleLines(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, pointBorderNormal);
            }

//            // point hover
//            if(hoveredPoint && hoveredPoint.value() == p && (selectedPoint && selectedPoint.value() != p || !selectedPoint)){
//                DrawRectangleLines(screenPos.x-(fontsize+margin)/2.f, screenPos.y-(fontsize+margin)/2.f, fontsize+margin, fontsize+margin, pointHover);
//            }

        }

        // Draw Curve
        raylib::Color curveColor = GuiGetStyle(LABEL,  TEXT_COLOR_FOCUSED);
        if(sortedPoint.size() == 1){
            auto* p = sortedPoint[0];
            auto pos = LocalToScreen(p->position);
            DrawLine(innerBounds.x, pos.y, innerBounds.x+innerBounds.width, pos.y, curveColor);
        }else {
            for(int i=0; i < sortedPoint.size()-1; i++){
                const CurveParameterPoint* p1 = sortedPoint[i];
                const CurveParameterPoint* p2 = sortedPoint[i+1];
                auto pos1 = LocalToScreen(p1->position);
                auto pos2 = LocalToScreen(p2->position);
                // Constant on edge
                if(pos1.x > innerBounds.x && i == 0){
                    DrawLine(innerBounds.x, pos1.y, pos1.x, pos1.y, curveColor);
                }
                if(pos2.x < innerBounds.x+innerBounds.width && i == sortedPoint.size()-2){
                    DrawLine(pos2.x, pos2.y, innerBounds.x+innerBounds.width, pos2.y, curveColor);
                }
                DrawHermiteNormalized(p1->position, p2->position, p1->tangents.y, p2->tangents.x, curveColor);
            }
        }

        EndScissorMode();

        // Draw contours
        if(mouse.CheckCollision(bounds))
            bounds.DrawLines(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)));
        else
            bounds.DrawLines(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));

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
                }

                // Right control
                raylib::Vector2 offsetRight{1, p->tangents.y};
                auto dirRight = LocalToScreen(p->position+offsetRight)- screenPos;
                raylib::Vector2 controlRight = screenPos + dirRight.Normalize() * handleLength;

                if(mouse.CheckCollision({controlRight.x-handleSize/2.f, controlRight.y-handleSize/2.f, handleSize, handleSize})){
                    rightControlSelected = true;
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
                p->rightLinear = false;

                if(!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))){
                    p->tangents.x = p->tangents.y;
                    p->leftLinear = false;
                }

            } else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && leftControlSelected){
                auto* p = selectedPoint.value();
                auto dir = ScreenToLocal(mouse)-p->position;
                p->tangents.x = dir.x > -0.001f ? dir.y/(-0.001f) : dir.y/dir.x;
                p->leftLinear = false;
                if(!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))){
                    p->tangents.y = p->tangents.x;
                    p->rightLinear = false;
                }
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredPoint){
                selectedPoint = hoveredPoint;
                auto* p = selectedPoint.value();
                mouseOffest = LocalToScreen(p->position) - mouse;
            }
            else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredPoint.has_value() && mouse.CheckCollision(bounds)){
                if(points.size() > 1){
                    CurveParameterPoint* selected = hoveredPoint.value();
                    auto it = std::find_if(points.begin(), points.end(), [selected](const CurveParameterPoint& p){return &p == selected;});
                    points.erase(it);
                    Deselect();

                }
            // add
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse.CheckCollision(innerBounds)){
                Deselect();
                CurveParameterPoint p;
                p.position =  ScreenToLocal(mouse);
                points.push_back(p);
                selectedPoint = {&points[points.size()-1]};
            } else if(selectedPoint && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouse.CheckCollision(bounds) ){
                selectedPoint.value()->position = Vector2Clamp(ScreenToLocal(mouse+mouseOffest), {0,0}, {1,1});
            }
    }

    [[nodiscard]] CurveParameter GetCurveParameter() const {
        CurveParameter c{GetSortedPoints()};
        c.min = min;
        c.max = max;
        return std::move(c);
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
    raylib::Rectangle innerBounds;
    raylib::Vector2 mouseOffest{0,0};
    std::vector<CurveParameterPoint> points;
    std::optional<CurveParameterPoint*> selectedPoint;
    bool rightControlSelected = false;
    bool leftControlSelected = false;

    void DrawHermiteNormalized(raylib::Vector2 start, raylib::Vector2 end, float startTangent, float endTangent, raylib::Color curveColor){
        float scale = (end.x-start.x)/3.f;
        raylib::Vector2 offset1{scale, scale*startTangent};
        // negative endTangent => top part => need to invert value to calculate offset
        raylib::Vector2 offset2{-scale, -scale*endTangent};
        auto c1 = start + offset1;
        auto c2 = end + offset2;
        DrawLineBezierCubic(LocalToScreen(start),LocalToScreen(end), LocalToScreen(c1), LocalToScreen(c2), 1, curveColor);
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
