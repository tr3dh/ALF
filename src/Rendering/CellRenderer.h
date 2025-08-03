// Der gesamte CellRenderer basiert auf Erstellung eines Elementnetzes und der permanenten Ersetzung der Vertex Koordinaten
// und Normalen in Schleife über alle Elemente
// >> für effizienteres und qualitativ hochwertiges Rendering müsste das Modell hier vorab ganz vernetzt werden und
//    und mit Fragment bzw. Vertex Farben initialisiert werden
// >> Wird hier aber nicht benötigt

#pragma once

#include "defines.h"
#include "Mesh/Cell.h"

extern float g_cellsWireFrameThickness3D;

static Mesh g_cellMesh;
static Model g_cellModel;

extern prefabIndex g_cellMesh_pID;

extern Shader g_cellMeshShader;
extern bool g_useCellMeshShader;
extern int g_locationMaterialColor;

static std::vector<Vector2> g_cellWireFrameIndices = {};

void freeCellRendererMem();

void initCellRenderer(const CellPrefab& pref);

void applyShader(const std::string& vsPath, const std::string& fsPath);

void renderCell(const Cell& cell, const NodeSet& nodes, const Color& color);

void renderCellsWireFrame(const Cell& cell, const NodeSet& nodes, const Color& color);