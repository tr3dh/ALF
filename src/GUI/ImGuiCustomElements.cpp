#include "ImGuiCustomElements.h"

// fügt zeilenumbrüche in string ein wenn maxLen überschritten wird
void InsertLineBreaks(std::string& text, size_t maxLen) {

    size_t count = 0;
    for (size_t i = 0; i < text.size(); ++i) {

        // neue Zeile
        if (text[i] == '\n') {
            count = 0;

        // innerhalb bestehender Zeile
        } else {
            count++;

            // gewünschte Länge erreicht
            if (count >= maxLen) {
                
                // Zeilenumbruch einfügen
                if (i + 1 < text.size() && text[i + 1] != '\n') {
                    text.insert(i + 1, 1, '\n');
                    count = 0;
                    ++i;
                }
            }
        }
    }
}

// formatiert text mit umbrüchen neu
void NormalizeLineBreaks(std::string& text, size_t maxLen) {

    text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
    InsertLineBreaks(text, maxLen);
}

// callback funktion zum einfügen von umbrüchen bei überschreiten der zeilenlänge
int LinebreakCallback(ImGuiInputTextCallbackData* data) {

    // blockiert Eingabe von '\n'
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
        if (data->EventChar == '\n') {
            return 1;
        }
    }

    // Cachen der Cursor Position
    int oldCursor = data->CursorPos;

    // Übertrag und update nur wenn tatsächlich Bearbeitung stattgefunden hat
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {

        std::string text(data->Buf, data->BufTextLen);
        NormalizeLineBreaks(text, g_maxLineLen);

        // wenn tatsächlich Änderung des Buffers stattgefunden hat
        if (text != std::string(data->Buf, data->BufTextLen)) {

            // Aktualisieren der BufferDaten mit geupdateten Umbrüchen
            strncpy(data->Buf, text.c_str(), data->BufSize);

            // für imgui format
            data->Buf[data->BufSize - 1] = '\0'; // letzten Eintrag markieren
            data->BufTextLen = strlen(data->Buf); // Länge aktualisieren
            data->BufDirty = true;
            data->CursorPos = oldCursor;

            if (data->CursorPos > 0 && data->Buf[data->CursorPos - 1] == '\n') {
                data->CursorPos += 1;
            }

            data->SelectionStart = data->SelectionEnd = data->CursorPos;
        }
    }
    return 0;
}

// hauptfunktion für die eingabe von ausdrücken im inputfeld
bool InputString(const std::string& label, std::string& source, const std::string& suffix) {
    
    // durchschnittliche zeichenbreite für abschätzung der max token pro zeile
    static float avgCharWidth;
    avgCharWidth = ImGui::CalcTextSize("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ").x / 52.0f;

    // buffer für eingabeinhalt (wird direkt von imgui beschrieben)
    static std::vector<char> expressionBuffer(1024);

    // speichert zuletzt bekannten source-string um zu erkennen ob neuladen nötig
    static std::string lastSourceStr;

    // aktueller string aus dem expression objekt
    static std::string currentSourceStr;
    currentSourceStr = source;

    // berechnung wie viele tokens (zeichen) in eine zeile passen
    static float lineLen, lineTokens;
    lineLen = ImGui::GetContentRegionAvail().x;
    lineTokens = (lineLen - ImGui::GetStyle().FramePadding.y * 8) / avgCharWidth;
    g_maxLineLen = lineTokens;

    // buffer aktualisieren wenn source geändert wurde
    if (lastSourceStr != currentSourceStr) {
        std::string exprStr = currentSourceStr;

        NormalizeLineBreaks(exprStr, lineTokens);

        size_t copyLen = std::min(exprStr.size(), expressionBuffer.size() - 1);

        std::fill(expressionBuffer.begin(), expressionBuffer.end(), 0);
        std::copy(exprStr.begin(), exprStr.begin() + copyLen, expressionBuffer.begin());
        expressionBuffer[copyLen] = '\0';

        lastSourceStr = currentSourceStr;
    }

    // pointer für callback setzen
    g_bufferPtr = &expressionBuffer;

    // eigentliche eingabe: multiline input mit callback für umbrüche
    if (ImGui::InputTextMultiline(("##" + label + suffix).c_str(), expressionBuffer.data(), expressionBuffer.size(),
        ImVec2(lineLen, ImGui::GetTextLineHeightWithSpacing() * 4),
        ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackCharFilter, LinebreakCallback)) {

    }

    // aus mir unerfindlichen gründen funktioniert dieser shortcut nicht
    // obwohl nach enter press der parse stattfindet wird der buffer nicht anständig aktualisiert
    // !! später fixen
    bool passButton = false;
    if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        passButton = true;
    }

    if (ImGui::Button("parse")){
        passButton = true;
    }

    // bei klick auf parse button: ausdruck analysieren
    if (passButton) {

        // vor dem Parsen Zeilenumbrüche entfernen
        std::string toParse(expressionBuffer.data());
        toParse.erase(std::remove(toParse.begin(), toParse.end(), '\n'), toParse.end());

        source = toParse;

        // Buffer neu laden aus aktualisierter source die jetzt in der Theorie den gleichen Inhalt unter Umständen umgeformt enthalten sollte
        std::string updated = source;
        NormalizeLineBreaks(updated, lineTokens);
        size_t copyLen = std::min(updated.size(), expressionBuffer.size() - 1);

        std::fill(expressionBuffer.begin(), expressionBuffer.end(), 0);
        std::copy(updated.begin(), updated.begin() + copyLen, expressionBuffer.begin());
        expressionBuffer[copyLen] = '\0';

        lastSourceStr = updated;

        return true;
    }

    return false;
}

bool InputExpression(const std::string& label, Expression& source, const std::string& suffix) {

    std::string expressionString = source->__str__();

    if(InputString(label, expressionString, suffix)){

        source = SymEngine::parse(expressionString);

        expressionString = source->__str__();

        ImGui::ClearActiveID();
        // ImGui::SetKeyboardFocusHere(-1);

        return true;
    }
    return false;
}

void displayExpression(const std::string& label, Expression& source, const std::string& suffix){

    static std::string expressionBuffer;
    expressionBuffer = source->__str__();

    ImGui::Text((label + " ").c_str());
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(expressionBuffer.c_str()).x + ImGui::GetStyle().FramePadding.x * 4.0f);
    ImGui::TextWrapped(expressionBuffer.data());
}

bool InputSliderFloatExpression(const std::string& label, Expression& source, const float& lowerBorder, const float& upperBorder,
    bool oneLiner, const std::string& suffix){
    
    static float valBuffer, tempBuffer;
    valBuffer = string::convert<float>(source->__str__());

    ImGui::Text(label.c_str());
    
    if(oneLiner){
        ImGui::SameLine();
    }

    if(ImGui::SliderFloat(("##"+label+suffix+"slider").c_str(), &valBuffer, lowerBorder,upperBorder)){
        tempBuffer = valBuffer;
    };

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        // wenn Slider losgelassen wurde
        source = toExpression(tempBuffer);
        return true;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(valBuffer).c_str()).x + ImGui::GetStyle().FramePadding.x * 4);
    if (ImGui::InputFloat(("##" + label + suffix + "entry").c_str(), &valBuffer,0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        source = toExpression(valBuffer);
        return true;
    }

    return false;
}

bool InputSliderFloat(const std::string& label, float& source, const float& lowerBorder, const float& upperBorder,
    bool oneLiner, const std::string& suffix, bool instantReaktion){
    
    static float valBuffer, tempBuffer;
    valBuffer = source;

    ImGui::Text(label.c_str());
    
    if(oneLiner){
        ImGui::SameLine();
    }

    if(ImGui::SliderFloat(("##"+label+suffix+"slider").c_str(), &valBuffer, lowerBorder,upperBorder)){
        tempBuffer = valBuffer;

        if(instantReaktion){
            source = tempBuffer;
        }
    };

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        // wenn Slider losgelassen wurde
        source = tempBuffer;
        return true;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(valBuffer).c_str()).x + ImGui::GetStyle().FramePadding.x * 4);
    if (ImGui::InputFloat(("##" + label + suffix + "entry").c_str(), &valBuffer,0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        source = valBuffer;
        return true;
    }

    return false;
}

void RaylibColorEdit(Color& rayColor, const std::string& label){

    //
    float colorEdit[4] = {
        rayColor.r / 255.0f,
        rayColor.g / 255.0f,
        rayColor.b / 255.0f,
        rayColor.a / 255.0f
    };

    // Color edit widget
    if (ImGui::ColorEdit4(label.c_str(), colorEdit, ImGuiColorEditFlags_NoInputs)) {
    
        // Umwandlung in raycolor nach bearbeitung
        rayColor.r = (unsigned char)(colorEdit[0] * 255.0f);
        rayColor.g = (unsigned char)(colorEdit[1] * 255.0f);
        rayColor.b = (unsigned char)(colorEdit[2] * 255.0f);
        rayColor.a = (unsigned char)(colorEdit[3] * 255.0f);
    }
}