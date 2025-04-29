// Main file für FEMProc
// deklariert main funktion für executable

#include "defines.h"
#include "Model/Model.h"
#include "GUI/ImGuiStyleDecls.h"
#include <implot.h>

#define MODELCACHE "../bin/.CACHE"

// Eigene Ausgabe für Raylib Log
void RaylibLogCallback(int logType, const char* text, va_list args) {

    char formatted[512];
    vsnprintf(formatted, sizeof(formatted), text, args);

    // Logging aktiviert/deaktiviert
    // LOG << "[redirected RaylibLog] " << formatted << endl;  // Weiterleitung LOG Makro
}

int main(void)
{
    //
    LOG << std::fixed << std::setprecision(4);
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
    scaleImguiUI(1.6);

    // Haupt-Loop
    while (!WindowShouldClose())
    {
        static std::string modelSource = fs::path(model.getSource()).filename().string();
        
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

        BeginDrawing();
        ClearBackground(Color(30,30,30,255));

        //
        rlImGuiBegin();

        //
        ImGui::Begin("ImPlot Histogramm");

        // ImPlot Histogramm
        ImPlot::SetNextAxesToFit();
        if (ImPlot::BeginPlot("Histogramm")) {
            ImPlot::PlotHistogram("Samples", model.getSamples().data(), model.getSamples().size(), 50, (1.0), ImPlotRange(), ImPlotHistogramFlags_Density);
            ImPlot::EndPlot();
        }

        ImGui::End();

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

        ImGui::Text(("Edit Model : " + modelSource).c_str());

        ImGui::SameLine();

        //
        static const std::map<const char*, std::function<void()>> buttons = {
            {"Unload", [&](){ model.unload();}},
            {"Reload", [&](){ model.reload();}},
            {"Save", [&](){ LOG << "!! not implemented yet" << endl;}}
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
        const char* tabNames[] = { "Model", "Constraints", "Isomparam", "Material" };

        maxWidth = 0;
        for (const auto& label : tabNames) {
            float w = ImGui::CalcTextSize(label).x;
            if (w > maxWidth) maxWidth = w;
        }

        // Create a child window for the tab bar
        ImGui::BeginChild("VertTabBar", ImVec2(maxWidth + ImGui::GetStyle().FramePadding.x * 4.0f, 0), true);
        for (int i = 0; i < IM_ARRAYSIZE(tabNames); i++)
        {
            if (ImGui::Selectable(tabNames[i], selectedTab == i))
            {
                selectedTab = i;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // Create a child window for the tab content
        ImGui::BeginChild("TabContent", ImVec2(0, 0), true);
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

            ImGui::Text("Edit Material...");

            ImGui::Separator();

            //
            static IsoMeshMaterial& mat = model.getMesh().getMaterial();
            mat = model.getMesh().getMaterial();

            static std::string pdfBuffer;
            pdfBuffer = mat.pdf->__str__();

            //
            ImGui::Text("pdf : ");
            ImGui::SetNextItemWidth(ImGui::CalcTextSize(pdfBuffer.c_str()).x + ImGui::GetStyle().FramePadding.x * 4.0f);
            if (ImGui::InputText("##pd Function", pdfBuffer.data(), pdfBuffer.capacity() + 1, ImGuiInputTextFlags_EnterReturnsTrue)) {
                mat.pdf = SymEngine::parse(pdfBuffer);
            }

            ImGui::Separator();

            static float valBuffer;

            for(auto& [symbol,sub] : mat.subs){

                valBuffer = string::convert<float>(sub->__str__());

                ImGui::Text(("Params['" + symbol->__str__() + "']").c_str());
                
                //
                if(ImGui::SliderFloat(("##slider"+symbol->__str__()).c_str(), &valBuffer, -5*valBuffer, 5*valBuffer)){
                    //sub = toExpression(valBuffer);
                };

                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    // wenn Slider losgelassen wurde
                    sub = toExpression(valBuffer);
                }

                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(valBuffer).c_str()).x + ImGui::GetStyle().FramePadding.x * 4);
                if (ImGui::InputFloat(("##" + symbol->__str__()).c_str(), &valBuffer,0, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    sub = toExpression(valBuffer);
                }

            }

            ImGui::Separator();

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