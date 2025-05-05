// Main file für FEMProc
// deklariert main funktion für executable

#include "defines.h"
#include "femModel/Model.h"
#include "GUI/ImGuiStyleDecls.h"
#include <implot.h>
#include "Rendering/3DRendering.h"

#define MODELCACHE "../bin/.CACHE"

// Eigene Ausgabe für Raylib Log
void RaylibLogCallback(int logType, const char* text, va_list args) {

    char formatted[512];
    vsnprintf(formatted, sizeof(formatted), text, args);

    // Logging aktiviert/deaktiviert
    // LOG << "[redirected RaylibLog] " << formatted << endl;  // Weiterleitung LOG Makro
}

void InputExpression(const std::string& label, Expression& source, const std::string& suffix = "_exprInput"){

    static std::string expressionBuffer;
    expressionBuffer = source->__str__();

    // Platz machen für schreiboperationen bis 4 fache größe der ursprünglichen Expression
    expressionBuffer.resize(expressionBuffer.size() * 4);

    //
    ImGui::Text(label.c_str());
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(expressionBuffer.c_str()).x + ImGui::GetStyle().FramePadding.x * 4.0f);
    if (ImGui::InputText(("##"+label+suffix).c_str(), expressionBuffer.data(), expressionBuffer.capacity() + 1, ImGuiInputTextFlags_EnterReturnsTrue)) {
        source = SymEngine::parse(expressionBuffer);
    }
}

void displayExpression(const std::string& label, Expression& source, const std::string& suffix = "_exprDisplay"){

    static std::string expressionBuffer;
    expressionBuffer = source->__str__();

    ImGui::Text((label + " ").c_str());
    ImGui::SetNextItemWidth(ImGui::CalcTextSize(expressionBuffer.c_str()).x + ImGui::GetStyle().FramePadding.x * 4.0f);
    ImGui::Text(expressionBuffer.data());
}

bool InputSliderFloatExpression(const std::string& label, Expression& source, const float& lowerBorder = -2, const float& upperBorder = 10,
    bool oneLiner = false, const std::string& suffix = "_exprfloatInputSlider"){
    
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

bool InputSliderFloat(const std::string& label, float& source, const float& lowerBorder = -2, const float& upperBorder = 10,
    bool oneLiner = false, const std::string& suffix = "_exprfloatInputSlider"){
    
    static float valBuffer, tempBuffer;
    valBuffer = source;

    ImGui::Text(label.c_str());
    
    if(oneLiner){
        ImGui::SameLine();
    }

    if(ImGui::SliderFloat(("##"+label+suffix+"slider").c_str(), &valBuffer, lowerBorder,upperBorder)){
        tempBuffer = valBuffer;
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

int main(void)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    LOG << "** Source Code " << countLinesInDirectory("../src") << " lines" << endl;
    LOG << "** Procs Code " << countLinesInDirectory("../Procs") << " lines" << endl;
    LOG << endl;

    FemModel model;
    model.loadFromCache();

    //
    SetTraceLogCallback(RaylibLogCallback); 

    InitWindow(600, 600, "<><FEMProc><>");

    // Raylib Fenster init
    float winSizeFaktor = 0.5f; // z.B. halbe Bildschirmgröße

    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int windowWidth = screenWidth * winSizeFaktor;
    int windowHeight = screenHeight * winSizeFaktor;

    bool fullScreen = false;

    SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition((screenWidth - windowWidth)/2, (screenHeight - windowHeight)/2);

    LOG << "-- init auf Bildsirm mit Screensize [" << screenWidth << "|" << screenHeight << "]" << endl

    Image icon = LoadImage("../Recc/Compilation/icon.png");

    // RGB zu RGBA falls alpha Kanal fehlt
    if (icon.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        LOG << "-- Konvertiere icon.png zu RGBA -> ergänze alpha channel" << endl;
        LOG << endl;
        ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }

    SetWindowIcon(icon);
    UnloadImage(icon);

    // rlImGui initialisieren (ImGui-Kontext wird angelegt)
    rlImGuiSetup(true);

    // ImPlot-Kontext anlegen (nach ImGui!)
    ImPlot::CreateContext();

    //
    SetupImGuiStyle();

    float imguiScale = 1.0f;

    // Haupt-Loop
    bool closeWindow = false;
    while (!closeWindow)
    {
        bool currentCtrlState = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        static std::string modelSource = fs::path(model.getSource()).filename().string();
        
        // Nur bei gedrückter Strg-Taste reagieren
        if(currentCtrlState) {

            float wheelMove = GetMouseWheelMove();
            
            if(wheelMove != 0) {
                // Skalierungsfaktor anpassen (min: 0.5, max: 3.0)
                imguiScale += wheelMove * 0.1f;
                imguiScale = Clamp(imguiScale, 0.5f, 3.0f);
                
                scaleImguiUI(imguiScale);
            }
        }

        if(WindowShouldClose()){

            // Kreuz gedrückt
            closeWindow = true;
        }

        if(IsKeyPressed(KEY_ESCAPE)){

            // cleanUp
            closeWindow = true;
        }

        if(IsKeyPressed(KEY_F11)){

            LOG << "++ toggleScreen" << endl;

            if(!fullScreen){
                SetWindowSize(screenWidth, screenHeight);
                SetWindowPosition(0,0);
            } else {
                // resize auf originalGröße
                SetWindowSize(windowWidth, windowHeight);
                SetWindowPosition((screenWidth - windowWidth)/2, (screenHeight - windowHeight)/2);
            }

            fullScreen = !fullScreen;
        }

        // Toggle Cursor capture
        static bool hideCursor = false;
        if (IsKeyPressed(KEY_C))
        {
            if (IsCursorHidden())
            {
                ShowCursor();
                EnableCursor();
            }
            else
            {
                HideCursor();
                DisableCursor();
            }
            hideCursor = !hideCursor;
        }

        if (ImGui::GetIO().WantCaptureMouse && !hideCursor){
            
        }

        BeginDrawing();
        ClearBackground(Color(30,30,30,255));

        //
        rlImGuiBegin();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("New"))
                {
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Open"))
                {

                    if(ImGui::MenuItem("Model")){

                        OpenFileDialog("Open femModel", { ".model" }, false, true, "../Import", [&](const std::string& chosenFilePath) {
                            
                            model.loadFromFile(chosenFilePath);
                            model.storePathInCache();

                            modelSource = fs::path(model.getSource()).filename().string();
                        });
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Credits"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Hardware"))
            {

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Größe und Position des Vert tabbars fürs model editing berechnen
        int winWidth = GetScreenWidth();
        int winHeight = GetScreenHeight();
        float panelWidth = winWidth * (1.0f/2.5f); // Breite des Panels
        ImVec2 panelPos(winWidth - panelWidth, ImGui::GetFrameHeight());
        ImVec2 panelSize(panelWidth, winHeight - ImGui::GetFrameHeight());

        // Fenster an rechter Seite über volle Höhe
        ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin(("Edit Model " + modelSource).c_str(), nullptr, flags);

        ImGui::Text(("Edit Model " + modelSource).c_str());

        ImGui::SameLine();

        //
        static const std::map<const char*, std::function<void()>> buttons = {
            {"Unload", [&](){ model.unload();}},
            {"Reload", [&](){ model.reload();}},
            {"Save", [&](){ model.getMesh().saveMaterial();}}
        };

        //
        float maxWidth = 0.0f;
        for (const auto& [label, func] : buttons) {
            float w = ImGui::CalcTextSize(label).x;
            if (w > maxWidth) maxWidth = w;
        }
        float previewWidth = ImGui::CalcTextSize("...").x;
        if (previewWidth > maxWidth) maxWidth = previewWidth;

        //
        maxWidth += ImGui::GetStyle().FramePadding.x * 4.0f;

        ImGui::SetNextItemWidth(maxWidth);
        if (ImGui::BeginCombo("##Model Interaction", "...")) {
            
            for (const auto& [label, func] : buttons) {
                if(ImGui::Button(label)){
                    func();
                }
            }
            ImGui::EndCombo();
        }

        static int selectedTab = 3;
        static std::map<int, int> selectedSubTab; // selectedSubTab[tabID] = subTabID

        const char* tabNames[] = { "Model", "Constraints", "Isomparam", "Material" };
        std::map<int, std::vector<const char*>> subTabNames = {
            {0, {"Import", "Export", "Settings"}},
            {1, {}},
            {2, {}},
            {3, {"std_params", "sampling", "pdf", "pdf_params", "pdf_settings"}}
        };

        maxWidth = 0.0f;
        float indentWidth = ImGui::GetStyle().IndentSpacing;

        for (int i = 0; i < IM_ARRAYSIZE(tabNames); ++i) {
            // Breite des Tab-Labels
            float tabWidth = ImGui::CalcTextSize(tabNames[i]).x;
            if (tabWidth > maxWidth) maxWidth = tabWidth;
        }

        // Breite der Subtab-Labels (falls vorhanden)
        for (const char* subLabel : subTabNames[selectedTab]) {

            float subTabWidth = ImGui::CalcTextSize(subLabel).x + indentWidth;
            if (subTabWidth > maxWidth) maxWidth = subTabWidth;
        }
        
        // Zusätzlich Padding für den Rahmen und Abstand
        float framePadding = ImGui::GetStyle().FramePadding.x * 4.0f;
        ImGui::BeginChild("VertTabBar", ImVec2(maxWidth + framePadding, 0), true);

        for (int i = 0; i < IM_ARRAYSIZE(tabNames); ++i) {
            if (ImGui::Selectable(tabNames[i], selectedTab == i)) {
                selectedTab = i;
            }
        
            // Subtabs unter dem aktuellen Tab
            if (selectedTab == i && subTabNames.count(i) > 0) {
                for (size_t j = 0; j < subTabNames[i].size(); ++j) {
                    ImGui::Bullet();
                    ImGui::SameLine();
                    if (ImGui::Selectable(subTabNames[i][j], selectedSubTab[i] == j)) {
                        selectedSubTab[i] = j;
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Create a child window for the tab content
        ImGui::BeginChild("TabContent", ImVec2(0, 0), true);

        if (subTabNames[selectedTab].size() > 0)
        {
            ImGui::Text("Edit %s::%s", tabNames[selectedTab], subTabNames[selectedTab][selectedSubTab[selectedTab]]);
        }
        else
        {
            ImGui::Text("Edit %s", tabNames[selectedTab]);
        }

        ImGui::Separator();

        switch (selectedTab)
        {
        case 0:
            ImGui::Text("Mesh Tab");
            break;
        case 1:
            ImGui::Text("constraint Tab");
            break;
        case 2:
            ImGui::Text("isoparam tab");
            break;
        case 3:{

            //
            static IsoMeshMaterial& mat = model.getMesh().getMaterial();
            mat = model.getMesh().getMaterial();

            switch (selectedSubTab[selectedTab]){

                case 0:{
                    InputSliderFloat("E", mat.E, 10000,20000, true);
                    InputSliderFloat("v", mat.v, 0.1,1, true);
                    InputSliderFloat("t", mat.t, 0.01, 0.5, true);
                    break;
                }
                case 1:{

                    ImPlot::SetNextAxesToFit();
                    if (ImPlot::BeginPlot("Sample Histogramm", ImVec2(-1,0))) {
                        ImPlot::PlotHistogram("Samples", model.getSamples().data(), model.getSamples().size(), 50, (1.0), ImPlotRange(), ImPlotHistogramFlags_Density);
                        ImPlot::EndPlot();
                    }

                    ImGui::Text("Mean : %.3f", model.m_mean);
                    ImGui::Text("Deviation : %.3f", model.m_deviation);

                    break;
                }
                case 2:{

                    InputExpression("pdf", mat.pdf);
                    ImGui::Separator();

                    displayExpression("pdf(xi)", mat.pdf_xi);

                    ImGui::Separator();

                    break;
                }
                case 3:{

                    ImGui::Text("Params");
                    ImGui::Separator();

                    for(auto& [symbol,sub] : mat.subs){
                        InputSliderFloatExpression("Params['" + symbol->__str__() + "']", sub, -2,10);
                    }
                
                    ImGui::Separator();

                    ImGui::Text("Add Param");

                    static bool openParamDialog = false;
                    if(!openParamDialog && ImGui::Button("Add")){
                        openParamDialog = true;
                    }

                    if(openParamDialog){

                        static char textBuffer[24];
                        static float valBuffer = 0.0f;

                        ImGui::Text("Param ");
                        ImGui::SameLine();
                        if(ImGui::InputText("##addParamLabel",textBuffer, ImGuiInputTextFlags_EnterReturnsTrue)){
                            //openParamDialog = false;
                        }

                        ImGui::Text("Value ");
                        ImGui::SameLine();
                        if(ImGui::InputFloat("##addParamValue", &valBuffer, 0, ImGuiInputTextFlags_EnterReturnsTrue)){
                            //openParamDialog = false;
                        }

                        if(IsKeyPressed(KEY_ENTER)){
                        
                            Symbol sym = SymEngine::symbol(textBuffer);
                            
                            if(!mat.subs.contains(sym) && sym->__str__() != ""){
                                mat.subs.try_emplace(SymEngine::symbol(textBuffer), toExpression(valBuffer));
                            }

                            valBuffer = 0;
                            memset(textBuffer, 0, sizeof(textBuffer));

                            openParamDialog = false;
                        }
                    }

                    ImGui::Separator();
                    ImGui::Text("Borders");
                    ImGui::Separator();
                    InputSliderFloat("xi min", mat.xi_min, -2,2,true);
                    InputSliderFloat("xi max", mat.xi_max, -2,2,true);
                    ImGui::Separator();

                    break;
                }
                case 4:{

                    InputSliderInt("nSamples ", mat.nSamples, 1, 1000000);

                    InputSliderFloat("tolerance", mat.tolerance, 0.001,0.2);
                    InputSliderFloat("segmentation", mat.segmentation, 0.001,0.2);

                    ImGui::Separator();

                    break;
                }
                default:{

                    break;
                }   
            }

            if(IsKeyPressed(KEY_ENTER)){
                    
                mat.substitutePdf();
                model.sampling();
            }

            break;
        }
        }
        ImGui::EndChild();

        ImGui::End();

        RenderFileDialog();

        rlImGuiEnd();

        EndDrawing();

        HandleFileDialog();
    }

    //
    ImPlot::DestroyContext();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}