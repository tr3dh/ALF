#pragma once

#include "defines.h"

struct IsoMeshMaterial{

    const static std::string fileSuffix;

    //
    IsoMeshMaterial() = default;

    bool loadFromFile(const std::string& path = NULLSTR);

    void createElasticityTensor(SymEngine::DenseMatrix& target, const size_t& dimension);

    float E,v,t;
};