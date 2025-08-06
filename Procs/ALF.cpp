// Main file für FEMProc
// deklariert main funktion für executable

#include "defines.h"
#include "templateDecls.h"
#include "Libaries/Libaries.h"

const std::string g_programsConfigCache = "../bin/CONFIG.CACHE";
std::string g_fileBrowserCWD = fs::current_path().string() + "/../Import/";
bool g_bindStartUpToReloadRecent = false;

std::string relPath(const std::string& str){
    return fs::relative(str, fs::current_path()).string();
}

void cacheConfigs(){

    if(!fs::exists(fs::path(g_programsConfigCache).parent_path())){
        fs::create_directory(fs::path(g_programsConfigCache).parent_path());
    }

    //
    ByteSequence bs;

    bs.insertMultiple(
        relPath(g_fileBrowserCWD),
        g_backgroundColor,
        g_progressDisplayBackgroundColor,
        g_progressDisplayTextColor,
        g_cellsNodeRadius2D,
        g_cellsWireFrameThickness2D,
        g_cellsWireFrameThickness3D,
        undeformedFrame,
        deformedFrame,
        deformedFramePlusXi,
        deformedFrameMinusXi,
        g_bindStartUpToReloadRecent
    );

    //
    bs.encode(g_encoderKey);
    bs.toFile(g_programsConfigCache);
}

void loadCachedConfigs(){

    if(!fs::exists(fs::path(g_programsConfigCache))){
        return;
    }

    //
    ByteSequence bs;

    bs.fromFile(g_programsConfigCache);
    bs.decode(g_encoderKey);

    bs.extractMultipleReversed(
        g_fileBrowserCWD,
        g_backgroundColor,
        g_progressDisplayBackgroundColor,
        g_progressDisplayTextColor,
        g_cellsNodeRadius2D,
        g_cellsWireFrameThickness2D,
        g_cellsWireFrameThickness3D,
        undeformedFrame,
        deformedFrame,
        deformedFramePlusXi,
        deformedFrameMinusXi,
        g_bindStartUpToReloadRecent
    );

    // aus relativem Pfad global gültigen generieren
    g_fileBrowserCWD = fs::current_path().string() + "/" + g_fileBrowserCWD;
}

//
void DrawArrow3D(Vector3 start, Vector3 end, float shaftRadius, float headRadius, float headLength, Color color) {

    Vector3 dir = Vector3Subtract(end, start);
    float length = Vector3Length(dir);
    Vector3 normDir = Vector3Normalize(dir);

    Vector3 shaftEnd = Vector3Add(start, Vector3Scale(normDir, length - headLength));

    // Schaft : gleichmäßiger Zylinder
    DrawCylinderEx(start, shaftEnd, shaftRadius, shaftRadius, 8, color);

    // Spitze : Kegel (Zylinder mit topRadius 0)
    DrawCylinderEx(shaftEnd, end, headRadius, 0.0f, 8, color);
}

//
void DrawAxisLabel(Vector3 position, Camera3D camera, const char* label, Color color) {

    Font font = GetFontDefault();
    Vector2 textSize = MeasureTextEx(font, label, 20, 1);
    Vector2 screenPos = GetWorldToScreen(position, camera);
    DrawTextEx(font, label, (Vector2){ screenPos.x - textSize.x / 2, screenPos.y - textSize.y / 2 }, 20, 1, color);
}

int main(void)
{
    //
    loadCachedConfigs();

    //
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int usableHeight = workArea.bottom - workArea.top;          // nutzbare Höhe (ohne Taskleiste)
    int usableWidth  = workArea.right - workArea.left;          // nutzbare Breite (in den meisten Fällen gleich der Fensterbreite)

    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    LOG << "** Source Code " << countLinesInDirectory("../src") << " lines" << endl;
    LOG << "** Procs Code " << countLinesInDirectory("../Procs") << " lines" << endl;
    LOG << endl;

    //
    enableRLLogging();
    SetTraceLogCallback(RaylibLogCallback);
    disableRLLogging();

    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_WINDOW_RESIZABLE);
    InitWindow(600,600, "\t Adaptive, Lightweight Finite Element Tool (ALF)");
    windowInitialized = true;

    //
    const char* glVersion = (const char*)glGetString(GL_VERSION);
    LOG << "** OpenGL Version: " << glVersion << endl;

    const char* vendor = (const char*)glGetString(GL_VENDOR);
    LOG << "** GPU Vendor: " << vendor << endl;

    // string splitten da im glVersion String noch Infos über die graphikkarte stehen
    g_glVersion = string::convert<float>(std::string(glVersion).substr(0,3));

    // ab der 4.3 enthält opengl eine shader pipeline für comp shaders
    g_ComputeShaderBackendEnabled = g_glVersion >= 4.3f;

    //
    g_vendorCorp = std::string(vendor);

    //
    g_CudaBackendEnabled = string::contains(g_vendorCorp, "NVIDIA");

    LOG << (g_ComputeShaderBackendEnabled ? "** Computeshader Backend freigeschaltet" :
        "** Computeshader Backend gesperrt, opengl version " + std::to_string(g_glVersion).substr(0,3) + " ist nicht mit comp shadern kompatibel, erforderliche Version : OpenGL 4.3") << endl;

    LOG << (g_CudaBackendEnabled ? "** Cuda Backend freigeschaltet" :
        "** Cuda Backend gesperrt, vendor " + g_vendorCorp + " ist nicht mit Cuda kompatibel") << endl;

    LOG << "** -----------------------------------------" << endl;

    // Raylib Fenster init
    float winSizeFaktor = 0.6f;

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

    // Fenster jetzt sichtbar machen
    ClearWindowState(FLAG_WINDOW_HIDDEN);

    //
    FemModel model;
    if(g_bindStartUpToReloadRecent){ model.loadFromCache(); }
    
    float imguiScale = 1.0f;

    // Camera init
    Camera camera = {
        .position = {0,0,0},
        .target = {10, 2.5,0},
        .up = {0,1,0},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE    // oder CAMERA_ORTHOGRAPHIC für isometrische Ansicht -> Zoom funktioniert nicht mehr
    };

    // Kameraführungsmodi
    bool planarCam = false;
    bool orbitCam = false;
    bool fpsCam = false;

    // Gizmo für Koordinatensystem
    int gizmoSize = 100;
    int margin = 20;

    //
    Camera3D gizmoCam = { 0 };
    gizmoCam.fovy = 45.0f;
    gizmoCam.projection = CAMERA_PERSPECTIVE;

    // Maße für Achspfeile
    float arrowLen = 1.0f;
    float shaftRadius = 0.05f;
    float headRadius = 0.12f;
    float headLength = 0.25f;

    //
    float time = 0;

    //
    bool closeWindow = false;
    while (!closeWindow)
    {
        // deltaTime
        static float dt;
        dt = GetFrameTime();

        //
        time += dt;

        // Screen stats
        int monitor = GetCurrentMonitor();
        Vector2 monitorSize = {GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
        Vector2 winSize = {GetScreenWidth(), GetScreenHeight()};

        // Nur bei gedrückter Strg Taste reagieren
        bool currentCtrlState = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        if(currentCtrlState) {

            float wheelMove = GetMouseWheelMove();
            
            if(wheelMove != 0) {

                // Skalierungsfaktor anpassen, clamp zwischen 0.5 und 3, sodass UI nicht vollkommen aus
                // dem Bereich einer sinnvollen Skalierung entweicht
                imguiScale += wheelMove * 0.1f;
                imguiScale = Clamp(imguiScale, 0.5f, 3.0f);
                
                scaleImguiUI(imguiScale);
            }
        }

        if(WindowShouldClose()){

            // gibt true zurück wenn Schließen Icon des Fenster gedrückt wird
            // kann bei Bedarf überschrieben werden, sodass das Fenster nicht mehr über den Klick auf den Icon Button von OS
            // geschlossen werden kann
            //
            // Da progressbar ebenfalls ohne event verarbeitung ins Fenster schreibt bewirkt der Klick auf des Kreuz in der 
            // Fensterecke dort nichts
            //
            closeWindow = true;
        }

        if(IsKeyPressed(KEY_ESCAPE)){

            // cleanUp
            closeWindow = true;
        }

        if(IsKeyPressed(KEY_F10)){

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

        if(IsKeyPressed(KEY_F11)){

            LOG << "++ toggleScreen" << endl;

            if(!fullScreen){
                SetWindowSize(usableWidth, usableHeight);
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
        if (IsKeyPressed(KEY_C) && !IsKeyDown(KEY_LEFT_CONTROL))
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

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            planarCam = true;
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            planarCam = false;
        }
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
            orbitCam = true;
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){
            orbitCam = false;
        }

        // nur wenn nicht ImGui::GetIO().WantCaptureMouse
        fpsCam = (orbitCam && planarCam);

        if(IsKeyPressed(KEY_LEFT_ALT) || camera.position == (Vector3){0,0,0}){

            // Center camera
            camera.target = model.modelCenter;
            float distance = model.maxModelExtent / (2.0f * tanf(camera.fovy * 0.5f * (PI/180.0f)));
            camera.position = (Vector3){model.modelCenter.x - distance, model.modelCenter.y + distance, model.modelCenter.z - distance};
        }

        size_t modelDimension = model.getMesh().getCells().begin()->second.getPrefabIndex() > 0
            ? model.getMesh().getCells().begin()->second.getPrefab().nDimensions : 0;

        // Camera handling überspringen wenn Netz nicht 3D
        if(modelDimension != 3){

        }
        // wenn die Maus in der UI steht soll keine Camerafahrt stattfinden
        // wenn der Cursor versteckt ist soll die Camerafahrt troztdem durchgeführt werden
        else if(ImGui::GetIO().WantCaptureMouse && !hideCursor){

        }
        else if(fpsCam){

            calcFirstPersonCamera( camera,
                (Vector3){
                    (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) - (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)),
                    (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) - (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)),
                    IsKeyDown(KEY_SPACE) - IsKeyDown(KEY_LEFT_SHIFT)
                },
                (Vector3){
                    GetMouseDelta().x, GetMouseDelta().y, GetMouseWheelMove()
                },
                (Vector3){150,150,150},
                (Vector3){1,1,-1000},
                GetFrameTime()
            );
        }
        else if(planarCam || GetMouseWheelMove() != 0){

            calcNormalPlanarCamera(camera,(Vector3){GetMouseDelta().x, GetMouseDelta().y, GetMouseWheelMove()},{0.5,0.5,5});
        }
        else if(orbitCam){

            // um model Center drehen
            calcOrbitCamera(camera, GetMouseDelta(), model.modelCenter, true, {0.01,0.005});
        }

        // für Frame gültige Shadergrößen setzen
        if(g_useCellMeshShader){

            static Vector3 lightPos;

            SetShaderValue(g_cellMeshShader, GetShaderLocation(g_cellMeshShader, "viewPos"),
                &camera.position, SHADER_UNIFORM_VEC3);

            lightPos = model.modelCenter + model.modelExtend;

            SetShaderValue(g_cellMeshShader, GetShaderLocation(g_cellMeshShader, "lightPos"),
                &camera.position, SHADER_UNIFORM_VEC3);

            SetShaderValue(g_cellMeshShader, GetShaderLocation(g_cellMeshShader, "time"),
                &time, SHADER_UNIFORM_FLOAT);
        }

        BeginDrawing();
        ClearBackground(g_backgroundColor); //Color(30,30,30,255));

        //
        rlImGuiBegin();

        //
        static bool openVertTabWin = false;

        // Größe und Position des Vert tabbars fürs model editing berechnen
        float panelWidth = winSize.x * (1.0f/2.5f); // Breite des Panels

        ImVec2 panelPos;
        ImVec2 panelSize;

        if(!openVertTabWin){

            panelPos = {winSize.x - ImGui::CalcTextSize("<-").x - ImGui::GetStyle().FramePadding.x * 4.0f,
                        ImGui::GetFrameHeight()};
            panelSize = {ImGui::CalcTextSize("<-").x + ImGui::GetStyle().FramePadding.x * 4.0f,
                winSize.y - ImGui::GetFrameHeight()};
        } else {
            panelPos = {winSize.x - panelWidth, ImGui::GetFrameHeight()};
            panelSize = {panelWidth, winSize.y - ImGui::GetFrameHeight()};
        }

        Vector2 leftFrameCorner = {0,panelPos.y};                           // linke obere Ecke zeichenfläche
        Vector2 displayFrame = {panelPos.x - leftFrameCorner.x, winSize.y - leftFrameCorner.y};    // Größe Frame
        Vector2 displayFrameCenter = {leftFrameCorner.x + displayFrame.x/2, leftFrameCorner.y + displayFrame.y/2}; // Center des Frames

        //
        static int plotOnMesh = 0;
        static int plotData = static_cast<decltype(plotData)>(MeshData::VANMISES_STRESS);            // Van Mises Stress
        static int plotKoord = 0;           // x (für van Mises Stress gibts nur eine Größe deshalb 0)

        //
        static bool splitScreen = false;
        static bool splitScreenVertical = true;

        if(modelDimension == 3){

            BeginMode3D(camera);
            model.display(static_cast<MeshData>(plotData), plotKoord, plotOnMesh, displayFrame, displayFrameCenter,
                {100,100}, splitScreen, splitScreenVertical);
            EndMode3D();

            //
            rlDrawRenderBatchActive();

            //
            rlViewport(0,0, gizmoSize, gizmoSize);

            gizmoCam.position = Vector3Scale(Vector3Normalize(camera.position), 3.0f);
            gizmoCam.target = (Vector3){ 0, 0, 0 };
            gizmoCam.up = camera.up;

            BeginMode3D(gizmoCam);
            rlEnableDepthTest();

            DrawArrow3D((Vector3){0,0,0}, (Vector3){arrowLen,0,0}, shaftRadius, headRadius, headLength, RED);   // X
            DrawArrow3D((Vector3){0,0,0}, (Vector3){0,arrowLen,0}, shaftRadius, headRadius, headLength, GREEN); // Y
            DrawArrow3D((Vector3){0,0,0}, (Vector3){0,0,arrowLen}, shaftRadius, headRadius, headLength, BLUE);  // Z

            // Buchstaben an den Enden
            DrawAxisLabel((Vector3){arrowLen + 0.2f, 0, 0}, gizmoCam, "X", RED);
            DrawAxisLabel((Vector3){0, arrowLen + 0.2f, 0}, gizmoCam, "Y", GREEN);
            DrawAxisLabel((Vector3){0, 0, arrowLen + 0.2f}, gizmoCam, "Z", BLUE);

            EndMode3D();

            // Viewport zurücksetzen
            rlViewport(0, 0, winSize.x, winSize.y);
            rlDrawRenderBatchActive();
        }
        else {
            model.display(static_cast<MeshData>(plotData), plotKoord, plotOnMesh, displayFrame, displayFrameCenter,
                {100,100}, splitScreen, splitScreenVertical);
        }

        static std::string modelSource;
        modelSource = fs::path(model.getSource()).filename().string();

        //
        auto modelFileDialog = [&](){

            OpenFileDialog("Open femModel", { ".model" }, false, true, g_fileBrowserCWD, [&](const std::string& chosenFilePath) {

            // Wenn Plotkoord nicht resettet wird und zb auf zz in einem 3d Modell gesetzt ist dann ein 2d Modell geöffnet wird
            // versucht das Programm auf nicht vorhandene Einträge zuzugreifen

            // plotOnMesh = 0;
            // plotData = 0;
            plotKoord = 0;

            model.loadFromFile(chosenFilePath);
            model.storePathInCache();

            modelSource = fs::path(model.getSource()).filename().string();
            g_fileBrowserCWD = fs::path(model.getSource()).parent_path().string();
            });
            
            // Center camera
            camera.target = model.modelCenter;
            float distance = model.maxModelExtent / (2.0f * tanf(camera.fovy * 0.5f * (PI/180.0f)));
            camera.position = (Vector3){model.modelCenter.x - distance, model.modelCenter.y + distance, model.modelCenter.z - distance};
        };

        // ShortCuts

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R)) {

            model.loadFromCache();
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_U)) {
            model.unload();
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
            clearCache(model.getSource());
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) {
            modelFileDialog();
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_SPACE)) {
            openVertTabWin = !openVertTabWin;
        }

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if(ImGui::BeginMenu("Reload")){

                    //
                    if(ImGui::MenuItem("Recent Model (Ctrl + R)")){
                        model.loadFromCache();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Open"))
                {
                    if(ImGui::MenuItem("Model")){

                        modelFileDialog();
                    }

                    ImGui::EndMenu();
                }

                if(ImGui::BeginMenu("Export")){

                    if(ImGui::MenuItem("*.RESULTS")){

                        model.storeResults();
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings"))
            {
                if(ImGui::BeginMenu("Startup")){

                    //
                    ImGui::Checkbox("Reload Recent on Startup", &g_bindStartUpToReloadRecent);

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("General Apperarance"))
                {

                    RaylibColorEdit(g_backgroundColor,                  "Background");
                    RaylibColorEdit(g_progressDisplayBackgroundColor,   "Progressbar Background");
                    RaylibColorEdit(g_progressDisplayTextColor,         "Progressbar Text");

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("2D Rendering"))
                {
                    // ImGui::NewLine();
                    InputSliderFloat("2D Wireframe Thickness", g_cellsWireFrameThickness2D, 0, 10, true, "_noId", true);
                    InputSliderFloat("2D Node Radius        ", g_cellsNodeRadius2D, 0, 10, true, "_noId", true);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("3D Rendering"))
                {
                    // ImGui::NewLine();
                    InputSliderFloat("3D Wireframe Thickness", g_cellsWireFrameThickness3D, 0, 10, true, "_noId", true);

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Hardware"))
            {
                ImGui::Text("OpenGL Version : %f", g_glVersion);
                ImGui::Text("GPU Vendor : %s", vendor);

                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Temp")){

                if(ImGui::MenuItem("Clear Cache (Strg + C)")){
                    clearCache(model.getSource());
                }

                if(ImGui::MenuItem("Clear Caches")){
                    clearCaches();
                }

                if(ImGui::MenuItem("Clear Program Bin")){
                    clearBin();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Libraries"))
            {
                if (ImGui::BeginTable("Overview", 3)) {

                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("License");
                    ImGui::TableSetupColumn("Link");
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < libaryNames.size(); ++i) {

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", libaryNames[i]);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", libaryLicenses[i]);

                        ImGui::TableSetColumnIndex(2);

                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 102, 204, 255));
                        if (ImGui::SmallButton(libaryUrls[i])) {
                            OpenLink(libaryUrls[i]);
                        }
                        ImGui::PopStyleColor();
                    }

                    ImGui::EndTable();
                }

                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Github")){

                if (ImGui::BeginTable("ProjectLinks", 2)) {

                    ImGui::TableSetupColumn("Subject");
                    ImGui::TableSetupColumn("Link");
                    ImGui::TableHeadersRow();

                    auto LinkRow = [](const char* label, const char* url) {

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(label);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 102, 204, 255)); // Linkfarbe
                        if (ImGui::SmallButton(url)) {
                            OpenLink(url);
                        }
                        ImGui::PopStyleColor();
                    };

                    LinkRow("GitHub Repository", githubRepositoryUrl.c_str());
                    LinkRow("Releases", (githubRepositoryUrl + "/releases").c_str());
                    LinkRow("Find out more", (githubRepositoryUrl+ "/blob/main/README.md").c_str());
                    LinkRow("Find out even more (read Thetis)", (githubRepositoryUrl+"/blob/main/Recc/Thetis/Studienarbeit.pdf").c_str());

                    ImGui::EndTable();
                }
                ImGui::EndMenu();
            }

            float buttonWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            float rightEdge = ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().FramePadding.x;
        
            ImGui::SetCursorPosX(rightEdge);
            if (ImGui::Button("X")) {
                closeWindow = true;
            }

            ImGui::EndMainMenuBar();
        }

        static int selectedTab = 0;
        static std::map<int, int> selectedSubTab; // selectedSubTab[tabID] = subTabID

        size_t tabNameSize = 3;
        const char* tabNames[] = { "Mesh", "Material", "Rendering" };
        std::map<int, std::vector<const char*>> subTabNames = {
            {0, {"overview"}},
            {1, {"std_params", "sampling", "pdf", "pdf_params", "pdf_settings"}},
            {2, {"MeshDisplay", "Animation", "pdf"}}
        };

        // ShortCuts
        if (IsKeyPressed(KEY_N) && !IsKeyDown(KEY_LEFT_CONTROL)) {

            openVertTabWin = true;
            selectedTab = 0;
        }

        if (IsKeyPressed(KEY_M) && !IsKeyDown(KEY_LEFT_CONTROL)) {

            openVertTabWin = true;
            selectedTab = 1;
        }

        if (IsKeyPressed(KEY_R) && !IsKeyDown(KEY_LEFT_CONTROL)) {

            openVertTabWin = true;
            selectedTab = 2;
        }

        if (IsKeyPressed(KEY_DOWN) && !fpsCam && !ImGui::IsAnyItemActive() && !ImGui::IsAnyItemFocused()) {

            openVertTabWin = true;

            if(selectedSubTab[selectedTab] >= subTabNames[selectedTab].size() - 1){
                
                if(selectedTab >= tabNameSize - 1){
                    selectedTab = 0;
                }
                else{
                    selectedTab += 1;
                }
                
                selectedSubTab[selectedTab] = 0;

                // Für Navgation in subtabs
                // selectedSubTab[selectedTab] = 0;
            }
            else{
                selectedSubTab[selectedTab] += 1;
            }
        }

        if (IsKeyPressed(KEY_UP) && !fpsCam && !ImGui::IsAnyItemActive() && !ImGui::IsAnyItemFocused()) {

            openVertTabWin = true;

            if(selectedSubTab[selectedTab] <= 0){
                
                if(selectedTab <= 0){
                    selectedTab = tabNameSize - 1;
                }
                else{
                    selectedTab -= 1;
                }

                selectedSubTab[selectedTab] = subTabNames[selectedTab].size() - 1;
                
                // Für Navigation in subtabs
                // selectedSubTab[selectedTab] = subTabNames[selectedTab].size() - 1;
            }
            else{
                selectedSubTab[selectedTab] -= 1;
            }
        }

        if((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) && !ImGui::IsAnyItemActive() && !ImGui::IsAnyItemFocused() && !fpsCam){
            openVertTabWin = !openVertTabWin;
        }

        if(!openVertTabWin){

            // Fenster an rechter Seite über volle Höhe
            ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize({ImGui::CalcTextSize("<-").x + ImGui::GetStyle().FramePadding.x * 4.0f,
                                     winSize.y - ImGui::GetFrameHeight()}, ImGuiCond_Always);
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

            ImGui::Begin(("Collapsed Edit Model Win" + modelSource).c_str(), &openVertTabWin, flags);

            //ImGui::SetCursorPosY(winSize.y/2 + ImGui::GetFrameHeight()/2);

            if(ImGui::Button("<-")){
                openVertTabWin = true;
            }

            ImGui::End();
        }
        else{

            // Fenster an rechter Seite über volle Höhe
            ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

            ImGui::Begin(("Edit Model " + modelSource).c_str(), &openVertTabWin, flags);

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

            ImGui::SameLine();

            float ButtonWidth = ImGui::CalcTextSize("->").x + ImGui::GetStyle().FramePadding.x * 4;
            ImGui::SetNextItemWidth(ButtonWidth);

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ButtonWidth - ImGui::GetStyle().FramePadding.x * 2);

            if (ImGui::Button("->")) {
                openVertTabWin = !openVertTabWin;
                ImGui::SetWindowCollapsed(openVertTabWin, ImGuiCond_Always);
            }

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

            //
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
            case 0:{

                switch (selectedSubTab[selectedTab]){

                    case 0:{
                        if(model.initialzed()){

                            ImGui::PushTextWrapPos();
                            ImGui::Text("Model at : '%s'", relPath(model.m_modelPath).c_str());
                            ImGui::PopTextWrapPos();

                            ImGui::Separator();
                            ImGui::Text("Dimension : %zuD", model.getMesh().nDimensions);
                            ImGui::Text("Nodes : %zu", model.getMesh().getUndeformedNodes().size());
                            ImGui::Text("Cells : %zu", model.getMesh().getCells().size());
                            ImGui::Text("Dofs : %zu", model.getMesh().getUndeformedNodes().size() * model.getMesh().nDimensions);
                            ImGui::Text("uncertainty Quantification : %s", model.getMesh().getMaterial().hasPdf ? "true" : "false");
                            ImGui::Text("linear Simulation : %s", model.getMesh().getMaterial().isLinear ? "true" : "false");
                            ImGui::Separator();
                        }
                        else{

                            ImGui::Text("No Model loaded");
                        }
                        break;
                    }
                }

                break;
            }
            case 1:{

                //
                IsoMeshMaterial& mat = model.getMesh().getMaterial();

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

                        // label anzeigen
                        ImGui::Text("pdf");
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x + ImGui::CalcTextSize("pdf").x - ImGui::CalcTextSize("Import").x);

                        static int currentFile = 0;
                        static std::vector<std::string> pdfFiles = {};
                        static std::vector<const char*> items = {};

                        // Custom Dropdown
                        if (ImGui::Button("Import")) {

                            ImGui::OpenPopup("DateiAuswahl");

                            // zweiteilung hier nötig da const char* entsprechendes objekt braucht um pointer zu erzeugen
                            pdfFiles.clear();
                            for (const auto& file : fs::directory_iterator("../Recc/pdf")) {
                                if (file.path().extension() == "._pdf")
                                    pdfFiles.emplace_back(file.path().string());
                            }

                            //
                            items.clear();
                            for (const auto& file : pdfFiles)
                                items.push_back(file.c_str());
                            
                        }

                        if (ImGui::BeginPopup("DateiAuswahl")) {

                            for (int i = 0; i < pdfFiles.size(); ++i) {
                                if (ImGui::Selectable(fs::path(pdfFiles[i]).filename().string().c_str(), i == currentFile) &&
                                    model.getMesh().getMaterial().isLinear) {

                                    currentFile = i;

                                    LOG << "** Import pdf Funktion " << pdfFiles[currentFile] << endl;

                                    model.getMesh().getMaterial().subs.clear();
                                    model.getMesh().saveMaterial();

                                    std::ifstream pdfFile(pdfFiles[currentFile]);
                                    nlohmann::json pdf = nlohmann::json::parse(pdfFile, nullptr, true, true);
                                    pdfFile.close();

                                    std::ifstream matFile(model.getMesh().getMaterialPath());
                                    nlohmann::json mat = nlohmann::json::parse(matFile, nullptr, true, true);

                                    mat["pdf"]["params"].clear();
                                    mat["pdf"] = pdf;

                                    std::remove(model.getMesh().getMaterialPath().c_str());
                                    std::ofstream matOutFile(model.getMesh().getMaterialPath());
                                    matOutFile << mat.dump(4);
                                    matOutFile.close();

                                    model.getMesh().loadIsoMeshMaterial();
                                    model.sampling();
                                }
                            }

                            if(ImGui::Selectable("ClearPdf")){
                                
                                model.getMesh().getMaterial().pdf = NULL_EXPR;
                                model.getMesh().getMaterial().subs.clear();
                                model.getMesh().saveMaterial();

                                model.getMesh().loadIsoMeshMaterial();
                                model.sampling();
                            }

                            ImGui::EndPopup();
                        }

                        InputExpression("pdf", mat.pdf);

                        ImGui::SameLine();

                        if(ImGui::Button("substitute") && mat.isLinear && mat.hasPdf){
                            
                            // Cachen amit man beim probieren nicht permanent zurückgesetzt wird
                            Expression temp = mat.pdf;
                            mat.substitutePdf();
                            mat.pdf = temp;
                        }

                        ImGui::SameLine();

                        if(ImGui::Button("sample") && mat.isLinear && mat.hasPdf){
                            
                            mat.substitutePdf();
                            model.sampling();
                        }

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

                        ImGui::Text("Handle Params");

                        static bool addParamDialog = false;
                        static bool deleteParamDialog = false;

                        if(!addParamDialog && !deleteParamDialog){

                            if(ImGui::Button("Add")){
                                addParamDialog = true;
                            }
    
                            ImGui::SameLine();
                            if(ImGui::Button("Delete")){
                                deleteParamDialog = true;
                            }
                        }

                        if(addParamDialog){

                            static char textBuffer[24];
                            static float valBuffer = 0.0f;

                            ImGui::Text("Add Param");
                            ImGui::Separator();

                            ImGui::Text("Param ");
                            ImGui::SameLine();
                            if(ImGui::InputText("##addParamLabel",textBuffer, ImGuiInputTextFlags_EnterReturnsTrue)){
                                //addParamDialog = false;
                            }

                            ImGui::Text("Value ");
                            ImGui::SameLine();
                            if(ImGui::InputFloat("##addParamValue", &valBuffer, 0, ImGuiInputTextFlags_EnterReturnsTrue)){
                                //addParamDialog = false;
                            }

                            ImGui::Separator();

                            if(ImGui::Button("Apply") || IsKeyPressed(KEY_ENTER)){
                            
                                Symbol sym = SymEngine::symbol(textBuffer);
                                
                                if(!mat.subs.contains(sym) && sym->__str__() != ""){
                                    mat.subs.try_emplace(SymEngine::symbol(textBuffer), toExpression(valBuffer));
                                }

                                valBuffer = 0;
                                memset(textBuffer, 0, sizeof(textBuffer));
                            }

                            ImGui::Separator();

                            if(ImGui::Button("close Dialog") || IsKeyPressed(KEY_Q)){
                                addParamDialog = false;
                            }
                        }

                        if(deleteParamDialog){

                            static char textBuffer[24];

                            ImGui::Text("Delete Param");
                            ImGui::Separator();
                            
                            for(const auto& [symbol, val] : mat.subs){

                                ImGui::Bullet();
                                ImGui::SameLine();

                                if(ImGui::Button((symbol->__str__()).c_str())){

                                    // compare gibt null für gleichheit und 1,-1 für < || > zurück
                                    // dementsprechend muss der return null sein damit pdf nicht von 
                                    // dem zu entfernenden Symbol abhängig ist
                                    if(mat.pdf->compare(*mat.pdf->subs({{symbol,val}})) != 0){

                                        _ERROR << "   für pdf Expression relevantes Symbol kann nicht entfernt werden, bitte zuerst pdf als von " << symbol <<
                                        " unabhängig deklarieren" << endl;
                                    } else {

                                        // wenn nicht abhängig rauslöschen
                                        mat.subs.erase(symbol);
                                    }
                                }
                            }

                            ImGui::Separator();

                            if(ImGui::Button("Close Dialog") || IsKeyPressed(KEY_Q)){
                                deleteParamDialog = false;
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

                // damit bei fps führung beim hochbewegen nicht jedes mal resampled wird
                if (!fpsCam && IsKeyPressed(KEY_SPACE) && !IsKeyDown(KEY_LEFT_CONTROL)) {

                    // nur ausführen wenn kein imgui element gerade im fokus ist
                    if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyItemFocused() && mat.isLinear && mat.hasPdf) {
                        mat.substitutePdf();
                        model.sampling();
                    }
                }

                break;
            }
            case 2:{
                
                switch (selectedSubTab[selectedTab]){
                    case 0:{

                        std::string camPerspective = "Camera : ";
                        if(fpsCam){camPerspective += "FPS";}
                        else if(orbitCam){camPerspective += "ORBITAL";}
                        else if(planarCam){camPerspective += "NORMAL PLANAR";}
                        else{camPerspective += "NONE";}

                        ImGui::Text(camPerspective.c_str());
                        ImGui::Separator();

                        ImGui::Text("Edit MeshColors");
                        ImGui::Separator();

                        RaylibColorEdit(undeformedFrame, "\tundeformed Mesh");
                        RaylibColorEdit(deformedFrame, "\tdeformed Mesh");
                        RaylibColorEdit(deformedFramePlusXi, "\tdeformed Mesh plus xi");
                        RaylibColorEdit(deformedFrameMinusXi, "\tdeformed Mesh minus xi");

                        ImGui::Separator();

                        ImGui::Text("Edit Datadisplay");
                        ImGui::Separator();

                        static const char* meshItems[] = { "undeformed Mesh", "deformed Mesh", "deformed Mesh plus xi", "deformed Mesh minus xi" };

                        {
                            static int selectedIndex = 0;

                            ImGui::PushItemWidth(200);
                            if (ImGui::BeginCombo("##DM", "Display on Mesh"))
                            {
                                for (int i = 0; i < IM_ARRAYSIZE(meshItems); i++)
                                {
                                    bool isSelected = (selectedIndex == i);
                                    if (ImGui::Selectable(meshItems[i], isSelected))
                                    {
                                        selectedIndex = i;
                                        plotOnMesh = selectedIndex;
                                    }

                                    if (isSelected)
                                        ImGui::SetItemDefaultFocus(); // Für bessere UX
                                }
                                ImGui::EndCombo();
                            }
                        }

                        static const char* plotItems[] = { "Strain", "Stress", "Van-Mises-Stress", "InnerVariable" };
                        static const char* units[] = { "[]", "[N]", "[N]", "[...]" };

                        {
                            
                            static int selectedIndex = 2;
                            
                            if (ImGui::BeginCombo("##DD", "Display Data"))
                            {
                                for (int i = 0; i < IM_ARRAYSIZE(plotItems); i++)
                                {
                                    bool isSelected = (selectedIndex == i);
                                    if (ImGui::Selectable(plotItems[i], isSelected))
                                    {
                                        selectedIndex = i;
                                        plotData = selectedIndex;
                                    }

                                    if (isSelected)
                                        ImGui::SetItemDefaultFocus(); // Für bessere UX
                                }
                                ImGui::EndCombo();
                            }
                        }

                        std::map<int, std::vector<const char*>> koordLabels;

                        {
                            const auto& mat = model.getMesh().getMaterial();

                            size_t totalSize = 1;
                            for (size_t dim : mat.innerVariableSize) {
                                totalSize *= dim;
                            }

                            // static: damit sie zwischen Frames gültig bleiben
                            static std::vector<std::string> indexStrings;
                            static std::vector<const char*> indexLabels;

                            indexStrings.clear();
                            indexLabels.clear();

                            for (size_t i = 0; i < totalSize; ++i) {
                                auto [x,y] = get2DIndexFromLinear(i,
                                    {mat.innerVariableSize.size() > 0 ? mat.innerVariableSize[0] : 0,
                                     mat.innerVariableSize.size() > 1 ? mat.innerVariableSize[1] : 0});

                                indexStrings.push_back("var["+std::to_string(x)+"|"+std::to_string(y)+"]");
                                indexLabels.push_back(indexStrings.back().c_str());
                            }

                            if (model.getMesh().nDimensions == 3) {
                                koordLabels[0] = {"xx", "yy", "zz", "yz", "xz", "xy"};
                                koordLabels[1] = {"xx", "yy", "zz", "yz", "xz", "xy"};
                            } else {
                                koordLabels[0] = {"xx", "yy", "xy"};
                                koordLabels[1] = {"xx", "yy", "xy"};
                            }

                            koordLabels[2] = {"default"};
                            koordLabels[3] = indexLabels;
                            
                            if(plotKoord >= koordLabels[plotData].size()){
                                plotKoord = 0;
                            }

                            const auto& items = koordLabels[plotData];
                            static int selectedIndex = 0;

                            if (ImGui::BeginCombo("##DK", "Display for Koord"))
                            {
                                for (int i = 0; i < items.size(); ++i)
                                {
                                    bool isSelected = (selectedIndex == i);
                                    if (ImGui::Selectable(items[i], isSelected))
                                    {
                                        selectedIndex = i;
                                        plotKoord = selectedIndex;
                                    }

                                    if (isSelected)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                        }

                        //
                        ImGui::Separator();
                        ImGui::Text("Min/Max Values");
                        // ImGui::Separator();

                        // // Anzeige der maximal Werte
                        // ImGui::Text("Mesh : %s \nData : %s \nKoord : %s",
                        //     meshItems[plotOnMesh], plotItems[plotData], koordLabels[plotData][plotKoord]);

                        // ImGui::Text("Unit : %s", units[plotData]);

                        ImGui::Separator();

                        ImGui::Text("min %s [%s] : %.2f %s", plotItems[plotData], koordLabels[plotData][plotKoord], model.minValue, units[plotData]);
                        ImGui::Text("max %s [%s] : %.2f %s", plotItems[plotData], koordLabels[plotData][plotKoord], model.maxValue, units[plotData]);

                        //
                        ImGui::Separator();

                        break;
                    }
                    case 1:{

                        ImGui::Text("Anim");
                        ImGui::SameLine();

                        if(model.animationPaused){

                            if(ImGui::Button("Play")){
                                model.animationPaused = false;
                            }
                        }
                        else {

                            if(ImGui::Button("Pause")){
                                model.animationPaused = true;
                            }
                        }

                        ImGui::SameLine();
                        if(ImGui::Button("Reset")){
                            model.frameCounter = 0;
                        }

                        ImGui::Text("Frame");
                        ImGui::SameLine();

                        static bool wasActiveLastFrame = false;

                        if(ImGui::SliderInt("##frameSlider", &model.frameCounter, 0, model.m_simulationSteps - 1)){

                        }

                        static bool cachedPaused = model.animationPaused;

                        // Erste Berführung des Sliders
                        if (ImGui::IsItemActive() && !wasActiveLastFrame) {
                            cachedPaused = model.animationPaused;
                            model.animationPaused = true;
                        }

                        // nach Loslassen
                        if (ImGui::IsItemDeactivatedAfterEdit()) {
                            model.animationPaused = cachedPaused;
                        }

                        wasActiveLastFrame = ImGui::IsItemActive();

                        ImGui::NewLine();
                        InputSliderFloat("deltaTime", model.m_deltaTime, 0.001, 0.5);

                        break;
                    }
                    case 2:{

                        ImGui::Text("Apperance 2D Models with pdf");
                        ImGui::Separator();

                        ImGui::Checkbox("SplitScreen", &splitScreen);
                        ImGui::Checkbox("SplitVertical", &splitScreenVertical);

                        break;
                    }
                    default:{
                        break;
                    }
                }
            }
            }
            ImGui::EndChild();

            ImGui::End();
        }

        Vector2 legendWinSizeFract = {0.25, 0.25};

        ImGuiWindowFlags lwinFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;

        ImVec2 legendWinPos = ImVec2(0, winSize.y);
        ImVec2 pivot = ImVec2(0.0f, 1.0f);
        ImGui::SetNextWindowPos(legendWinPos, ImGuiCond_Always, pivot);

        // ImGui::Begin("##Legend", nullptr, lwinFlags);

        // ImGui::End();

        RenderFileDialog();

        rlImGuiEnd();

        EndDrawing();

        HandleFileDialog();
    }

    //
    ImPlot::DestroyContext();
    rlImGuiShutdown();
    CloseWindow();

    cacheConfigs();

    return 0;
}