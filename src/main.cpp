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

int cubemain() {

    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    IsoMesh isomesh;
    isomesh.loadFromFile("../Import/2DQuadMesh.model/2DQuadMesh.inp");
    isomesh.loadIsoMeshMaterial();

    if(isomesh.createStiffnessMatrix()){

        isomesh.readBoundaryConditions();

        isomesh.solve();
        isomesh.calculateStrainAndStress();

        //isomesh.display(MeshData::VANMISES_STRESS, 0, false, false, {100,100});
    }

    // roadmap unsicherheitsanalyse

    // für zufallsverteilte variable xi
    int Xi = 0.1;

    DataSet advancedData;
    acvanceDataSet(isomesh.getCellData(), advancedData, {1.0f-Xi,1.0f-Xi,1.0f-Xi*Xi});

    NodeSet n0 = isomesh.getUndeformedNodes();
    Eigen::SparseMatrix<float> u_xi = isomesh.getDisplacement() * (1-Xi);

    IsoMesh::displaceNodes(n0, u_xi, n0.begin()->first);

    //
    std::vector<float> samples = {};

    // Monte Carlo für pdf
    Expression pdf = xi*xi-xi+1;
    rejectionSampling(-pow(xi,2)-xi+1,samples,1000000);

    //
    processSamples(samples);

    //
    LOG << "** FemPROC | total Lines of Code " << countLinesInDirectory("../src") << endl;
    LOG << endl;

    //
    FemModel model;

    //
    SetTraceLogCallback(RaylibLogCallback); 

    // Raylib Fenster init
    const int screenWidth = 1600;
    const int screenHeight = 1200;
    InitWindow(screenWidth, screenHeight, "<><FEMProc><>");
    SetTargetFPS(60);

    Image icon = LoadImage("../Recc/Compilation/icon.png");

    // RGB zu RGBA falls alpha Kanal fehlt
    if (icon.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        LOG << "-- Konvertiere icon.png zu RGBA -> ergänze alpha channel" << endl;
        LOG << endl;
        ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }

    SetWindowIcon(icon);
    UnloadImage(icon);

    //
    ImGui::CreateContext();       // ImGui-Kontext anlegen
    ImPlot::CreateContext();      // ImPlot-Kontext anlegen (nach ImGui!)
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    ImPlot::CreateContext();

    //
    rlImGuiSetup(true);

    //
    SetupImGuiStyle();

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 2.0f, 4.0f, 6.0f }; 
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; 
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Mesh generieren
    Mesh cubeMesh = GenMeshCube(2.0f, 2.0f, 2.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);

    //
    Shader lightingShader = LoadShader("../shader/lighting.vs", "../shader/tex.fs");
    // Shader texShader = LoadShader("../shader/lighting.vs", "../shader/tex.fs");

    //Material mat = LoadMaterialDefault();
    //mat.shader = lightingShader;

    //cubeModel.materials[0] = mat;

    Texture2D texture = LoadTexture("../Recc/Compilation/icon.png");
    cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    cubeModel.materials[0].shader = lightingShader;

    //
    float ambientStr = 0.2f;
    float specularStr = 0.4f;
    int shininess = 1.0;

    Vector4 materialRed = {1.0f, 0.0f, 0.0f, 1.0f};
    Vector4 materialBlack = {0.0f, 0.0f, 0.0f, 1.0f};
    SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "ambientStr"), &ambientStr, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "specularStr"), &specularStr, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "shininess"), &shininess, SHADER_UNIFORM_INT);
    SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "materialColor"), &materialRed, SHADER_UNIFORM_VEC4);

    Vector4 colDiffuseVec = {
        255,
        0,
        0,
        255.0f
    };
    
    int colDiffuseLoc = GetShaderLocation(lightingShader, "colDiffuse");
    SetShaderValue(lightingShader, colDiffuseLoc, &colDiffuseVec, SHADER_UNIFORM_VEC4);

    //
    scaleImguiUI(2);

    //
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            LOG << "SPACE gedrückt" << endl;
        }
        
        if (IsKeyReleased(KEY_SPACE)) {
            LOG << "SPACE losgelassen" << endl;
        }

        UpdateCamera(&camera, CAMERA_ORBITAL);
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };

        BeginDrawing();
        ClearBackground(Color{40,40,40,0});

        BeginMode3D(camera);

        //
        Vector3 lightPos = { 2.0f, 4.0f, 5.0f };
        Vector3 lightColor = { 1.0f, 60.0f, 1.0f };
        
        SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "lightPos"), &lightPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "lightColor"), &lightColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "viewPos"), &camera.position, SHADER_UNIFORM_VEC3);

        // 
        SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "materialColor"), &materialRed, SHADER_UNIFORM_VEC4);
        DrawModel(cubeModel, {0,1,0}, 1.0f, WHITE);

        SetShaderValue(lightingShader, GetShaderLocation(lightingShader, "materialColor"), &materialBlack, SHADER_UNIFORM_VEC4);
        //DrawModelWires(cubeModel, {0,1,0}, 1.0f, BLACK);

        DrawPlane(Vector3Zero(), (Vector2) { 12.0, 12.0 }, BLACK);
        
        DrawGrid(6, 2.0f);

        EndMode3D();

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

        // if (ImPlot::BeginPlot("Mein Histogramm")) {
        //     // samples.data() ist ein Zeiger auf die Daten, samples.size() die Anzahl
        //     // 50 ist die Anzahl der Bins (kannst du anpassen)
        //     //ImPlot::PlotHistogram("Samples", samples.data(), samples.size(), 50);
        //     ImPlot::EndPlot();
        // }

        //
        RenderFileDialog();

        //
        rlImGuiEnd();

        //
        HandleFileDialog();
        
        EndDrawing();
    }

    UnloadModel(cubeModel);

    //
    rlImGuiShutdown();
    CloseWindow();

    //ImPlot::DestroyContext();
    ImGui::DestroyContext();

    return 0;
}


int main(void)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    FemModel model;

    //
    SetTraceLogCallback(RaylibLogCallback); 

    // Raylib Fenster init
    const int screenWidth = 1600;
    const int screenHeight = 1200;
    InitWindow(screenWidth, screenHeight, "<><FEMProc><>");
    SetTargetFPS(60);

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
    scaleImguiUI(2);

    // Haupt-Loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        //
        rlImGuiBegin();

        //
        ImGui::Begin("ImPlot Histogramm");

        // ImPlot-Histogramm
        if (ImPlot::BeginPlot("Histogramm")) {
            ImPlot::PlotHistogram("Samples", model.getSamples().data(), model.getSamples().size(), 50);
            ImPlot::EndPlot();
        }

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

        RenderFileDialog();

        ImGui::End();

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