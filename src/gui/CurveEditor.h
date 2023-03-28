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
#include <iostream>

class CurveEditor {
public:

    CurveEditor(){

        // Initial point
        CurveParameterPoint p;
        p.position = raylib::Vector2(0.5,1);
        points.push_back(p);

        // Setup context menu
        menu.AddItem("Remove");
        menu.AddItem("Right linear", true);
        menu.AddItem("Left linear", true);
    }

    void DrawAndUpdate(raylib::Rectangle bounds, float fontsize){
        innerBounds = raylib::Rectangle{bounds.x+fontsize, bounds.y+fontsize, bounds.width-2*fontsize, bounds.height-2*fontsize};
        raylib::Vector2 mouse{GetMousePosition()};

        auto hoveredPoint = GetHoveredPoint(fontsize);


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


        // Draw Curve
        auto sortedPoint = GetSortedPoints();

        if(sortedPoint.size() == 1){
            auto p = sortedPoint[0];
            auto pos = LocalToScreen(p.position);
            DrawLine(innerBounds.x, pos.y, innerBounds.x+innerBounds.width, pos.y, BLACK);
        }else{
            for(int i=0; i < sortedPoint.size()-1; i++){
                const CurveParameterPoint& p1 = sortedPoint[i];
                const CurveParameterPoint& p2 = sortedPoint[i+1];
                auto pos1 = LocalToScreen(p1.position);
                auto pos2 = LocalToScreen(p2.position);

                // Constant on edge
                if(pos1.x > innerBounds.x && i == 0){
                    DrawLine(innerBounds.x, pos1.y, pos1.x, pos1.y, BLACK);
                }
                if(pos2.x < innerBounds.x+innerBounds.width && i == sortedPoint.size()-2){
                    DrawLine(pos2.x, pos2.y, innerBounds.x+innerBounds.width, pos2.y, BLACK);
                }

                // Linear
                if(p1.rightLinear && p2.leftLinear){
                    pos1.DrawLine(pos2, BLACK);
                }
                    // Bezier
                else {
                    float tangentLength = (pos2.x-pos1.x)/2.f;
                    raylib::Vector2 centerControl = (pos1+pos2) / 2.f;
                    raylib::Vector2 offset1{std::cos(p1.rightAngle), std::sin(p1.rightAngle)};
                    raylib::Vector2 offset2{-std::cos(p2.leftAngle), std::sin(p1.leftAngle)};
                    raylib::Vector2 control1 =  p1.rightLinear ? centerControl : pos1+offset1*tangentLength;
                    raylib::Vector2 control2 = p2.leftLinear ? centerControl : pos2+offset2*tangentLength;
                    DrawLineBezierCubic(pos1,pos2, control1, control2, 1, BLACK);
                }
            }
        }

        // Draw points
        for(auto& p : points){

            raylib::Vector2 screenPos = LocalToScreen(p.position);
            raylib::Rectangle pRect{screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize};

            // Draw point
            if(selectedPoint && selectedPoint.value() == &p){
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, BLUE);
                // TODO: Draw controls points
            } else {
                DrawRectangle(screenPos.x-fontsize/2.f, screenPos.y-fontsize/2.f, fontsize, fontsize, BLACK);
            }

            // point hover
            if(hoveredPoint && hoveredPoint.value() == &p)
                DrawRectangleLines(screenPos.x-(fontsize+margin)/2.f, screenPos.y-(fontsize+margin)/2.f, fontsize+margin, fontsize+margin, BLACK);
        }


        //Draw Menu && update choice
        if(menu.IsVisible()){
            menu.DrawAndUpdate(fontsize);
            auto item = menu.GetSelected();
            if(item){
                switch (item.value()->index) {
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
            //TODO: select control point

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredPoint.has_value())
                selectedPoint = hoveredPoint;
            else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hoveredPoint.has_value())
                selectedPoint = {};
            else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredPoint.has_value()){
                menu.position = mouse;
                menu.Show();
                selectedPoint = hoveredPoint;
                menu.GetItemAt(1)->checked = selectedPoint.value()->rightLinear;
                 menu.GetItemAt(2)->checked = selectedPoint.value()->leftLinear;
            } else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && mouse.CheckCollision(innerBounds)){
                CurveParameterPoint p;
                p.position = ScreenToLocal(mouse);
                points.push_back(p);
            } else if(selectedPoint && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                selectedPoint.value()->position = Vector2Clamp(ScreenToLocal(mouse),{0,0}, {1,1});
        }
    }

    [[nodiscard]] CurveParameter GetCurveParameter() const {
        return {GetSortedPoints()};
    }



private:
    constexpr static const float margin = 4;
    ContextMenu menu;
    raylib::Rectangle innerBounds;


    std::vector<CurveParameterPoint> points;
    std::optional<CurveParameterPoint*> selectedPoint;

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

};


#endif //TIMTIM_CURVEEDITOR_H
