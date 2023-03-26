
#include "raylib-cpp.hpp"
#include "../src/external/raygui/raygui.h"
#include "../src/core/Scene.h"
#include "../src/render/particles.h"

#include <iostream>
#include <functional>

struct CurvePoint {
    raylib::Vector2 position{0,0};
    float leftAngle{0};
    float rightAngle{0};

    bool leftLinear{false};
    bool rightLinear{false};
};

float MapValue(float value, float inMin, float inMax, float outMin, float outMax){
    return outMin + ((outMax - outMin) / (inMax - inMin)) * (value - inMin);
}

class CurveEditor {
public:

    CurveEditor(){
        CurvePoint p;
        points.push_back(p);
    }

    std::function<float(float)> GetFunc(){
        // Sort points on x axis
        auto sortedPoints = GetSortedPoints(true);

        return [sortedPoints=std::move(sortedPoints)](float t){
            float val = 0;
            for(int i=0; i < sortedPoints.size()-1; i++){
                const CurvePoint& p = sortedPoints[i];
                const CurvePoint& p2 = sortedPoints[i+1];
                if(p.position.x == p2.position.x)
                    continue;
                float tangentLength = (p2.position.x-p.position.x)/2.f;
                raylib::Vector2 controlOffset1{std::cos(p.rightAngle), std::sin(p.rightAngle)};
                raylib::Vector2 controlOffset2{-std::cos(p2.leftAngle), std::sin(p2.leftAngle)};
                raylib::Vector2 control1 = p.position+controlOffset1*tangentLength;
                raylib::Vector2 control2 = p2.position+controlOffset2*tangentLength;

                float relativeT = (t-p.position.x)/(p2.position.x-p.position.x);
                float y = p.position.y*(1-relativeT)*(1-relativeT)*(1-relativeT)+control1.y*3*relativeT*(1-relativeT)*(1-relativeT)+control2.y*3*relativeT*relativeT*(1-relativeT)+p2.position.y*relativeT*relativeT*relativeT;

                if(p.position.x == 0 || i == 0)
                    val += (float) (t <= p2.position.x && t >= p.position.x)*y;
                else
                    val += (float) (t <= p2.position.x && t > p.position.x)*y;
            }
            return val;
        };
    }

    void Draw(raylib::Rectangle bounds){
        updateMouse();
        updateBounds(bounds);
        hoveredPointIndex = GetHoveredIndex();

        if(CanRemoveHovered()){
            points.erase(points.begin()+hoveredPointIndex.value());
            hoveredPointIndex = {};
        } else if(CanAddPoint()){
            CurvePoint p;
            p.position = ScreenToLocal(mouse);
            points.push_back(p);
        }

        if(CanSelectHovered()){
            selectedPointIndex = hoveredPointIndex;
        } else if(CanDeselect()){
            selectedPointIndex = {};
        }

        if(selectedPointIndex.has_value() && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            auto& p = points[selectedPointIndex.value()];
            p.position = ScreenToLocal(mouse);
        }

        DrawGrid();
        DrawCurve();
        DrawPoints();
    }


private:
    float fontSize = 12;
    float handleLength = 2.5*fontSize;
    float hoverSize = fontSize + 4;
    raylib::Color lineColor = raylib::GRAY;

    raylib::Vector2 mouse;
    raylib::Vector2 previousMouse;
    std::vector<CurvePoint> points;
    std::optional<int> selectedPointIndex;
    std::optional<int> hoveredPointIndex;
    raylib::Rectangle innerBounds;
    raylib::Rectangle bounds;


    raylib::Vector2 LocalToScreen(const raylib::Vector2& p){
        raylib::Vector2 res{innerBounds.x+innerBounds.width*p.x, innerBounds.y+innerBounds.height*(1.f-p.y)};
        return res;
    }
    raylib::Vector2 ScreenToLocal(const raylib::Vector2& p){
        float x = std::clamp(p.x, innerBounds.x, innerBounds.x+innerBounds.width);
        float y = std::clamp(p.y, innerBounds.y, innerBounds.y+innerBounds.height);
        raylib::Vector2 res{(x-innerBounds.x) / innerBounds.width, (innerBounds.y+innerBounds.height-y) / innerBounds.height};
        return res;
    }

    bool CanRemoveHovered(){
        return hoveredPointIndex.has_value() && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && mouse.CheckCollision(bounds) && points.size() > 1;
    }
    bool CanSelectHovered(){
        return hoveredPointIndex.has_value() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse.CheckCollision(bounds);
    }
    bool CanDeselect(){
        return selectedPointIndex.has_value() && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !mouse.CheckCollision(
                GetPointRect(points[selectedPointIndex.value()])) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT));
    }
    bool CanAddPoint(){
        return !hoveredPointIndex.has_value() && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && mouse.CheckCollision(bounds);
    }

    std::optional<int> GetHoveredIndex(){
        for(int i=0; i < points.size(); i++){
            auto& p = points[i];
            if(mouse.CheckCollision(GetPointRect(p)))
                return {i};
        }
        return {};
    }

    raylib::Rectangle GetPointRect(CurvePoint p){
        auto sp = LocalToScreen(p.position);
        return {sp.x-fontSize/2.f, sp.y-fontSize/2.f, fontSize, fontSize};
    }


    void updateBounds(raylib::Rectangle bounds) {
        this->bounds = bounds;
        innerBounds.x = bounds.x+fontSize;
        innerBounds.y = bounds.y + fontSize;
        innerBounds.width = bounds.width-2*fontSize;
        innerBounds.height = bounds.height-2*fontSize;
    }
    void updateMouse(){
        previousMouse = mouse;
        mouse.x = GetMouseX();
        mouse.y = GetMouseY();
    }
    void DrawGrid(){
        bounds.Draw(WHITE); // BG
        LocalToScreen({0,0}).DrawLine(LocalToScreen({1,0}), lineColor);
        LocalToScreen({0,1}).DrawLine(LocalToScreen({1,1}), lineColor);
        LocalToScreen({0,0.5}).DrawLine(LocalToScreen({1,0.5}), lineColor);
        LocalToScreen({0,0}).DrawLine(LocalToScreen({0,1}), lineColor);
        LocalToScreen({1,0}).DrawLine(LocalToScreen({1,1}), lineColor);
        LocalToScreen({0.5,0}).DrawLine(LocalToScreen({0.5,1}), lineColor);
        LocalToScreen({0.75,0}).DrawLine(LocalToScreen({0.75,1}), lineColor);
        LocalToScreen({0.25,0}).DrawLine(LocalToScreen({0.25,1}), lineColor);
        raylib::DrawText("0.0", bounds.x+fontSize, bounds.y+bounds.height-fontSize, fontSize, lineColor);
        raylib::DrawText("0.5", bounds.x+bounds.width/2.f, bounds.y+bounds.height-fontSize, fontSize, lineColor);
        raylib::DrawText("1.0", bounds.x+bounds.width-fontSize, bounds.y+bounds.height-fontSize, fontSize, lineColor);
        raylib::DrawText("0.25", innerBounds.x+innerBounds.width/4.f, bounds.y+bounds.height-fontSize, fontSize, lineColor);
        raylib::DrawText("0.75", innerBounds.x+3*innerBounds.width/4.f, bounds.y+bounds.height-fontSize, fontSize, lineColor);
        raylib::DrawText("0", bounds.x+fontSize, bounds.y+bounds.height-2*fontSize, fontSize, lineColor);
        raylib::DrawText("1", bounds.x+fontSize, bounds.y, fontSize, lineColor);
        raylib::DrawText("0.5", bounds.x+fontSize, bounds.y+bounds.height/2.f-fontSize, fontSize, lineColor);
    }

    void DrawPoints(){
        for(int i=0; i < points.size(); i++){
            auto& p = points[i];
            raylib::Vector2 drawPos = LocalToScreen(p.position);

            if(selectedPointIndex.has_value() && selectedPointIndex.value() == i){
                DrawRectangle(drawPos.x-fontSize/2.f,drawPos.y-fontSize/2.f,  fontSize, fontSize, BLUE);
                // draw anchors
                if(i<points.size()-1)
                    DrawLine(drawPos.x, drawPos.y, drawPos.x+std::cos(p.rightAngle)*handleLength, drawPos.y+std::sin(p.rightAngle)*handleLength, BLUE);
                if(i>0)
                    DrawLine(drawPos.x, drawPos.y, drawPos.x-std::cos(p.leftAngle)*handleLength, drawPos.y+std::sin(p.leftAngle)*handleLength, BLUE);
            } else {
                DrawRectangle(drawPos.x-fontSize/2.f,drawPos.y-fontSize/2.f,  fontSize, fontSize, BLACK);
            }
            if(hoveredPointIndex.has_value() && hoveredPointIndex.value() == i){
                DrawRectangleLines(drawPos.x-hoverSize/2.f,drawPos.y-hoverSize/2.f,  hoverSize, hoverSize, BLACK);
            }
        }
    }

    std::vector<CurvePoint> GetSortedPoints(bool addEdges = false){
        std::vector<CurvePoint> sortedPoints(points.size());
        for(int i=0; i < points.size(); i++) sortedPoints[i] = points[i];
        std::sort(sortedPoints.begin(), sortedPoints.end(), [](const CurvePoint& p1, const CurvePoint& p2) {return p1.position.x < p2.position.x;});

        if(sortedPoints[0].position.x > 0 && addEdges){
            CurvePoint edge;
            edge.position.x = 0;
            edge.position.y = sortedPoints[0].position.y;
            sortedPoints.insert(sortedPoints.begin(), edge);
        }
        if(sortedPoints[sortedPoints.size()-1].position.x < 1 && addEdges){
            CurvePoint edge;
            edge.position.x = 1;
            edge.position.y = sortedPoints[sortedPoints.size()-1].position.y;
            sortedPoints.push_back(edge);
        }
        return std::move(sortedPoints);
    }

    void DrawCurve(){
        // Sort points on x axis
        auto sortedPoints = GetSortedPoints(true);

        for(int i=0; i < sortedPoints.size()-1; i++){
            const CurvePoint& p = sortedPoints[i];
            const CurvePoint& p2 = sortedPoints[i+1];
            auto pos1 = LocalToScreen(p.position);
            auto pos2 = LocalToScreen(p2.position);
            float tangentLength = (p2.position.x-p.position.x)/2.f;
            raylib::Vector2 controlOffset1{std::cos(p.rightAngle), std::sin(p.rightAngle)};
            raylib::Vector2 controlOffset2{-std::cos(p2.leftAngle), std::sin(p2.leftAngle)};
            raylib::Vector2 control1 = LocalToScreen(p.position+controlOffset1*tangentLength);
            raylib::Vector2 control2 = LocalToScreen(p2.position+controlOffset2*tangentLength);
            DrawLineBezierCubic(pos1,pos2, control1, control2, 1, BLACK);
        }
    }
};


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
        auto func = curveEdit.GetFunc();
        int res = 500;
        float t = 1.f/(float) res;
        float scale = 100;
        for(int i=0; i < res; i++){
            float x = t*(float)i;
            float y = func(x);
            float x2 = t*(float)(i+1);
            float y2 = func(x2);
            DrawLine(x*scale,-y*scale,x2*scale,-y2*scale, YELLOW);
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
        if(GuiIntInput({x,y,width,height}, particlesCount, 0, 100000, inputBoxFocus == 1))
            inputBoxFocus = inputBoxFocus == 1 ? 0 : 1;
        y+=dy + height;

        // Particle life time
        GuiLabel({x,y,width,height}, "Life time");
        y += dy+height/2;
        if(GuiIntInput({x,y,width,height}, particlesCount, 0, 100000, inputBoxFocus == 2))
            inputBoxFocus = inputBoxFocus == 2 ? 0 : 2;
        y += dy+height;

        curveEdit.Draw({x,y,3*width,6*height});
        y += dy+6*height;


        // Saving & loading settings
        x = screenSize.x-16-width;
        y = 16;
        if(GuiButton({x,y,width, height}, "Load settings")){

        }
        y += dy+height;
        if(GuiButton({x,y,width, height}, "Save settings")){

        }
    }
    void SetScreenSize(int screenWidth, int screenHeight){
        screenSize.x = screenWidth;
        screenSize.y = screenHeight;
        cam.offset = {screenWidth / 2.f, screenHeight / 2.f};
    }
private:
    CurveEditor curveEdit;
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