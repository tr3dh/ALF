#pragma once

#include "defines.h"

inline SYMBOL(xi);

struct IsoMeshMaterial{

    const static std::string fileSuffix;

    //
    IsoMeshMaterial() = default;

    bool loadFromFile(const std::string& path = NULLSTR);

    void createElasticityTensor(SymEngine::DenseMatrix& target, const size_t& dimension);

    float E = 0,v = 0,t = 0;
    Expression pdf = NULL_EXPR;
};