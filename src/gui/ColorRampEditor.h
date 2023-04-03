//
// Created by pjaff on 02/04/2023.
//

#ifndef TIMTIM_COLORRAMPEDITOR_H
#define TIMTIM_COLORRAMPEDITOR_H

#include "raylib-cpp.hpp"
#include "../external/raygui/raygui.h"
#include "../core/Resource.h"
#include <vector>
#include <algorithm>
#include <optional>

class ColorRampEditor {
public:

    bool showColorPopup = false;

    ColorRampEditor(){
        colors.emplace_back(WHITE, 0);
        colors.emplace_back(BLACK, 1);
    }

    std::optional<std::pair<raylib::Color, float>*> GetSelectedColor(){
        return selectedColor;
    }

    ColorRamp GetColorRamp(){
        auto sortedData = GetSortedColorsPtr();
        std::vector<std::pair<raylib::Color, float>> data(sortedData.size());
        for(auto* p : sortedData){data.emplace_back(p->first, p->second);}
        return {data};
    }

    void DrawAndUpdate(raylib::Rectangle bounds, float fontSize){
        raylib::Vector2 mouse{GetMousePosition()};
        innerBounds = raylib::Rectangle{bounds.x+fontSize/2.f, bounds.y, bounds.width-bounds.height-fontSize-fontSize/2.f, bounds.height};
        raylib::Rectangle colorPreview{bounds.x+bounds.width-bounds.height, bounds.y, bounds.height, bounds.height};

        // DrawCheckers
        DrawCheckers(innerBounds, fontSize);
        // DrawCheckers
        DrawCheckers(colorPreview, fontSize);

        // Color preview rendering
        if(selectedColor){
            colorPreview.Draw(selectedColor.value()->first);
        } else {
            colorPreview.Draw(GRAY);
            DrawLine(colorPreview.x, colorPreview.y, colorPreview.x+colorPreview.width, colorPreview.y+colorPreview.height, BLACK);
            DrawLine(colorPreview.x, colorPreview.y+colorPreview.height, colorPreview.x+colorPreview.width, colorPreview.y, BLACK);
        }

        if(colorPreview.CheckCollision(mouse) && selectedColor || showColorPopup)
            colorPreview.DrawLines(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)));
        else
            colorPreview.DrawLines(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));


        auto sortedColors = GetSortedColorsPtr();

        // Draw Color gradient
        if(sortedColors.size() == 1){
            auto c = sortedColors[0]->first;
            innerBounds.Draw(c);
        } else {
            for(int i=0; i < sortedColors.size()-1; i++){
                const auto* p1 = sortedColors[i];
                const auto* p2 = sortedColors[i+1];
                auto x1 = LocalToScreen(p1->second);
                auto x2 = LocalToScreen(p2->second);
                auto c1 = p1->first;
                auto c2 = p2->first;

                // Constant on edge
                if(x1 > innerBounds.x && i == 0){
                    raylib::Rectangle rect{innerBounds.x, innerBounds.y, x1-innerBounds.x, innerBounds.height};
                    rect.Draw(c1);
                }
                if(x2 < innerBounds.x+innerBounds.width && i == sortedColors.size()-2){
                    raylib::Rectangle rect{x2, innerBounds.y, innerBounds.x+innerBounds.width-x2, innerBounds.height};
                    rect.Draw(c2);
                }
                raylib::Rectangle rect{x1, innerBounds.y, x2-x1, innerBounds.height};
                rect.DrawGradientH(c1, c2);
            }
        }

        // Draw innerBounds contours
        if(mouse.CheckCollision(innerBounds))
            innerBounds.DrawLines(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)));
        else
            innerBounds.DrawLines(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));

        // Draw color handles
        for(auto* p : sortedColors){
            float x = LocalToScreen(p->second);
            raylib::Rectangle rect{x-fontSize/2.f, innerBounds.y+innerBounds.height/2.f, fontSize, innerBounds.height/2.f};
            rect.Draw(p->first);
            raylib::Color c;
            if(selectedColor && selectedColor.value() == p)
                c = GuiGetStyle(BUTTON, BORDER_COLOR_PRESSED);
            else if(mouse.CheckCollision(rect))
                c = GuiGetStyle(BUTTON, BORDER_COLOR_FOCUSED);
            else
                c = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
            rect.DrawLines(c);
            DrawLine(x, innerBounds.y+innerBounds.height/2.f, x, innerBounds.y, c);
        }


        // Skip input
        if(showColorPopup){
            return;
        }

        auto hoveredColor = GetHoveredColor(fontSize);

        // Show color popup
        if(mouse.CheckCollision(colorPreview) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && selectedColor){
            showColorPopup = true;
            return;
        }

        // Select color
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredColor){
            selectedColor = hoveredColor;
            mouseOffset = LocalToScreen(selectedColor.value()->second)-mouse.x;
            return;
        }

        // Add color
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !hoveredColor && mouse.CheckCollision(innerBounds)){
            selectedColor = {};
            // find closest color
            float x = ScreenToLocal(mouse.x);
            std::pair<raylib::Color, float>* closest = &colors[0];
            for(auto& p : colors){
                if(std::abs(x-p.second) < std::abs(x-closest->second))
                    closest = &p;
            }
            colors.emplace_back(closest->first, x);
            selectedColor = {&colors[colors.size()-1]};
            mouseOffset = 0;
            return;
        }

        // remove color
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredColor){
            if(colors.size() == 1)
                return;
            auto* selected = hoveredColor.value();
            selectedColor = {};
            auto it = std::find_if(colors.begin(), colors.end(), [selected](const auto& p){return &p == selected;});
            colors.erase(it);
            return;
        }

        // Move handle
        if(selectedColor && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouse.CheckCollision(innerBounds)){
            selectedColor.value()->second = ScreenToLocal(mouse.x+mouseOffset);
        }

    }



private:
    raylib::Rectangle innerBounds;
    float mouseOffset;

    std::vector<std::pair<raylib::Color, float>> colors;

    std::optional<std::pair<raylib::Color, float>*> selectedColor;

    void DrawCheckers(raylib::Rectangle rect, float fontSize){
        // Draw alpha background
        BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
        int nx = std::ceil(rect.width / fontSize);
        int ny = std::ceil(rect.height / fontSize);
        for(int i=0; i < ny; i++){
            for(int j=0; j < nx; j++){
                raylib::Rectangle r{rect.x+j*fontSize, rect.y+i*fontSize, fontSize, fontSize};
                r.Draw( (i+j) % 2 == 0 ? GRAY : WHITE);
            }
        }
        EndScissorMode();
    }


    std::optional<std::pair<raylib::Color, float>*> GetHoveredColor(float fontSize){
        raylib::Vector2 mouse{GetMousePosition()};
        for(auto& p : colors){
            float x = LocalToScreen(p.second);
            raylib::Rectangle rect{x-fontSize/2.f, innerBounds.y+innerBounds.height/2.f, fontSize, innerBounds.height/2.f};
            if(rect.CheckCollision(mouse)){
                return {&p};
            }
        }
        return {};
    }

    float ScreenToLocal(float x){
        return std::clamp((x-innerBounds.x)/innerBounds.width, 0.f, 1.f);
    }
    float LocalToScreen(float x){
        return innerBounds.x + innerBounds.width*x;
    }


    std::vector<std::pair<raylib::Color, float>*> GetSortedColorsPtr(){
        std::vector<std::pair<raylib::Color, float>*> sortedPoint(colors.size());
        for(int i=0; i < colors.size(); i++) sortedPoint[i] = &colors[i];
        std::sort(sortedPoint.begin(), sortedPoint.end(), [](const std::pair<raylib::Color, float>* p1, const std::pair<raylib::Color, float>* p2){return p1->second < p2->second;});
        return std::move(sortedPoint);
    }
};


#endif //TIMTIM_COLORRAMPEDITOR_H
