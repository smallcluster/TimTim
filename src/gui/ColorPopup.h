#include "raylib.h"
#include "../external/raygui/raygui.h"

#include <string.h>

#ifndef GUI_COLORPOPUP_H
#define GUI_COLORPOPUP_H

struct GuiColorPopupState {
    // Define anchors
    Vector2 anchor01;            // ANCHOR ID:1
    
    // Define controls variables
    bool ColorPopupActive;            // WindowBox: ColorPopup
    Color ColorPickerValue;            // ColorPicker: ColorPicker
    int ColorModeActive;            // ComboBox: ColorMode
    bool RValueEditMode;
    int RValueValue;            // ValueBOx: RValue
    float RSliderValue;            // Slider: RSlider
    bool GValueEditMode;
    int GValueValue;            // ValueBOx: GValue
    float GSliderValue;            // Slider: GSlider
    bool BValueEditMode;
    int BValueValue;            // ValueBOx: BValue
    float BSliderValue;            // Slider: BSlider
    bool AValueEditMode;
    int AValueValue;            // ValueBOx: AValue
    float ASliderValue;            // Slider: ASlider

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

};


GuiColorPopupState InitGuiColorPopup(void);
void GuiColorPopup(GuiColorPopupState *state);


GuiColorPopupState InitGuiColorPopup(void)
{
    GuiColorPopupState state = { 0 };

    // Init anchors
    state.anchor01 = (Vector2){ 96, 72 };            // ANCHOR ID:1
    
    // Initilize controls variables
    state.ColorPopupActive = true;            // WindowBox: ColorPopup
    state.ColorPickerValue = (Color){ 0, 0, 0, 0 };            // ColorPicker: ColorPicker
    state.ColorModeActive = 0;            // ComboBox: ColorMode
    state.RValueEditMode = false;
    state.RValueValue = 0;            // ValueBOx: RValue
    state.RSliderValue = 0.0f;            // Slider: RSlider
    state.GValueEditMode = false;
    state.GValueValue = 0;            // ValueBOx: GValue
    state.GSliderValue = 0.0f;            // Slider: GSlider
    state.BValueEditMode = false;
    state.BValueValue = 0;            // ValueBOx: BValue
    state.BSliderValue = 0.0f;            // Slider: BSlider
    state.AValueEditMode = false;
    state.AValueValue = 0;            // ValueBOx: AValue
    state.ASliderValue = 0.0f;            // Slider: ASlider

    // Custom variables initialization

    return state;
}


void GuiColorPopup(GuiColorPopupState *state)
{
    // Draw controls
    if (state->ColorPopupActive)
    {
        state->ColorPopupActive = !GuiWindowBox((Rectangle){ state->anchor01.x + 0, state->anchor01.y + 0, 304, 216 }, "Edit Color");



        // rgb color pick up
        Color c = GuiColorPicker((Rectangle){ state->anchor01.x + 8, state->anchor01.y + 32, 112, 112 }, NULL, state->ColorPickerValue);

        // RGB or HSV mode
        state->ColorModeActive = GuiComboBox((Rectangle){ state->anchor01.x + 8, state->anchor01.y + 184, 136, 24 }, "RGB;HSV", state->ColorModeActive);

        if(!state->AValueEditMode)
            state->AValueValue = state->ColorPickerValue.a;

        state->ASliderValue = state->ColorPickerValue.a;
        if(state->ColorModeActive == 0){
            if(!state->RValueEditMode)
                state->RValueValue = c.r;
            if(!state->GValueEditMode)
                state->GValueValue = c.g;
            if(!state->BValueEditMode)
                state->BValueValue = c.b;

            state->RSliderValue = c.r;
            state->GSliderValue = c.g;
            state->BSliderValue = c.b;
        } else {
            Vector3 hsv = ColorToHSV(c);

            if(!state->RValueEditMode)
                state->RValueValue = lround(hsv.x);
            if(!state->GValueEditMode)
                state->GValueValue = lround(hsv.y * 100.f);
            if(!state->BValueEditMode)
                state->BValueValue = lround(hsv.z * 100.f);

            state->RSliderValue = lround(hsv.x);
            state->GSliderValue = lround(hsv.y * 100.f);
            state->BSliderValue = lround(hsv.z * 100.f);
        }

        // const
        const char* RText = state->ColorModeActive == 0 ? "R " : "H ";
        const char* GText = state->ColorModeActive == 0 ? "G " : "S ";
        const char* BText = state->ColorModeActive == 0 ? "B " : "V ";

        const int maxR = state->ColorModeActive == 0 ? 255 : 360;
        const int maxG = state->ColorModeActive == 0 ? 255 : 100;
        const int maxB = state->ColorModeActive == 0 ? 255 : 100;
        const int maxA = 255;

        bool RValueInput = GuiValueBox((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 32, 128, 24 }, RText, &state->RValueValue, 0, maxR, state->RValueEditMode);
        if(RValueInput) {
            state->RValueEditMode = !state->RValueEditMode;
            // update slider values
            if(RValueInput && !state->RValueEditMode){
                state->RSliderValue = state->RValueValue;
            }
        }
        state->RSliderValue = GuiSlider((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 56, 128, 10 }, NULL, NULL, state->RSliderValue, 0, maxR);
        bool GValueInput = GuiValueBox((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 72, 128, 24 }, GText, &state->GValueValue, 0, maxG, state->GValueEditMode);
        if (GValueInput){
            state->GValueEditMode = !state->GValueEditMode;
            // update slider values
            if(GValueInput && !state->GValueEditMode){
                state->GSliderValue = state->GValueValue;
            }
        }
        state->GSliderValue = GuiSlider((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 96, 128, 10 }, NULL, NULL, state->GSliderValue, 0, maxG);
        bool BValueInput = GuiValueBox((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 112, 128, 24 }, BText, &state->BValueValue, 0, maxB, state->BValueEditMode);
        if (BValueInput) {
            state->BValueEditMode = !state->BValueEditMode;
            // update slider values
            if(BValueInput && !state->BValueEditMode){
                state->BSliderValue = state->BValueValue;
            }
        }
        state->BSliderValue = GuiSlider((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 136, 128, 10 }, NULL, NULL, state->BSliderValue, 0, maxB);

        bool AValueInput =GuiValueBox((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 152, 128, 24 }, "A ", &state->AValueValue, 0, maxA, state->AValueEditMode);
        if (AValueInput){
            state->AValueEditMode = !state->AValueEditMode;
            // update slider values
            if(AValueInput && !state->AValueEditMode){
                state->ASliderValue = state->AValueValue;
            }
        }
        state->ASliderValue = GuiSlider((Rectangle){ state->anchor01.x + 168, state->anchor01.y + 176, 128, 10 }, NULL, NULL, state->ASliderValue, 0, maxA);

        if(state->ColorModeActive == 0) {
            c.r = state->RSliderValue;
            c.g = state->GSliderValue;
            c.b = state->BSliderValue;
            c.a = state->ASliderValue;
        } else {
            c = ColorFromHSV(state->RSliderValue, state->GSliderValue/100.f, state->BSliderValue/100.f);
            c.a = state->ASliderValue;
        }

        state->ColorPickerValue = c;


        // Draw checkers
        #if !defined(RAYGUI_COLORBARALPHA_CHECKED_SIZE)
            #define RAYGUI_COLORBARALPHA_CHECKED_SIZE   10
        #endif
        Rectangle r = (Rectangle){state->anchor01.x + 8, state->anchor01.y + 152, 136, 24};

        BeginScissorMode(r.x, r.y, r.width, r.height);
        int nx = ceil(r.width / RAYGUI_COLORBARALPHA_CHECKED_SIZE);
        int ny = ceil(r.height / RAYGUI_COLORBARALPHA_CHECKED_SIZE);
        for(int i=0; i < ny; i++){
            for(int j=0; j < nx; j++){
                DrawRectangle(r.x+j*RAYGUI_COLORBARALPHA_CHECKED_SIZE, r.y+i*RAYGUI_COLORBARALPHA_CHECKED_SIZE, RAYGUI_COLORBARALPHA_CHECKED_SIZE, RAYGUI_COLORBARALPHA_CHECKED_SIZE , (i+j) % 2 == 0 ? GRAY : WHITE);
            }
        }
        EndScissorMode();

        DrawRectangle(r.x, r.y, r.width, r.height , state->ColorPickerValue);
    }
}

#endif // GUI_COLORPOPUP_IMPLEMENTATION
