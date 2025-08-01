#pragma once

#include "defines.h"
#include "Mesh/Cell.h"

extern float g_cellsWireFrameThickness3D;

static Mesh g_cellMesh;
static Model g_cellModel;

static prefabIndex g_cellMesh_pID = 0;

static std::vector<Vector2> g_cellWireFrameIndices = {};

void freeCellRendererMem();

void initCellRenderer(const CellPrefab& pref);

void renderCell(const Cell& cell, const NodeSet& nodes, const Color& color);

void renderCellsWireFrame(const Cell& cell, const NodeSet& nodes, const Color& color);