#pragma once

#include "defines.h"

inline SYMBOL(xi);

struct IsoMeshMaterial{

    const static std::string fileSuffix;

    //
    IsoMeshMaterial() = default;

    bool loadFromFile(const std::string& path = NULLSTR);

    void createElasticityTensor(SymEngine::DenseMatrix& target, const size_t& dimension);

    void substitutePdf();

    void save(const std::string& path);

    float E = 0,v = 0,t = 0;

    Expression pdf = NULL_EXPR, pdf_xi = NULL_EXPR;
    float xi_min = 0, xi_max = 0;
    SymEngine::map_basic_basic subs = {};
    float segmentation = 0, tolerance = 0;
    unsigned int nSamples = 0;
    bool linear = true;
};