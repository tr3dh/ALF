#include "CellRenderer.h"

float g_cellsWireFrameThickness3D = 0.2f;
prefabIndex g_cellMesh_pID = 0;

Shader g_cellMeshShader = {0};
bool g_useCellMeshShader = false;
int g_locationMaterialColor = -1;

void freeCellRendererMem(){

    //
    if(g_useCellMeshShader){
        
        UnloadShader(g_cellMeshShader);
        g_useCellMeshShader = false;
        g_locationMaterialColor = -1;
    }

    // Freigabe GPU Ressourcen
    // Freigabe CPU Ressourcen (Speicherplatz im Heap)
    //
    // UnloadModel übernimmt hier tatsächlich die Freigabe alles reservierten Netzressourcen,
    // wie mesh.vertices, mesh.indices, etc. und entlädt zudem das Model und das Netz selbst
    UnloadModel(g_cellModel);

    //
    LOG << "++ CellRenderer Unloaded Pref " << +g_cellMesh_pID << ", Memory freed" << ENDL;
}

void initCellRenderer(const CellPrefab& pref){

    //
    LOG << "** Init CellRenderer for CellPref " << pref.label << ENDL;
    LOG << "** Try to unload Pref " << +g_cellMesh_pID << " and free Memory" << ENDL;
    LOG << "** Try to load Pref " << +pref.pID << ENDL;
    LOG << ENDL;

    // Reccourcen nur freigeben falls sie schoneinmal belegt worden sind
    g_cellMesh_pID == 0 ? (void)0 : freeCellRendererMem();

    // Init des Meshs
    g_cellMesh = {0};

    // Übergabe mesh stats
    g_cellMesh.vertexCount = pref.numFaces * 3;
    g_cellMesh.triangleCount = pref.numFaces;

    // Speicher Allokierung im Heap
    g_cellMesh.vertices = (float*)MemAlloc(g_cellMesh.vertexCount * 3 * sizeof(float));
    g_cellMesh.normals = (float*)MemAlloc(g_cellMesh.vertexCount * 3 * sizeof(float));
    g_cellMesh.indices = (unsigned short*)MemAlloc(g_cellMesh.triangleCount * 3 * sizeof(unsigned short));

    // Dummy Indizes
    for (int i = 0; i < g_cellMesh.vertexCount; i++) {
        g_cellMesh.indices[i] = i;
    }

    g_cellWireFrameIndices = pref.wireFrameIndices;

    UploadMesh(&g_cellMesh, false);
    g_cellModel = LoadModelFromMesh(g_cellMesh);

    // Cachen der prefab ID
    g_cellMesh_pID = pref.pID;
}

void applyShader(const std::string& vsPath, const std::string& fsPath){

    //
    g_cellMeshShader = LoadShader(vsPath.c_str(), fsPath.c_str());

    //
    RETURNING_ASSERT(g_cellMeshShader.id > 0, 
        "Shader konnte nicht kompiliert werden, aktiviere im Debug mode das Rayliblogging um mehr zu erfahren",);

    //
    g_cellModel.materials[0].shader = g_cellMeshShader;
    g_useCellMeshShader = true;
    g_locationMaterialColor = GetShaderLocation(g_cellMeshShader, "materialColor");

    //
    LOG << "** Initialized Shader" << ENDL;
    LOG << ENDL;
}

void renderCell(const Cell& cell, const NodeSet& nodes, const Color& color){

    const auto& pref = cell.getPrefab();

    int i0, i1, i2;
    Vector3 v0, v1, v2;
    Vector3 edge1, edge2, normal;
    int baseIdx;

    for (int face = 0; face < pref.numFaces; ++face) {

        // Indices der Vertices des Dreiecks
        i0 = pref.faceIndices[face * 3 + 0];
        i1 = pref.faceIndices[face * 3 + 1];
        i2 = pref.faceIndices[face * 3 + 2];

        // Ortskoordinaten der Nodes mit getauschter y und z Koordinate
        v0 = {nodes.at(cell[i0])[0], nodes.at(cell[i0])[2], nodes.at(cell[i0])[1]};
        v1 = {nodes.at(cell[i1])[0], nodes.at(cell[i1])[2], nodes.at(cell[i1])[1]};
        v2 = {nodes.at(cell[i2])[0], nodes.at(cell[i2])[2], nodes.at(cell[i2])[1]};

        // Normale berechnen
        edge1 = Vector3Subtract(v1, v0);
        edge2 = Vector3Subtract(v2, v0);
        normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

        // Position im Vertex Array [1 ... pref.numFaces * 3 * 3] (3 Punkte pro face, 3 Koordinaten pro Punkt)
        baseIdx = face * 9;

        // Vertices reinschreiben
        float* verts = g_cellMesh.vertices;
        verts[baseIdx + 0] = v0.x; verts[baseIdx + 1] = v0.y; verts[baseIdx + 2] = v0.z;
        verts[baseIdx + 3] = v1.x; verts[baseIdx + 4] = v1.y; verts[baseIdx + 5] = v1.z;
        verts[baseIdx + 6] = v2.x; verts[baseIdx + 7] = v2.y; verts[baseIdx + 8] = v2.z;

        // Normale für alle 3 Vertices gleich
        float* norms = g_cellMesh.normals;
        for (int j = 0; j < 3; ++j) {
            norms[baseIdx + j*3 + 0] = normal.x;
            norms[baseIdx + j*3 + 1] = normal.y;
            norms[baseIdx + j*3 + 2] = normal.z;
        }
    }

    // Vertices und Normalen auf GPU laden
    UpdateMeshBuffer(g_cellMesh, 0, g_cellMesh.vertices, sizeof(float)*g_cellMesh.vertexCount*3, 0);
    UpdateMeshBuffer(g_cellMesh, 2, g_cellMesh.normals, sizeof(float)*g_cellMesh.vertexCount*3, 0);

    //
    if(g_useCellMeshShader && g_cellMeshShader.id > 0 && g_locationMaterialColor > -1){

        //
        SetShaderValue(g_cellMeshShader, g_locationMaterialColor, 
            (Vector4[]){{ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f }}, SHADER_UNIFORM_VEC4);
    }

    // Rendern
    DrawModel(g_cellModel, (Vector3){0,0,0}, 1.0f, color);
}

void renderCellsWireFrame(const Cell& cell, const NodeSet& nodes, const Color& color){

    //
    for(const auto& dir : g_cellWireFrameIndices){

        const auto& r_WireStartNode = nodes.at(cell[dir.x]);
        const auto& r_WireEndNode = nodes.at(cell[dir.y]);

        DrawCylinderEx((Vector3){r_WireStartNode[0],r_WireStartNode[2],r_WireStartNode[1]},
                        (Vector3){r_WireEndNode[0],r_WireEndNode[2],r_WireEndNode[1]}, g_cellsWireFrameThickness3D, g_cellsWireFrameThickness3D, 8, color);
    }
}