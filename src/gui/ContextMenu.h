//
// Created by pjaff on 27/03/2023.
//

#ifndef TIMTIM_CONTEXTMENU_H
#define TIMTIM_CONTEXTMENU_H

#include "raylib-cpp.hpp"
#include <vector>
#include <optional>
#include <numeric>


class MenuItem {
public:
    bool checked = false;
    bool checkable = false;
    std::string name = "";
};

class ContextMenu {

public:
    bool show = false;
    raylib::Vector2 position;

    auto GetSelected(){
        return selectedItem;
    }

    void DrawAndUpdate(float fontSize){
        const raylib::Vector2 mouse{GetMousePosition()};
        raylib::Rectangle rect = GetMinRect(fontSize);
        // Pannel
        rect.Draw(LIGHTGRAY);
        rect.DrawLines(BLACK);
        // Items
        for(auto& m : items){
            const auto itemMinSize = GetMinItemSize(m, fontSize);
            DrawMenuItem(m, position, fontSize, rect.width);
            // item click
            const raylib::Rectangle itemRect{position, itemMinSize};
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse.CheckCollision(itemRect)){
                selectedItem = {m}; // select item
                // update selected item state
                if(m.checkable){
                    m.checked = m.checked;
                }
            }
            position.y += itemMinSize.y;
        }
        // Hide context menu
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !mouse.CheckCollision(rect) || !IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            show = false;
    }

    raylib::Vector2 GetMinSize(float fontSize) const {
        std::vector<raylib::Rectangle> rectItems(items.size());
        std::transform(items.begin(), items.end(), rectItems.begin(), [fontSize](const MenuItem& m){return GetMinItemSize(m, fontSize);});
        auto max = *std::max_element(rectItems.begin(), rectItems.end(), [](const raylib::Rectangle& r1, const raylib::Rectangle& r2){return r1.width < r2.width;});
        auto height = std::accumulate(rectItems.begin(), rectItems.end(), 0.f, [](const auto& acc, const auto& r){return acc + r.height;});
        return {max.width, height};
    }

    raylib::Rectangle GetMinRect(float fontsize){
        return {position, GetMinSize(fontsize)};
    }

    static void DrawMenuItem(const MenuItem& m, raylib::Vector2 position, float fontSize, float widthOverride = 0){
        raylib::Vector2 mouse{GetMousePosition()};
        auto minSize = GetMinItemSize(m, fontSize);
        if(widthOverride > 0)
            minSize.x = widthOverride;
        raylib::Rectangle rect{position, minSize};
        raylib::Color accentColor = mouse.CheckCollision(rect) ? BLUE : BLACK;

        rect.DrawLines(accentColor);
        float x = margin;
        float y = margin;
        if(m.checkable){
            DrawRectangle(x,y, fontSize, fontSize, m.checked ? BLUE : GRAY);
            x += fontSize + margin;
        }
        raylib::DrawText(m.name, x, y, fontSize, accentColor);
    }

    static raylib::Vector2 GetMinItemSize(const MenuItem& m, float fontSize){
        float textWidth = raylib::MeasureText(m.name, fontSize);
        float width = margin  + textWidth + margin;
        float height = margin + fontSize  + margin;
        if(m.checkable){
            width += fontSize + margin;
        }
        return {width, height};
    }

private:
    constexpr static const float margin = 4;
    std::vector<MenuItem> items;
    std::optional<MenuItem> selectedItem;
};


#endif //TIMTIM_CONTEXTMENU_H
