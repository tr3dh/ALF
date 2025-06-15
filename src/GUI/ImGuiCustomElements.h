#pragma once

#include "defines.h"

// maximale zeilenlänge global für callback
static size_t g_maxLineLen = 0;

// globaler pointer auf aktuellen buffer
static std::vector<char>* g_bufferPtr = nullptr;

// fügt zeilenumbrüche in string ein wenn maxLen überschritten wird
void InsertLineBreaks(std::string& text, size_t maxLen);

// formatiert text mit umbrüchen neu
void NormalizeLineBreaks(std::string& text, size_t maxLen);

// callback funktion zum einfügen von umbrüchen bei überschreiten der zeilenlänge
int LinebreakCallback(ImGuiInputTextCallbackData* data);

//
bool InputString(const std::string& label, std::string& source, const std::string& suffix = "_strInput");

// hauptfunktion für die eingabe von ausdrücken im inputfeld
bool InputExpression(const std::string& label, Expression& source, const std::string& suffix = "_exprInput");

void displayExpression(const std::string& label, Expression& source, const std::string& suffix = "_exprDisplay");

bool InputSliderFloatExpression(const std::string& label, Expression& source, const float& lowerBorder = -2, const float& upperBorder = 10,
    bool oneLiner = false, const std::string& suffix = "_exprfloatInputSlider");

bool InputSliderFloat(const std::string& label, float& source, const float& lowerBorder = -2, const float& upperBorder = 10,
    bool oneLiner = false, const std::string& suffix = "_exprfloatInputSlider");

// für alle int typen
// . unsigned, signed, ...
template<typename T>
bool InputSliderInt(const std::string& label, T& source, const int& lowerBorder = -2, const int& upperBorder = 10,
    bool oneLiner = false, const std::string& suffix = "_exprfloatInputSlider"){
    
    static int valBuffer, tempBuffer;
    valBuffer = source;

    ImGui::Text(label.c_str());
    
    if(oneLiner){
        ImGui::SameLine();
    }

    if(ImGui::SliderInt(("##"+label+suffix+"slider").c_str(), &valBuffer, lowerBorder,upperBorder)){
        tempBuffer = valBuffer;
    };

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        // wenn Slider losgelassen wurde
        source = tempBuffer;
        return true;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(valBuffer).c_str()).x + ImGui::GetStyle().FramePadding.x * 4);
    if (ImGui::InputInt(("##" + label + suffix + "entry").c_str(), &valBuffer,0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        source = valBuffer;
        return true;
    }

    return false;
}

void RaylibColorEdit(Color& rayColor, const std::string& label = NULLSTR);