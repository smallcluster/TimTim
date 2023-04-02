
#include "raylib-cpp.hpp"
#include "../src/external/raygui/raygui.h"
#include "../src/core/Scene.h"
#include "../src/render/particles.h"

#include <iostream>
#include <functional>
#include "../src/gui/CurveEditor.h"
#include "../src/gui/ColorRampEditor.h"
#include "../src/gui/ColorPopup.h"

#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24

//----------------------------------------------------------------------------------------------------------------------
// EDITOR SCENE
//----------------------------------------------------------------------------------------------------------------------
class Editor : public Scene {
public:
    Editor() {
        // Camera setup
        cam.offset = {0};
        cam.target = {0};
        cam.zoom = 2.0f;
        cam.rotation = 0;
        colorPopupState = InitGuiColorPopup();
        colorPopupState.ColorPopupActive = false;
    }
    void Update(float delta) override {
        Scene::Update(delta);
    }

    void Draw() override{
        ClearBackground(BLACK);

        // Draw Scene from camera
        cam.BeginMode();
        DrawLine(-screenSize.x/2, 0, screenSize.x/2, 0, RED);
        DrawLine(0, -screenSize.y/2,0, screenSize.y/2, GREEN);
        Scene::Draw();


        // curve func test
        auto curve = curveEdit.GetCurveParameter();
        int res = 32;
        float t = 1.f/(float) res;
        float scaleX = 3*64;
        float scaleY = 6*16;
        for(int i=0; i < res; i++){
            float x = t*(float)i;
            float y = curve.Eval(x);
            float x2 = t*(float)(i+1);
            float y2 = curve.Eval(x2);
            DrawLine(x*scaleX,-y*scaleY,x2*scaleX,-y2*scaleY, YELLOW);
        }

        cam.EndMode();

        // Draw GUI
        float x = 16;
        float y = 16;
        float dy = 8;
        float width = 128;
        float height = 32;


        if(GuiButton({x,y,width, height}, "Load texture")){
        }
        y += dy+height;

        // --- settings options ---

        // Particle count
        GuiLabel({x,y,width,height}, "Count");
        y += dy+height/2;

        if(GuiSpinner({x,y,width,height}, "", &particlesCount, 0, 100000, inputBoxFocus == 1))
            inputBoxFocus = inputBoxFocus == 1 ? 0 : 1;
        y+=dy + height;


        // Particle life time
        GuiLabel({x,y,width,height}, "Life time");
        y += dy+height/2;
        if(GuiIntInput({x,y,width,height}, particlesCount, 0, 100000, inputBoxFocus == 2))
            inputBoxFocus = inputBoxFocus == 2 ? 0 : 2;
        y += dy+height;

        GuiWindowBox({x,y,3*width,6*height}, "Scale");

        curveEdit.DrawAndUpdate({x, y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 3 * width, 6 * height-RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}, 12);
        y += dy+6*height;
        auto selectedPoint = curveEdit.GetSelectedPoint();
        if(selectedPoint){
            if(GuiFloatInput({x, y, 3*width/4, height}, selectedPoint.value()->position.x, 0, 1, inputBoxFocus == 3)){
                inputBoxFocus = inputBoxFocus == 3 ? 0 : 3;
            }
            if(GuiFloatInput({x+3*width/4, y, 3*width/4, height}, selectedPoint.value()->position.y, 0, 1, inputBoxFocus == 4)){
                inputBoxFocus = inputBoxFocus == 4 ? 0 : 4;
            }
            if(GuiFloatInput({x+3*width/2, y, 3*width/4, height}, selectedPoint.value()->tangents.x, -9999, 9999, inputBoxFocus == 5)){
                inputBoxFocus = inputBoxFocus == 5 ? 0 : 5;
            }
            if(GuiFloatInput({x+9*width/4, y, 3*width/4, height}, selectedPoint.value()->tangents.y, -9999, 9999, inputBoxFocus == 6)){
                inputBoxFocus = inputBoxFocus == 6 ? 0 : 6;
            }
            y+=dy+height;
        }

        colorRampEdit.DrawAndUpdate({x,y, 3*width, 2*height}, 12);

        // Show color popup ?
        if(colorRampEdit.showColorPopup && !colorPopupState.ColorPopupActive){
            colorPopupState.anchor01 = GetMousePosition();
            colorPopupState.ColorPickerValue = colorRampEdit.GetSelectedColor().value()->first;
            colorPopupState.ColorPopupActive = true;
        }

        // Get value from color pop up
        if(colorRampEdit.showColorPopup && colorPopupState.ColorPopupActive){
            GuiColorPopup(&colorPopupState);
            auto selectedColor = colorRampEdit.GetSelectedColor();
            selectedColor.value()->first = colorPopupState.ColorPickerValue;
        }

        // close pop up ?
        if(!colorPopupState.ColorPopupActive){
            colorRampEdit.showColorPopup = false;
        }




        // Saving & loading settings
        x = screenSize.x-16-width;
        y = 16;
        if(GuiButton({x,y,width, height}, "Load settings")){
            curveEdit.LoadCurve("./data/curve.json");
        }
        y += dy+height;
        if(GuiButton({x,y,width, height}, "Save settings")){
            curveEdit.SaveCurve("./data/curve.json");
        }
    }
    void SetScreenSize(int screenWidth, int screenHeight){
        screenSize.x = screenWidth;
        screenSize.y = screenHeight;
        cam.offset = {screenWidth / 2.f, screenHeight / 2.f};
    }
private:
    CurveEditor curveEdit;
    ColorRampEditor colorRampEdit;
    GuiColorPopupState colorPopupState;


    raylib::Camera2D cam;
    raylib::Vector2 screenSize;
    int particlesCount = 0;
    int inputBoxFocus = 0;

    bool GuiIntInput(raylib::Rectangle bounds, int& value, int min, int max, bool edit, int defaultVal = 0){
        char buffer[11] = {0};
        std::string strRep = std::to_string(value);
        strRep.copy(buffer, strRep.length());
        buffer[10] = '\0';
        bool pressed = GuiTextBox(bounds, buffer, 11, edit);
        int res;
        try {
            res = std::stoi(std::string{buffer});
        } catch(std::invalid_argument& e) {
            res = defaultVal;
        } catch (std::out_of_range& e){
            res = max;
        }
        value = std::clamp(res, min, max);
        return pressed;
    }
    bool GuiFloatInput(raylib::Rectangle bounds, float& value, float min, float max, bool edit, float defaultVal = 0){
        char buffer[256] = {0};
        std::string strRep = std::to_string(value);
        strRep.copy(buffer, strRep.length());
        buffer[255] = '\0';
        bool pressed = GuiTextBox(bounds, buffer, 256, edit);
        float res;
        try {
            res = std::stof(std::string{buffer});
        } catch(std::invalid_argument& e) {
            res = defaultVal;
        } catch (std::out_of_range& e){
            res = max;
        }
        value = std::clamp(res, min, max);
        return pressed;
    }
};


//----------------------------------------------------------------------------------------------------------------------
// START EDITOR
//----------------------------------------------------------------------------------------------------------------------
int main(){
    const int screenWidth = 1280;
    const int screenHeight = 720;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(screenWidth, screenHeight, "Particle Editor");
    Editor scene; // Default scene
    scene.SetScreenSize(screenWidth, screenHeight);
    while(!window.ShouldClose()){

        if(window.IsResized())
            scene.SetScreenSize(window.GetWidth(), window.GetHeight());

        window.BeginDrawing();
        scene.Update(window.GetFrameTime());
        scene.Draw();
        window.EndDrawing();
    }
    return 0;
}