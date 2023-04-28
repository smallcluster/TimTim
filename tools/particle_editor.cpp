#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24

#include "raylib-cpp.hpp"
#include "../src/external/raygui/raygui.h"
#include "../src/core/Scene.h"
#include "../src/gui/gui.h"
#include "../src/gui/CurveEditor.h"
#include "../src/gui/ColorRampEditor.h"
#include <iostream>
#include <functional>
#include <optional>




//----------------------------------------------------------------------------------------------------------------------
// EDITOR SCENE
//----------------------------------------------------------------------------------------------------------------------
class Editor : public Scene {
public:
    Editor() {
        GuiLoadStyleDefault();

        // Camera setup
        cam.offset = {0};
        cam.target = {0};
        cam.zoom = 2.0f;
        cam.rotation = 0;

        // widget setup
        categories.emplace_back("Time", false);
        categories.emplace_back("Drawing", false);
        categories.emplace_back("Shape", false);
        categories.emplace_back("Particles Flags", false);
        categories.emplace_back("Emission Direction", false);
        categories.emplace_back("Gravity", false);
        categories.emplace_back("Velocity", false);
        categories.emplace_back("Angular Velocity", false);
        categories.emplace_back("Damping", false);
        categories.emplace_back("Angle", false);
        categories.emplace_back("Scale", false);
        categories.emplace_back("Color", false);
        themePaths.emplace_back("");
        themeList = "default";

        auto themeMap = gui::FindThemes("./data/Engine/GuiStyles/");
        for(auto& p : themeMap){
            themePaths.push_back(p.second);
            themeList += ";"+p.first;
        }

        GuiLoadStyleDefault();
        GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    }
    void Update(float delta) override {
        Scene::Update(delta);
    }

    void Draw() override{
        raylib::Color bgColor  = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
        raylib::Color shadowColor(0, 0, 0, 50);


        raylib::Vector2 mouse{GetMousePosition()};
        static float mainPanelSplitPos = 0.75f;
        static float settingsContentHeight = 0.f;
        static raylib::Vector2 settingsScroll{0,0};

        // consts
        const float scrollBarWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
        const float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
        const float margin = fontSize/2.f;
        const float mainSplitWidth = margin/2.f;
        const float checkBoxTextPadding = GuiGetStyle(CHECKBOX, TEXT_PADDING);
        const float valueBoxTextPadding = GuiGetStyle(VALUEBOX, TEXT_PADDING);
        const float menuBarHeight = fontSize;
        const float slx = screenSize.x*mainPanelSplitPos+mainSplitWidth/2.f+margin;
        const float evx = screenSize.x - margin - scrollBarWidth;
        const float elx = slx + (evx-slx) /2.f - margin;
        const float svx = slx + (evx-slx) /2.f + margin;

        ClearBackground(bgColor);


        //SCENE
        //--------------------------------------------------------------------------------------------------------------
        cam.BeginMode();
        DrawLine(-screenSize.x/2, 0, screenSize.x/2, 0, RED);
        DrawLine(0, -screenSize.y/2,0, screenSize.y/2, GREEN);
        Scene::Draw();
        cam.EndMode();

        // GUI
        //--------------------------------------------------------------------------------------------------------------

        // Setting window
        static bool showSetting = true;

        // Update camera offset
        if(showSetting){
            cam.offset.x = (screenSize.x*mainPanelSplitPos-mainSplitWidth/2.f)/2.f;
        } else {
            cam.offset.x = screenSize.x/2.f;
        }

        float sx = screenSize.x*mainPanelSplitPos+mainSplitWidth/2.f;
        float sy = 0;

        // Menu bar
        GuiStatusBar({0, 0, screenSize.x, menuBarHeight}, NULL);
        float menuDx = 0;

        // show settings
        const char* fileTxt = GuiIconText(showSetting ? ICON_EYE_ON : ICON_EYE_OFF, "Settings");
        int dtxt = MeasureText(fileTxt, fontSize);
        if(GuiButton({menuDx, sy, (float)dtxt, fontSize}, fileTxt)){
            showSetting = !showSetting;
        }
        menuDx += dtxt+margin;

        // switch theme color
        static int themeIndex = 0;
        int choice = GuiComboBox({menuDx, sy, 128, fontSize}, themeList.c_str(), themeIndex);
        if(choice != themeIndex){
            themeIndex = choice;
            if(themeIndex == 0){
                GuiLoadStyleDefault();
            } else {
                GuiLoadStyle(themePaths[themeIndex].c_str());
            }
            GuiSetStyle(DEFAULT, TEXT_SIZE, std::max(18, GuiGetStyle(DEFAULT, TEXT_SIZE)));
        }
        menuDx += 128+margin;


        sy += menuBarHeight;
        DrawRectangleGradientV(0,sy, screenSize.x, fontSize, shadowColor, BLANK);


        if(showSetting){

            DrawRectangleGradientH(sx-fontSize,sy, fontSize, screenSize.y-menuBarHeight, BLANK, shadowColor);

            raylib::Rectangle mainPanel{sx, sy, screenSize.x-sx, screenSize.y-menuBarHeight};
            showSetting = !GuiWindowBox(mainPanel, "Settings");
            sy += RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT+margin;

            BeginScissorMode(sx, sy, screenSize.x-sx, screenSize.y);

            GuiScrollPanel({sx, sy-margin, screenSize.x-sx, screenSize.y}, NULL, {sx, sy, screenSize.x-sx, settingsContentHeight}, &settingsScroll);

            // Default settings
            // emitting
            static bool emitting = false;
            GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+checkBoxTextPadding}, "Emitting");
            emitting = GuiCheckBox({svx, sy+settingsScroll.y, fontSize + checkBoxTextPadding, fontSize + checkBoxTextPadding}, NULL, emitting);
            sy += fontSize + margin + checkBoxTextPadding;

            // amount
            static int amount = 0;
            GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+valueBoxTextPadding}, "Amount");
            static gui::ValueBox amountBox;
            amountBox.bounds = (Rectangle){svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding};
            amountBox.Draw(&amount, 0, 10000);
            sy += fontSize+margin+valueBoxTextPadding;

            // Draw category
            for(int i=0; i < categories.size(); i++){
                auto& p = categories[i];
                gui::CollapsingHeader h;
                h.bounds = (Rectangle) {slx, sy+settingsScroll.y, evx-slx, fontSize};
                h.active = p.second;
                h.Draw(p.first);
                p.second = h.active;

                sy += fontSize+margin;

                // Not active
                if(!p.second)
                    continue;

                // Draw group
                switch (i) {

                    // Time group
                    case 0:{

                        // LifeTime
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+valueBoxTextPadding}, "Life time");
                        static gui::ValueSliderCombo lifeTimeVb;
                        lifeTimeVb.bounds = (Rectangle){svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        lifeTimeVb.sliderHeight = fontSize/2.f;
                        static int lifeTime = 1;
                        lifeTimeVb.Draw(&lifeTime, 0, 999);
                        sy += lifeTimeVb.bounds.height+margin;

                        // SpeedScale
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+valueBoxTextPadding}, "Speed scale");
                        static gui::ValueSliderCombo speedScaleVb;
                        speedScaleVb.bounds = (Rectangle){svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        speedScaleVb.sliderHeight = fontSize/2.f;
                        static int speedScale = 1;
                        speedScaleVb.Draw(&speedScale, 0, 999);
                        sy += speedScaleVb.bounds.height+margin;

                        // explosiveness
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+valueBoxTextPadding}, "Explosiveness");
                        static gui::ValueSliderCombo explosivenessVb;
                        explosivenessVb.bounds = (Rectangle){svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        explosivenessVb.sliderHeight = fontSize/2.f;
                        static int explosiveness = 1;
                        explosivenessVb.Draw(&explosiveness, 0, 999);
                        sy += explosivenessVb.bounds.height+margin;

                        // life time randomness
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+valueBoxTextPadding}, "Life time rand");
                        static gui::ValueSliderCombo lifeTimeRandVb;
                        lifeTimeRandVb.bounds = (Rectangle){svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        lifeTimeRandVb.sliderHeight = fontSize/2.f;
                        static int lifeTimeRand = 1;
                        lifeTimeRandVb.Draw(&lifeTimeRand, 0, 999);
                        sy += lifeTimeRandVb.bounds.height+margin;

                        // One shot
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+checkBoxTextPadding}, "One shot");
                        static bool oneShot = false;
                        oneShot = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, NULL, oneShot);
                        sy += fontSize+checkBoxTextPadding+margin;

                        break;
                    }

                    case 1: {
                        // local coords
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+checkBoxTextPadding}, "Local coords");
                        static bool localCoords = false;
                        localCoords = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, NULL, localCoords);
                        sy += fontSize+checkBoxTextPadding+margin;

                        // Texture
                        static std::optional<std::string> texturePath;
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Texture");
                        if(texturePath){
                            GuiLabel({svx+fontSize+margin, sy+settingsScroll.y, fontSize, fontSize}, texturePath.value().c_str());
                            if(GuiButton({svx, sy+settingsScroll.y, fontSize, fontSize}, GuiIconText(ICON_BIN, ""))){
                                texturePath = {};
                            }
                        } else {
                            if(GuiButton({svx, sy+settingsScroll.y, evx-svx, fontSize}, "Load")){
                                texturePath = {"dummy.png"};
                            }
                        }
                        sy += fontSize+margin;

                        break;
                    }

                    case 2:{
                        // Shape
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Shape");
                        static int shapeId = 0;
                        shapeId = GuiComboBox({svx, sy+settingsScroll.y, evx-svx, fontSize}, "Point;Rectangle;Disc;Circle", shapeId);
                        sy += fontSize+margin;
                        break;
                    }

                    case 3: {
                        // align y
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize+checkBoxTextPadding}, "Align Y");
                        static bool alignY = false;
                        alignY = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, NULL, alignY);
                        sy += fontSize+checkBoxTextPadding+margin;
                        break;
                    }

                    case 4:{

                        // Direction
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "direction");
                        static int dirx = 0;
                        static gui::ValueBox dirxVb;
                        dirxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                        dirxVb.Draw(&dirx, 0, 1);
                        static int diry = 0;
                        static gui::ValueBox diryVb;
                        diryVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                        diryVb.Draw(&diry, 0, 1);
                        sy += diryVb.bounds.height + margin;

                        // Spread
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Spread");
                        static int spread = 0;
                        static gui::ValueSliderCombo spreadVB;
                        spreadVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        spreadVB.sliderHeight = fontSize/2.f;
                        spreadVB.Draw(&spread, 0, 180);
                        sy += spreadVB.bounds.height+margin;

                        break;
                    }

                    case 5:{
                        // Gravity
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Gravity");
                        static int gravx = 0;
                        static gui::ValueBox gravxVb;
                        gravxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                        gravxVb.Draw(&gravx, 0, 1);
                        static int gravy = 0;
                        static gui::ValueBox gravyVb;
                        gravyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                        gravyVb.Draw(&gravy, 0, 1);
                        sy += gravyVb.bounds.height + margin;

                        break;
                    }

                    case 6:{
                        // velocity min
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial min");
                        static int velMin = 0;
                        static gui::ValueSliderCombo velMinVB;
                        velMinVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        velMinVB.sliderHeight = fontSize/2.f;
                        velMinVB.Draw(&velMin, 0, 180);
                        sy += velMinVB.bounds.height+margin;
                        // velocity max
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial max");
                        static int velMax = 0;
                        static gui::ValueSliderCombo velMaxVB;
                        velMaxVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        velMaxVB.sliderHeight = fontSize/2.f;
                        velMaxVB.Draw(&velMax, 0, 180);
                        sy += velMaxVB.bounds.height+margin;
                        break;
                    }

                    case 7: {
                        // angular velocity min
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial min");
                        static int velAngMin = 0;
                        static gui::ValueSliderCombo velAngMinVB;
                        velAngMinVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        velAngMinVB.sliderHeight = fontSize/2.f;
                        velAngMinVB.Draw(&velAngMin, 0, 180);
                        sy += velAngMinVB.bounds.height+margin;
                        // velocity max
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial max");
                        static int velAngMax = 0;
                        static gui::ValueSliderCombo velAngMaxVB;
                        velAngMaxVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        velAngMaxVB.sliderHeight = fontSize/2.f;
                        velAngMaxVB.Draw(&velAngMax, 0, 180);
                        sy += velAngMaxVB.bounds.height+margin;

                        // Angular curve parametter
                        static std::optional<CurveEditor> curve;
                        GuiLine({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Rate of change");
                        sy += fontSize+margin;
                        if(curve){
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, GuiIconText(ICON_BIN, "")))
                                curve = {};
                            sy += fontSize+margin;
                        }
                        if(curve){
                            curve->DrawAndUpdate({slx, sy+settingsScroll.y, evx-slx, 128}, 12);
                            sy += 128 + margin;

                            auto selectedPoint = curve->GetSelectedPoint();

                            if(selectedPoint){
                                // Selected position
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Position");
                                static int curvePosx = 0;
                                static gui::ValueBox curvePosxVb;
                                curvePosxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosxVb.Draw(&curvePosx, 0, 1);
                                static int curvePosy = 0;
                                static gui::ValueBox curvePosyVb;
                                curvePosyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosyVb.Draw(&curvePosy, 0, 1);
                                sy += curvePosyVb.bounds.height + margin;

                                // Selected tangents
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Tangents");
                                static int curveTanx = 0;
                                static gui::ValueBox curveTanxVb;
                                curveTanxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanxVb.Draw(&curveTanx, 0, 1);
                                static int curveTany = 0;
                                static gui::ValueBox curveTanyVb;
                                curveTanyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanyVb.Draw(&curveTany, 0, 1);
                                sy += curveTanyVb.bounds.height + margin;

                                // left linear
                                static bool angCurveLeftLinear = false;
                                angCurveLeftLinear = GuiCheckBox({slx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Left linear", angCurveLeftLinear);
                                static bool angCurveRightLinear = false;
                                angCurveRightLinear = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Right linear", angCurveRightLinear);
                                sy += fontSize + checkBoxTextPadding+margin;
                            }

                        } else {
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Use curve")){
                                CurveEditor c;
                                curve = {c};
                            }
                            sy += fontSize + margin;
                        }

                        break;
                    }

                    case 8:{

                        // damping
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Damping");
                        static int dampingx = 0;
                        static gui::ValueBox dampingxVb;
                        dampingxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                        dampingxVb.Draw(&dampingx, 0, 1);
                        static int dampingy = 0;
                        static gui::ValueBox dampingyVb;
                        dampingyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                        dampingyVb.Draw(&dampingy, 0, 1);
                        sy += dampingyVb.bounds.height + margin;

                        // Curve parametter
                        static std::optional<CurveEditor> curve;
                        GuiLine({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Rate of change");
                        sy += fontSize+margin;
                        if(curve){
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, GuiIconText(ICON_BIN, "")))
                                curve = {};
                            sy += fontSize+margin;
                        }
                        if(curve){
                            curve->DrawAndUpdate({slx, sy+settingsScroll.y, evx-slx, 128}, 12);
                            sy += 128 + margin;

                            auto selectedPoint = curve->GetSelectedPoint();

                            if(selectedPoint){
                                // Selected position
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Position");
                                static int curvePosx = 0;
                                static gui::ValueBox curvePosxVb;
                                curvePosxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosxVb.Draw(&curvePosx, 0, 1);
                                static int curvePosy = 0;
                                static gui::ValueBox curvePosyVb;
                                curvePosyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosyVb.Draw(&curvePosy, 0, 1);
                                sy += curvePosyVb.bounds.height + margin;

                                // Selected tangents
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Tangents");
                                static int curveTanx = 0;
                                static gui::ValueBox curveTanxVb;
                                curveTanxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanxVb.Draw(&curveTanx, 0, 1);
                                static int curveTany = 0;
                                static gui::ValueBox curveTanyVb;
                                curveTanyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanyVb.Draw(&curveTany, 0, 1);
                                sy += curveTanyVb.bounds.height + margin;

                                // left linear
                                static bool angCurveLeftLinear = false;
                                angCurveLeftLinear = GuiCheckBox({slx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Left linear", angCurveLeftLinear);
                                static bool angCurveRightLinear = false;
                                angCurveRightLinear = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Right linear", angCurveRightLinear);
                                sy += fontSize + checkBoxTextPadding+margin;
                            }
                        } else {
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Use curve")){
                                CurveEditor c;
                                curve = {c};
                            }
                            sy += fontSize + margin;
                        }
                        break;
                    }

                    case 9:{
                        // ang min
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial min");
                        static int AngMin = 0;
                        static gui::ValueSliderCombo angMinVB;
                        angMinVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        angMinVB.sliderHeight = fontSize/2.f;
                        angMinVB.Draw(&AngMin, 0, 180);
                        sy += angMinVB.bounds.height+margin;
                        // ang max
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial max");
                        static int aAngMax = 0;
                        static gui::ValueSliderCombo angMaxVB;
                        angMaxVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        angMaxVB.sliderHeight = fontSize/2.f;
                        angMaxVB.Draw(&aAngMax, 0, 180);
                        sy += angMaxVB.bounds.height+margin;

                        // Angular curve parametter
                        static std::optional<CurveEditor> curve;
                        GuiLine({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Rate of change");
                        sy += fontSize+margin;
                        if(curve){
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, GuiIconText(ICON_BIN, "")))
                                curve = {};
                            sy += fontSize+margin;
                        }
                        if(curve){
                            curve->DrawAndUpdate({slx, sy+settingsScroll.y, evx-slx, 128}, 12);
                            sy += 128 + margin;

                            auto selectedPoint = curve->GetSelectedPoint();

                            if(selectedPoint){
                                // Selected position
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Position");
                                static int curvePosx = 0;
                                static gui::ValueBox curvePosxVb;
                                curvePosxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosxVb.Draw(&curvePosx, 0, 1);
                                static int curvePosy = 0;
                                static gui::ValueBox curvePosyVb;
                                curvePosyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosyVb.Draw(&curvePosy, 0, 1);
                                sy += curvePosyVb.bounds.height + margin;

                                // Selected tangents
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Tangents");
                                static int curveTanx = 0;
                                static gui::ValueBox curveTanxVb;
                                curveTanxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanxVb.Draw(&curveTanx, 0, 1);
                                static int curveTany = 0;
                                static gui::ValueBox curveTanyVb;
                                curveTanyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanyVb.Draw(&curveTany, 0, 1);
                                sy += curveTanyVb.bounds.height + margin;

                                // left linear
                                static bool angCurveLeftLinear = false;
                                angCurveLeftLinear = GuiCheckBox({slx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Left linear", angCurveLeftLinear);
                                static bool angCurveRightLinear = false;
                                angCurveRightLinear = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Right linear", angCurveRightLinear);
                                sy += fontSize + checkBoxTextPadding+margin;
                            }
                        } else {
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Use curve")){
                                CurveEditor c;
                                curve = {c};
                            }
                            sy += fontSize + margin;
                        }
                        break;
                    }
                    case 10:{
                        // scale min
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial min");
                        static int scaleMin = 0;
                        static gui::ValueSliderCombo scaleMinVB;
                        scaleMinVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        scaleMinVB.sliderHeight = fontSize/2.f;
                        scaleMinVB.Draw(&scaleMin, 0, 180);
                        sy += scaleMinVB.bounds.height+margin;
                        // scale max
                        GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Initial max");
                        static int scaleMax = 0;
                        static gui::ValueSliderCombo scaleMaxVB;
                        scaleMaxVB.bounds = (Rectangle) {svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding+fontSize/2.f};
                        scaleMaxVB.sliderHeight = fontSize/2.f;
                        scaleMaxVB.Draw(&scaleMax, 0, 180);
                        sy += scaleMaxVB.bounds.height+margin;

                        // Angular curve parametter
                        static std::optional<CurveEditor> curve;
                        GuiLine({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Rate of change");
                        sy += fontSize+margin;
                        if(curve){
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, GuiIconText(ICON_BIN, "")))
                                curve = {};
                            sy += fontSize+margin;
                        }
                        if(curve){
                            curve->DrawAndUpdate({slx, sy+settingsScroll.y, evx-slx, 128}, 12);
                            sy += 128 + margin;

                            auto selectedPoint = curve->GetSelectedPoint();

                            if(selectedPoint){
                                // Selected position
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Position");
                                static int curvePosx = 0;
                                static gui::ValueBox curvePosxVb;
                                curvePosxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosxVb.Draw(&curvePosx, 0, 1);
                                static int curvePosy = 0;
                                static gui::ValueBox curvePosyVb;
                                curvePosyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curvePosyVb.Draw(&curvePosy, 0, 1);
                                sy += curvePosyVb.bounds.height + margin;

                                // Selected tangents
                                GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Tangents");
                                static int curveTanx = 0;
                                static gui::ValueBox curveTanxVb;
                                curveTanxVb.bounds = (Rectangle) {svx, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanxVb.Draw(&curveTanx, 0, 1);
                                static int curveTany = 0;
                                static gui::ValueBox curveTanyVb;
                                curveTanyVb.bounds = (Rectangle) {svx+(evx-svx+margin)/2.f, sy+settingsScroll.y, (evx-svx-margin)/2.f, fontSize+valueBoxTextPadding};
                                curveTanyVb.Draw(&curveTany, 0, 1);
                                sy += curveTanyVb.bounds.height + margin;

                                // left linear
                                static bool angCurveLeftLinear = false;
                                angCurveLeftLinear = GuiCheckBox({slx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Left linear", angCurveLeftLinear);
                                static bool angCurveRightLinear = false;
                                angCurveRightLinear = GuiCheckBox({svx, sy+settingsScroll.y, fontSize+checkBoxTextPadding, fontSize+checkBoxTextPadding}, "Right linear", angCurveRightLinear);
                                sy += fontSize + checkBoxTextPadding+margin;
                            }

                        } else {
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Use curve")){
                                CurveEditor c;
                                curve = {c};
                            }
                            sy += fontSize + margin;
                        }


                        break;
                    }

                    case 11 : {
                        static std::optional<ColorRampEditor> colorRamp;
                        static std::optional<ColorRampEditor> initialColorRamp;

                        if(!colorRamp){
                            static raylib::Color basecolor{255,0,0, 200};
                            GuiLabel({slx, sy+settingsScroll.y, elx-slx, fontSize}, "Color");
                            gui::ColorButton b;
                            b.bounds = (Rectangle){svx, sy+settingsScroll.y, evx-svx, fontSize+valueBoxTextPadding};

                            if(b.Draw(basecolor)){
                                // Display popup color
                            }
                            sy += fontSize + valueBoxTextPadding + margin;
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, "Use gradient")){
                                ColorRampEditor c;
                                colorRamp = {c};
                            }
                            sy += fontSize + margin;
                        } else {
                            if(GuiButton({slx, sy+settingsScroll.y, evx-slx, fontSize}, GuiIconText(ICON_BIN, "")))
                                colorRamp = {};
                            sy += fontSize+margin;
                            if(colorRamp){
                                colorRamp.value().DrawAndUpdate({slx, sy+settingsScroll.y, evx-slx, 48}, 12);
                                sy += 48 + margin;
                            }
                        }
                        break;
                    }

                    // skip for now
                    default:{
                        continue;
                    }
                }

            }

            EndScissorMode();

            settingsContentHeight = sy;

            // Split control
            raylib::Rectangle splitControl{screenSize.x*mainPanelSplitPos-mainSplitWidth/2.f, menuBarHeight, mainSplitWidth, screenSize.y};

            // Split plane movement
            static bool moveSplit = false;

            if(GuiGetState() == STATE_DISABLED){
                raylib::Color c = GuiGetStyle(DEFAULT, BASE_COLOR_DISABLED);
                splitControl.Draw(c);
                return;
            }else if(moveSplit){
                raylib::Color c = GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED);
                splitControl.Draw(c);
            } else if(mouse.CheckCollision(splitControl)){
                raylib::Color c = GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED);
                splitControl.Draw(c);
            }

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouse.CheckCollision(splitControl)){
                moveSplit = true;
            }
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && moveSplit){
                moveSplit = false;
            }
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && moveSplit){
                mainPanelSplitPos = std::clamp(mouse.x, 0.f, screenSize.x-300) / screenSize.x;
            }
        }

    }
    void SetScreenSize(int screenWidth, int screenHeight){
        screenSize.x = screenWidth;
        screenSize.y = screenHeight;
        cam.offset = {screenWidth / 2.f, screenHeight / 2.f};

    }
private:
    raylib::Camera2D cam;
    raylib::Vector2 screenSize;

    // GUI STATES
    std::vector<std::string> themePaths;
    std::string themeList;
    std::vector<std::pair<std::string, bool>> categories;

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