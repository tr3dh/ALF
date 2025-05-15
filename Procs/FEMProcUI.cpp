// Main file für FEMProc
// deklariert main funktion für executable

#include "defines.h"
#include "femModel/Model.h"
#include "GUI/ImGuiStyleDecls.h"
#include "GUI/ImGuiCustomElements.h"
#include "Rendering/3DRendering.h"

#define MODELCACHE "../bin/.CACHE"

int main(void)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    LOG << "** Source Code " << countLinesInDirectory("../src") << " lines" << endl;
    LOG << "** Procs Code " << countLinesInDirectory("../Procs") << " lines" << endl;
    LOG << endl;

    FemModel model("../Import/3DCubeMesh.model");

    //FemModel model;
    //model.loadFromCache();

    //
    //enableRLLogging();
    SetTraceLogCallback(RaylibLogCallback);

    InitWindow(600, 600, "<><FEMProc><>");

    //
    const char* glVersion = (const char*)glGetString(GL_VERSION);
    LOG << "** OpenGL Version: " << glVersion << endl;

    // string splitten da im glVersion String noch Infos über die graphikkarte stehen
    g_glVersion = string::convert<float>(std::string(glVersion).substr(0,3));

    // ab der 4.3 enthält opengl eine shader pipeline für comp shaders
    g_ComputeShaderBackendEnabled = g_glVersion >= 4.3f;

    LOG << (g_ComputeShaderBackendEnabled ? "** Computeshader Backend freigeschaltet" :
        "** Computeshader Backend gesperrt, opengl version " + std::to_string(g_glVersion).substr(0,3) + " ist nicht mit comp shadern kompatibel, erforderliche Version : OpenGL 4.3") << endl;

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

    //const NodeSet& nodes = model.getMesh().getUndeformedNodes();
    const NodeSet& nodes = model.getMesh().getDeformedNodes();
    const CellSet& cells = model.getMesh().getCells();

    // player Camera
    Camera camera = {
        .position = {0,2.5,0},
        .target = {10, 2.5,0},
        .up = {0,1,0},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE    // oder CAMERA_ORTHOGRAPHIC
    };

    // 1. Bounding Box berechnen
    Vector3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
    Vector3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    for (const auto& node : nodes) {
        const auto& pos = node.second;
        Vector3 v = {pos[0], pos[1], pos[2]};
        if (v.x < min.x) min.x = v.x;
        if (v.y < min.y) min.y = v.y;
        if (v.z < min.z) min.z = v.z;
        if (v.x > max.x) max.x = v.x;
        if (v.y > max.y) max.y = v.y;
        if (v.z > max.z) max.z = v.z;
    }

    Vector3 center = {
        (min.x + max.x) * 0.5f,
        (min.z + max.z) * 0.5f,
        (min.y + max.y) * 0.5f
    };
    float extentX = max.x - min.x;
    float extentY = max.z - min.z;
    float extentZ = max.y - min.y;
    float maxExtent = fmaxf(fmaxf(extentX, extentY), extentZ);

    camera.target = center;

    float distance = maxExtent / (2.0f * tanf(camera.fovy * 0.5f * (PI/180.0f)));
    camera.position = (Vector3){center.x - distance, center.y + distance, center.z - distance};

    float movementSensitivity = 15;
    float pitchSensitivity = 0.4;
    float scrollSensitivity = -1000;

    // pre decl des Meshs bzw. Modells
    // im weiteren werden nur noch die Vertices geupdatet
    Mesh mesh = {0};
    mesh.vertexCount = 8;
    mesh.triangleCount = 12;
    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.indices = (unsigned short*)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    // wichtig dass Indices der Vertices im richtigen Dresinn angegeben werden,
    // ansonsten wird Normale falsch herum angenommen und face culling führt zu rendering der
    // Fläche nur bei Blick von element Innerem aus gerendert
    unsigned short cubeIndices[36] = {
        0,1,2, 0,2,3,
        1,6,2, 1,5,6,
        3,6,7, 3,2,6,
        0,7,4, 0,3,7,
        0,5,1, 0,4,5,
        4,6,5, 4,7,6
    };
    memcpy(mesh.indices, cubeIndices, sizeof(cubeIndices));

    std::vector<Vector2> wireFrameIndices = {
        {0,1},{1,2},{2,3},{3,0},    // Vordersete
        {4,5},{5,6},{6,7},{7,4},    // Rückseite 
        {0,4},{1,5},{2,6},{3,7}     // Verbindungen
    };

    // Mesh einmalig hochladen
    UploadMesh(&mesh, false);
    Model cubeModel = LoadModelFromMesh(mesh);

    bool mode3D = false;

    while (!WindowShouldClose()){

        float dt = GetFrameTime();

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            mode3D = true;
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            mode3D = false;
        }

        if(IsKeyPressed(KEY_LEFT_ALT)){
            camera.target = center;
            camera.position = (Vector3){center.x - distance, center.y + distance, center.z - distance};
        }

        // nur wenn nicht ImGui::GetIO().WantCaptureMouse
        if(mode3D || GetMouseWheelMove() != 0){

            Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
            Vector3 localUp = Vector3Normalize(Vector3CrossProduct(right, forward));

            Vector2 mouseDelta = GetMouseDelta();
            float panSpeed = 0.5f;

            Vector3 pan = Vector3Scale(right, -mouseDelta.x * panSpeed);
            pan = Vector3Add(pan, Vector3Scale(localUp, mouseDelta.y * panSpeed));

            float scroll = GetMouseWheelMove();
            float scrollSpeed = 2.0f;
            Vector3 scrollMove = Vector3Scale(forward, scroll * scrollSpeed);

            Vector3 move = Vector3Add(pan, scrollMove);

            camera.position = Vector3Add(camera.position, move);
            camera.target   = Vector3Add(camera.target, move);

            UpdateCameraPro(&camera,
                (Vector3){
                    0,0,0
                },
                (Vector3){0.0f, 0.0f, 0.0f},
                GetMouseWheelMove() * scrollSensitivity * dt
            );
        } else if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)){

            //
            Vector2 centerScreenBefore = GetWorldToScreen(center, camera);

            float rotationSensitivity = 0.01;

            //
            Vector3 preoffset = Vector3Subtract(camera.position, camera.target);
            float azimuth = atan2(preoffset.x, preoffset.z);

            float radius = Vector3Length(Vector3Subtract(camera.position, camera.target));
            float elevation = asin(preoffset.y / radius);

            //
            Vector2 mouseDelta = GetMouseDelta();
            float yaw = -mouseDelta.x * rotationSensitivity;
            float pitch = mouseDelta.y * rotationSensitivity;

            azimuth += yaw;
            elevation += pitch;

            float minElevation = -PI/2 + 0.01f;
            float maxElevation =  PI/2 - 0.01f;
            if (elevation < minElevation) elevation = minElevation;
            if (elevation > maxElevation) elevation = maxElevation;

            //
            Vector3 offset = {
                radius * cosf(elevation) * sinf(azimuth),
                radius * sinf(elevation),
                radius * cosf(elevation) * cosf(azimuth)
            };
            camera.position = Vector3Add(camera.target, offset);
            camera.target = camera.target;

            //
            Vector2 centerScreenAfter = GetWorldToScreen(center, camera);
            
        } else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){

            //
            Vector3 preoffset = Vector3Subtract(camera.position, camera.target);
            float azimuth = atan2(preoffset.x, preoffset.z);

            float radius = Vector3Length(Vector3Subtract(camera.position, camera.target));
            float elevation = asin(preoffset.y / radius);

            //
            float rotationSensitivity = 0.01;
            Vector2 mouseDelta = GetMouseDelta();
            float yaw = -mouseDelta.x * rotationSensitivity;
            float pitch = mouseDelta.y * rotationSensitivity;

            //
            azimuth += yaw;
            elevation += pitch;

            //
            float minElevation = -PI/2 + 0.01f;
            float maxElevation =  PI/2 - 0.01f;
            if (elevation < minElevation) elevation = minElevation;
            if (elevation > maxElevation) elevation = maxElevation;

            //
            Vector3 offset = {
                radius * cosf(elevation) * sinf(azimuth),
                radius * sinf(elevation),
                radius * cosf(elevation) * cosf(azimuth)
            };
            camera.position = Vector3Add(center, offset);
            camera.target = center;
        }

        ClearBackground(Color(30,30,30,255));

        BeginDrawing();
        
        BeginMode3D(camera);

        std::map<CellIndex, float> values = {};
        for (const auto& [cellIndex, cellData] : model.getMesh().getCellData()) {
            values.try_emplace(cellIndex, cellData.getData(MeshData::VANMISES_STRESS, 0));
        }

        float minValue = 0, maxValue = 0;

        if (!values.empty()) {
            // Über die Werte iterieren, nicht die Map-Paare!
            auto [minIt, maxIt] = std::minmax_element(
                values.begin(), values.end(),
                [](const auto& a, const auto& b) { return a.second < b.second; }
            );
            minValue = minIt->second;
            maxValue = maxIt->second;
        }

        for (const auto& [cellIndex, cell] : cells) {

            const CellPrefab& r_pref = cell.getPrefab();

            std::vector<Vector3> cubeVertices;
            for (size_t node = 0; node < cell.getPrefab().nNodes; node++) {
                const auto& Node = nodes.at(cell[node]);
                cubeVertices.push_back((Vector3){Node[0], Node[1], Node[2]});
                DrawSphere((Vector3){Node[0], Node[2], Node[1]},0.05,GREEN);
            }

            // Vertices kopieren
            // tauschen von z und y damit die z Achse als Hochachse gerendert wird
            for (int i = 0; i < 8; i++) {
                mesh.vertices[i*3 + 0] = cubeVertices[i].x;
                mesh.vertices[i*3 + 1] = cubeVertices[i].z;
                mesh.vertices[i*3 + 2] = cubeVertices[i].y;
            }

            // Vertices auf die GPU laden
            UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float)*mesh.vertexCount*3, 0);

            // Rendern
            DrawModel(cubeModel, (Vector3){0,0,0}, 1.0f, getColorByValue(values[cellIndex], minValue, maxValue));
            
            //
            for(const auto& dir : wireFrameIndices){

                const auto& startNode = nodes.at(cell[dir.x]);
                const auto& endNode = nodes.at(cell[dir.y]);

                DrawCylinderEx((Vector3){startNode[0],startNode[2],startNode[1]},
                                (Vector3){endNode[0],endNode[2],endNode[1]}, 0.2, 0.2, 8, BLACK);
            }
        }

        EndMode3D();

        EndDrawing();

    }

    //
    bool closeWindow = false;
    while (!closeWindow)
    {
        bool currentCtrlState = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        static std::string modelSource = fs::path(model.getSource()).filename().string();
        
        int monitor = GetCurrentMonitor();
        Vector2 monitorSize = {GetMonitorWidth(monitor), GetMonitorHeight(monitor)};
        Vector2 winSize = {GetScreenWidth(), GetScreenHeight()};

        // Nur bei gedrückter Strg Taste reagieren
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
        static bool splitScreen = false;
        static bool splitScreenVertical = true;
        model.display(MeshData::VANMISES_STRESS, 0, false, false, displayFrame, displayFrameCenter, {100,100}, splitScreen, splitScreenVertical);

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

            float buttonWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            float rightEdge = ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().FramePadding.x;
        
            ImGui::SetCursorPosX(rightEdge);
            if (ImGui::Button("X")) {
                closeWindow = true;
            }

            ImGui::EndMainMenuBar();
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

            static int selectedTab = 3;
            static std::map<int, int> selectedSubTab; // selectedSubTab[tabID] = subTabID

            const char* tabNames[] = { "Model", "Constraints", "Isomparam", "Material" };
            std::map<int, std::vector<const char*>> subTabNames = {
                {0, {"Import", "Export", "Settings"}},
                {1, {}},
                {2, {}},
                {3, {"std_params", "sampling", "pdf", "pdf_params", "pdf_settings", "rendering"}}
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
                                if (ImGui::Selectable(fs::path(pdfFiles[i]).filename().string().c_str(), i == currentFile)) {

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
                                }
                            }
                            ImGui::EndPopup();
                        }

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
                    case 5:{

                        ImGui::Text("Appearance");
                        ImGui::Separator();

                        ImGui::Checkbox("SplitScreen", &splitScreen);
                        ImGui::Checkbox("SplitVertical", &splitScreenVertical);

                        break;
                    }
                    default:{

                        break;
                    }   
                }

                if (IsKeyPressed(KEY_SPACE)) {

                    // nur ausführen wenn kein imgui element gerade im fokus ist
                    if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyItemFocused()) {
                        mat.substitutePdf();
                        model.sampling();
                    }
                }

                break;
            }
            }
            ImGui::EndChild();

            ImGui::End();

        }

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