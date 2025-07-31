#include "defines.h"

#include "defines.h"
#include "GUI/ImGuiCustomElements.h"

void createFormulaTexture(const std::string& formula, Texture2D& texture){

    std::ofstream tex("formula.tex");
    tex << "\\documentclass[preview]{standalone}\n"
        "\\usepackage{amsmath}\n"
        "\\usepackage{xcolor}\n"
        "\\usepackage{graphicx}\n"
        "\\usepackage{bm}\n"
        "\\begin{document}\n"
        "\\color{red} $" << formula << "$\n"
        "\\end{document}\n";
    tex.close();

    system("pdflatex -interaction=nonstopmode formula.tex");
    system("pdftocairo -png -singlefile -transp -r 300 formula.pdf formula_tmp");
    system("magick formula_tmp.png -fuzz 20% -transparent white formula.png");

    texture = LoadTexture("formula.png");
}

int main(void)
{
    //
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int usableHeight = workArea.bottom - workArea.top; // verfügbare Höhe (ohne Taskleiste)
    int usableWidth  = workArea.right - workArea.left; // verfügbare Breite

    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    //
    enableRLLogging();
    SetTraceLogCallback(RaylibLogCallback);
    disableRLLogging();

    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(600,600, "<><FEMProc><>");

    // Raylib Fenster init
    float winSizeFaktor = 0.5f;

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

    std::string formula = "\\frac{\\sigma}{\\nu} \\cdot \\Phi";

    Texture2D texture;
    createFormulaTexture(formula, texture);

    ASSERT(texture.id != 0, "Textur wurde nicht geladen");

    //
    // ImGui::GetStyle().WindowBorderSize = 2.0f;
    // ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(0,0,0,1);

    // Fenster jetzt sichtbar machen
    ClearWindowState(FLAG_WINDOW_HIDDEN);

    //
    bool closeWindow = false;
    while (!closeWindow)
    {
        // deltaTime
        static float dt;
        dt = GetFrameTime();

        // Screen stats
        int monitor = GetCurrentMonitor();
        Vector2 monitorSize = {GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
        Vector2 winSize = {GetScreenWidth(), GetScreenHeight()};

        if(WindowShouldClose()){

            // Kreuz gedrückt
            closeWindow = true;
        }

        BeginDrawing();
        ClearBackground(Color(30,30,30,255));

        //
        rlImGuiBegin();

        if (ImGui::GetIO().WantCaptureMouse){
            
        }

        ImGuiWindowFlags window_flags = 0;
        // window_flags |= ImGuiWindowFlags_NoBackground;
        ImGui::Begin("Formula Edit", nullptr, window_flags);

        static bool editFormula = false;

        if(!editFormula){

            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0,0,0,0)); // normal
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25,0.125,0,1)); // beim Hover
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1,0.5,0,1)); // beim Klicken

            if (ImGui::ImageButton("icon",(ImTextureID)texture.id, ImVec2({texture.width, texture.height}))) {
                
                editFormula = true;
            }

            ImGui::PopStyleColor(3);
        }
        else if(InputString("formula", formula)){

            createFormulaTexture(formula, texture);
            editFormula = false;
        }

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