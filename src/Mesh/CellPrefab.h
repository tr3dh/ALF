#pragma once

#include "Node.h"

class CellPrefab{

public:

    static CellPrefab nullRef;

    CellPrefab();
    CellPrefab(const std::string& path);

    std::string label = NULLSTR;
    uint8_t nDimensions = 0, nNodes = 0;

    std::vector<Expression> shapeFunctions = {};
    std::vector<SymEngine::DenseMatrix> shapeFunctionDerivatives = {};

    std::vector<SymEngine::map_basic_basic> quadraturePoints = {};
    std::vector<float> weights = {};
};