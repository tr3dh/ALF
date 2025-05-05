#include "defines.h"
#include "Rendering/3DRendering.h"
#include <chrono>
#include <thread>
#include "femModel/Model.h"

enum class MODEL : uint8_t{

    DEFAULT_BLOCK,
    DIRT_BLOCK,
    SUN_SPHERE,
    SKY_BOX,
    NONE,
};

struct rModel{

    rModel() = default;
    rModel(const std::string& texturePath, Shader& shader){

        // for (int i = 0; i < m_model.materialCount; i++) {
        //     m_model.materials[i].shader = shader;
        //     m_model.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = m_texture;
        // }
    }

    void genSphere(Shader& shader, const std::string& texturePath = NULLSTR){

        m_mesh = GenMeshSphere(2.0f, 100,100);
        m_model = LoadModelFromMesh(m_mesh);

        m_model.materials[0].shader = shader;

        if(texturePath != NULLSTR){

            m_texture = LoadTexture(texturePath.c_str());
            m_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_texture;
        }
    }

    void genCube(Shader& shader, const std::string& texturePath = NULLSTR){

        m_mesh = GenMeshCube(2.0f, 2.0f, 2.0f);
        m_model = LoadModelFromMesh(m_mesh);

        m_model.materials[0].shader = shader;

        if(texturePath != NULLSTR){

            m_texture = LoadTexture(texturePath.c_str());
            m_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_texture;
        }
    }

    rModel(const std::string& filePath, const std::string& texturePath, Shader& shader){
        LOG << "nicht implementiert" << endl;
    }

    ~rModel(){

        UnloadMesh(m_mesh);
        UnloadModel(m_model);
        UnloadTexture(m_texture);
    }

    // phong beleuchtung
    float ambientStr     = 0.5f;
    float specularStr    = 0.5f;
    int shininess        = 256;

    // 
    Vector4 materialColor = { 1.0f, 0.5f, 0.31f, 1.0f };
    int useTexture = false;

    //
    Texture2D m_texture;
    Mesh m_mesh;
    Model m_model;
};

struct objekt{

    objekt(const MODEL& model, const Vector3& position) : m_model(model), m_position(position){

    }

    Vector3 m_position;
    MODEL m_model;
};

class Scene{

public:

    unsigned int sunId = 0;

    Scene(){

        m_phongShader = LoadShader("../shader/lighting.vs", "../shader/lighting.fs");

        sunId = emplaceObjekt(MODEL::SUN_SPHERE, {20,20,20});

        // float x_lower = -10, x_upper = 10;
        // float y_lower = -10, y_upper = 10;
        // for(float x = x_lower; x < x_upper; x+=2){
        //     for(float y = y_lower; y < y_upper; y+=2){

        //         emplaceObjekt(MODEL::DIRT_BLOCK, {x,-1,y});
        //     }
        // }
    }

    void loadModels(){

        MODEL cmod;
        std::string cmodLabel;

        for(uint8_t i = 0; i < static_cast<uint8_t>(MODEL::NONE);i++){

            cmod = static_cast<MODEL>(i);
            cmodLabel = magic_enum::enum_name(static_cast<MODEL>(i));

            if(cmodLabel=="DEFAULT_BLOCK"){

                // block laden --> cube model
                m_models.try_emplace(cmod);
                m_models[cmod].genCube(m_phongShader, NULLSTR);
            }
            if(cmodLabel=="SKY_BOX"){

                
            }
            else if(string::endsWith(std::string(cmodLabel),"_BLOCK")){

                // block laden --> cube model
                m_models.try_emplace(cmod);
                m_models[cmod].genCube(m_phongShader, "../Recc/textures/dirt-tile.png");
            }
            else if(string::endsWith(std::string(cmodLabel),"_SPHERE")){

                // block laden --> cube model
                m_models.try_emplace(cmod);
                m_models[cmod].genSphere(m_phongShader, NULLSTR);
            }
        }
    }

    Shader m_phongShader;

    std::map<MODEL, rModel> m_models;

    unsigned int m_objektIDCounter = 0;
    std::map<unsigned int, objekt> m_objekts;

    unsigned int emplaceObjekt(const MODEL& model, const Vector3& position){

        m_objekts.try_emplace(m_objektIDCounter, model, position);
        return m_objektIDCounter++;
    }

    void draw(){

        //
        UpdateCamera(&m_camera, CAMERA_CUSTOM);

        Vector3 viewPos      = m_camera.position;
        Vector3 lightPos     = m_objekts.at(sunId).m_position;
        Vector3 lightColor   = { 1.0f, 1.0f, 1.0f };
        float ambientStr     = 0.5f;
        float specularStr    = 0.5f;
        int shininess        = 256;
        Vector4 materialColor= { 1.0f, 0.5f, 0.31f, 1.0f };
        int useTexture = true;

        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "viewPos"),      &viewPos,      SHADER_UNIFORM_VEC3);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "lightPos"),     &lightPos,     SHADER_UNIFORM_VEC3);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "lightColor"),   &lightColor,   SHADER_UNIFORM_VEC3);

        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "ambientStr"),   &ambientStr,   SHADER_UNIFORM_FLOAT);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "specularStr"),  &specularStr,  SHADER_UNIFORM_FLOAT);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "shininess"),    &shininess,    SHADER_UNIFORM_INT);

        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "useTexture"),   &useTexture,   SHADER_UNIFORM_INT);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "materialColor"),&materialColor,SHADER_UNIFORM_VEC4);

        for(auto& [id, objekt] : m_objekts){

            if(id == sunId){
                continue;
            }

            DrawModel(m_models[objekt.m_model].m_model, objekt.m_position,1.0, WHITE);
        }

        useTexture = false;
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "useTexture"),   &useTexture,   SHADER_UNIFORM_INT);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "viewPos"),      &lightPos,      SHADER_UNIFORM_VEC3);
        SetShaderValue(m_phongShader, GetShaderLocation(m_phongShader, "lightPos"),     &viewPos,     SHADER_UNIFORM_VEC3);

        DrawModel(m_models[MODEL::SUN_SPHERE].m_model, lightPos, 1.0f, RED);
    }

    float movementSensitivity = 15;
    float pitchSensitivity = 51;
    float scrollSensitivity = -1000;

    void updateCamera(){

        float dt = GetFrameTime();

        UpdateCameraPro(&m_camera,
            (Vector3){
                ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * movementSensitivity -
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * movementSensitivity) * dt,

                ((IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * movementSensitivity -
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * movementSensitivity) * dt,

                ((IsKeyDown(KEY_SPACE)) * movementSensitivity -
                (IsKeyDown(KEY_LEFT_SHIFT)) * movementSensitivity) * dt,
            },
            (Vector3){
                GetMouseDelta().x * pitchSensitivity * dt,
                GetMouseDelta().y * pitchSensitivity * dt,
                0.0f
            },
            GetMouseWheelMove() * scrollSensitivity * dt
        );
    }

    // player Camera
    Camera m_camera = {
        .position = {0,2.5,0},
        .target = {10, 2.5,0},
        .up = {0,1,0},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };
};

template <typename T, std::size_t Row, std::size_t Col>
void printArray(const std::array<std::array<T, Col>, Row> &arr)
{
    for (const auto& arow: arr)   // get each array row
    {
        for (const auto& e: arow) // get each element of the row
            std::cout << e << ' ';

        std::cout << '\n';
    }
}

// An alias template for a two-dimensional std::array
template <typename T, std::size_t Row, std::size_t Col>
using Array2d = std::array<std::array<T, Col>, Row>;

//template <typename T, std::size_t Row, std::size_t Col>
// Array2d;

float interpolate(float a0, float a1, float x)
{
    float g; // Gewicht für die Interpolation
    //g = x;                                                // lineare Interpolation; ergibt stetiges, aber nicht differenzierbares Rauschen
    //g = (3.0 - x * 2.0) * x * x;                          // kubische Interpolation mit dem Polynom 3 * x^2 - 2 * x^3
    g = ((x * (x * 6.0 - 15.0) + 10.0) * x * x * x);        // Interpolation mit dem Polynom 6 * x^5 - 15 * x^4 + 10 * x^3
    return (a1 - a0) * g + a0;
}

int main(void)
{
    //
    seedFloatGenerator(0);

    InitWindow(600, 600, "<><3DDemo><>");

    constexpr size_t gridStep = 50;
    constexpr int imageSize = 100;
    constexpr size_t gridSize = (imageSize / gridStep) + 1;
    Array2d<float, gridSize, gridSize> grid;

    for(int x = 0; x < gridSize; x++){
        for(int y = 0; y < gridSize; y++){
            grid[x][y] = randFloat(0, 2*PI);
        }
    }

    Image img1 = GenImageColor(imageSize, imageSize, WHITE);
    Color *pixels = LoadImageColors(img1);

    auto fade = [](float t) { return t * t * (3 - 2 * t); };
    auto lerp = [](float a, float b, float t) { return a + t * (b - a); };

    for (int y = 0; y < img1.height; y++) {
        for (int x = 0; x < img1.width; x++) {
            int index = y * img1.width + x;

            int gridX = x / gridStep;
            int gridY = y / gridStep;

            // Gradient-Vektoren
            Vector2 g00 = Vector2{cos(grid[gridX][gridY]), sin(grid[gridX][gridY])};
            Vector2 g10 = Vector2{cos(grid[gridX+1][gridY]), sin(grid[gridX+1][gridY])};
            Vector2 g01 = Vector2{cos(grid[gridX][gridY+1]), sin(grid[gridX][gridY+1])};
            Vector2 g11 = Vector2{cos(grid[gridX+1][gridY+1]), sin(grid[gridX+1][gridY+1])};

            // Offset-Vektoren
            float fx = (x - gridX * gridStep) / (float)gridStep;
            float fy = (y - gridY * gridStep) / (float)gridStep;

            Vector2 d00 = Vector2{fx, fy};
            Vector2 d10 = Vector2{fx - 1, fy};
            Vector2 d01 = Vector2{fx, fy - 1};
            Vector2 d11 = Vector2{fx - 1, fy - 1};

            // Dot-Produkte
            float s = Vector2DotProduct(g00, d00);
            float t = Vector2DotProduct(g10, d10);
            float u_ = Vector2DotProduct(g01, d01);
            float v_ = Vector2DotProduct(g11, d11);

            // Fade
            float u = fade(fx);
            float v = fade(fy);

            // Interpolation
            float ix0 = lerp(s, t, u);
            float ix1 = lerp(u_, v_, u);
            float w = lerp(ix0, ix1, v);

            // In Grauwert umwandeln
            int gray = (int)((w + 1.0f) * 0.5f * 255.0f);
            gray = Clamp(gray, 0, 255);
            pixels[index] = Color{gray, gray, gray, 255};
        }
    }

    // Die Änderungen ins Image schreiben
    Image newImg = ImageFromImage(img1, (Rectangle){0,0,img1.width,img1.height});
    for (int i = 0; i < img1.width * img1.height; i++) {
        ((Color *)newImg.data)[i] = pixels[i];
    }

    Texture2D tex1 = LoadTextureFromImage(newImg); 

    Image img = GenImagePerlinNoise(100,100,0,0,5);
    Texture2D tex = LoadTextureFromImage(img); 

    Scene scene;
    scene.loadModels();

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

    int hmin = 0, hmax = 40;

    Color *colors = LoadImageColors(newImg);
    for (int y = 0; y < newImg.width; y++) {
        for (int x = 0; x < newImg.height; x++) {
            int index = y * img1.width + x;
            float height = colors[index].r;
            float scaledHeight = ((height/255)-1)*hmax;

            int h = (int)scaledHeight;
            if(h%2 != 0){
                h-=1;
            }

            scene.emplaceObjekt(MODEL::DIRT_BLOCK, {x*2, h,y*2});
        }
    }

    //
    bool closeWindow = false;
    while (!closeWindow)
    {

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

        scene.updateCamera();   
        
        BeginDrawing();
        ClearBackground(Color(0,206,255,255));

        BeginMode3D(scene.m_camera);

        scene.draw();

        EndMode3D();

        // Texture zeichnen (z.B. bei 100,100)
        //DrawTexture(tex, 100, 100, WHITE);
        DrawTexture(tex1, 10,10, WHITE);

        EndDrawing();
    }

    //
    CloseWindow();

    return 0;
}