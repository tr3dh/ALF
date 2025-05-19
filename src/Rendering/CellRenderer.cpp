#include "CellRenderer.h"

void freeCellRendererMem(){

    // Freigabe GPU Reccourcen
    UnloadMesh(g_cellMesh);
    UnloadModel(g_cellModel);

    // Freigabe CPU Reccourcen
    MemFree(g_cellMesh.vertices);
    MemFree(g_cellMesh.indices);
}

void initCellRenderer(const CellPrefab& pref){

    // Reccourcen freigeben falls belegt
    if(g_cellMesh_pID == pref.pID){
        
        return;
    }

    //
    LOG << "** Init CellRenderer for CellPref " << pref.label << endl;

    // Reccourcen nur freigeben falls sie schoneinmal belegt worden sind
    g_cellMesh_pID == 0 ? (void)0 : freeCellRendererMem();

    // Init des Meshs
    g_cellMesh = {0};

    // Übergabe mesh stats
    g_cellMesh.vertexCount = pref.nNodes;
    g_cellMesh.triangleCount = pref.numFaces;

    // Speicher Allokierung im Heap
    g_cellMesh.vertices = (float*)MemAlloc(g_cellMesh.vertexCount * 3 * sizeof(float));
    g_cellMesh.indices = (unsigned short*)MemAlloc(g_cellMesh.triangleCount * 3 * sizeof(unsigned short));

    memcpy(g_cellMesh.indices, pref.faceIndices.data(), pref.faceIndices.size() * sizeof(pref.faceIndices[0]));
    g_cellWireFrameIndices = pref.wireFrameIndices;

    UploadMesh(&g_cellMesh, false);
    g_cellModel = LoadModelFromMesh(g_cellMesh);
}

void renderCell(const Cell& cell, const NodeSet& nodes, const Color& color){

    // loop durch Nodes und überschreiben der Mesh Vertices
    for (size_t node = 0; node < cell.getPrefab().nNodes; node++) {
        
        //
        const auto& r_Node = nodes.at(cell[node]);

        // Vertices vertauschen damit hochachse aus der Berechnung z als Hochachse y im Rendering erscheint
        g_cellMesh.vertices[node*3 + 0] = r_Node[0];
        g_cellMesh.vertices[node*3 + 1] = r_Node[2];
        g_cellMesh.vertices[node*3 + 2] = r_Node[1];
    }

    // Vertices auf die GPU laden
    UpdateMeshBuffer(g_cellMesh, 0, g_cellMesh.vertices, sizeof(float)*g_cellMesh.vertexCount*3, 0);

    // Rendern
    DrawModel(g_cellModel, (Vector3){0,0,0}, 1.0f, color);
}

void renderCellsWireFrame(const Cell& cell, const NodeSet& nodes, const Color& color){

    //
    for(const auto& dir : g_cellWireFrameIndices){

        const auto& r_WireStartNode = nodes.at(cell[dir.x]);
        const auto& r_WireEndNode = nodes.at(cell[dir.y]);

        DrawCylinderEx((Vector3){r_WireStartNode[0],r_WireStartNode[2],r_WireStartNode[1]},
                        (Vector3){r_WireEndNode[0],r_WireEndNode[2],r_WireEndNode[1]}, 0.2, 0.2, 8, color);
    }
}