#pragma once

#include "Node.h"

class CellPrefab{

public:

    static CellPrefab nullRef;

    CellPrefab();
    CellPrefab(const std::string& path);

    std::string label = NULLSTR;
    uint8_t pID = 0, nDimensions = 0, nNodes = 0;

    std::vector<Expression> shapeFunctions = {};
    std::vector<SymEngine::DenseMatrix> shapeFunctionDerivatives = {};

    std::vector<SymEngine::map_basic_basic> quadraturePoints = {};
    std::vector<float> weights = {};

    // nur für 3D
    int numFaces = 0;
    std::vector<unsigned short> faceIndices = {};
    std::vector<Vector2> wireFrameIndices = {};
};