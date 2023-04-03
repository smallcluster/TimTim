//
// Created by pjaff on 03/04/2023.
//

#ifndef TIMTIM_GUI_H
#define TIMTIM_GUI_H

#include "raylib-cpp.hpp"
#include "../external/raygui/raygui.h"
#include "fmt/printf.h"
#include <vector>
#include <map>
#include <filesystem>

namespace gui{

    std::map<std::string, std::string> FindThemes(const std::string& themeDir){
        std::map<std::string, std::string> themes;
        for(const auto& entry : std::filesystem::directory_iterator(themeDir)){
            if(entry.path().extension() == ".rgs"){
                themes[entry.path().stem().string()] = entry.path().string();
            }
        }
        return themes;
    }


    class ValueBox{
    public:
        raylib::Rectangle bounds;
        void Draw(int* value, int min, int max){
            if(!edit){
                internalValue = *value;
            }
            if(GuiValueBox(bounds, NULL, &internalValue, min, max, edit)){
                edit = !edit;
                if(!edit){
                    internalValue = std::clamp(internalValue, min, max);
                    *value = internalValue;
                }
            }
        }
    private:
        bool edit = false;
        int internalValue = 0;
    };

    class Slider{
    public:
        raylib::Rectangle bounds;
        void Draw(float* value, float min, float max){
            *value = GuiSlider(bounds, NULL, NULL, *value, min, max);
        }
    };
    class CollapsingHeader{
    public:
        raylib::Rectangle bounds;
        bool active = false;
        void Draw(const std::string& txt){
            GuiStatusBar(bounds, NULL);
            if(GuiLabelButton(bounds, GuiIconText(active ? ICON_ARROW_DOWN_FILL : ICON_ARROW_RIGHT_FILL, txt.c_str())))
                active = !active;
        }
    };

    class ValueSliderCombo{
    public:
        raylib::Rectangle bounds;
        float sliderHeight = 10;

        void Draw(int* value, int min, int max){
            vb.bounds = bounds;
            vb.bounds.height -= sliderHeight;
            s.bounds = bounds;
            s.bounds.height = sliderHeight;
            s.bounds.y += vb.bounds.height;
            vb.Draw(value, min, max);
            float v = *value;
            s.Draw(&v, min, max);
            *value = v;
        }
    private:
        gui::ValueBox vb;
        gui::Slider s;
    };

    class ColorButton{
        public:
        raylib::Rectangle bounds;
        bool Draw(raylib::Color color){
            int fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
            raylib::Color linecolor = GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL);
            if(RMouse::GetPosition().CheckCollision(bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                linecolor = GuiGetStyle(DEFAULT, BORDER_COLOR_PRESSED);
            else if(RMouse::GetPosition().CheckCollision(bounds))
                linecolor = GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED);

            if(GuiGetState() == STATE_DISABLED)
                linecolor = GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED);
            // draw checkers
            BeginScissorMode(bounds.x, bounds.y, bounds.width-bounds.height-fontSize/2.f, bounds.height);
            float size = bounds.height / 3.f;
            int nx = std::ceil((bounds.width-bounds.height-(float)fontSize/2.f) / size);
            int ny = std::ceil(bounds.height / size);
            for(int i=0; i < ny; i++){
                for(int j=0; j < nx; j++){
                    raylib::Rectangle r{bounds.x+j*size, bounds.y+i*size, size, size};
                    r.Draw( (i+j) % 2 == 0 ? GRAY : WHITE);
                }
            }
            EndScissorMode();
            DrawRectangle(bounds.x, bounds.y, bounds.width-bounds.height-fontSize/2.f, bounds.height, color);
            GuiButton({bounds.x+bounds.width-bounds.height, bounds.y, bounds.height, bounds.height},
                      GuiIconText(ICON_COLOR_PICKER,""));
            DrawRectangleLines(bounds.x, bounds.y, bounds.width-bounds.height-fontSize/2.f, bounds.height, linecolor);
            if(RMouse::GetPosition().CheckCollision(bounds) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !GuiIsLocked() && GuiGetState() != STATE_DISABLED){
                return true;
            }
            return false;
        }
    };
}




#endif //TIMTIM_GUI_H
