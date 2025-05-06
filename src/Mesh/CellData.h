#pragma once

#include "CellPrefabCache.h"

enum class MeshData : uint8_t{

    DISPLACEMENT,
    STRAIN,     
    STRESS,
    VANMISES_STRESS,
    NONE,
};

struct Coeffs {
    float displacementCoeff;
    float strainCoeff;
    float stressCoeff;
};

struct CellData{

    static CellData nullRef;

    CellData(const CellPrefab& prefab);

    CellData(const CellData& other);
    CellData& operator=(const CellData& other);

    void calculateCellStrainAndStress();

    void calculateVanMisesStress();

    float getData(const MeshData& data, int globKoord, int forQuadraturePoint = -1, bool returnAbs = false) const;

    const CellPrefab& m_prefab;

    Eigen::MatrixXd strain, stress;
    std::vector<Eigen::MatrixXd> quadratureStrain = {}, quadratureStress = {};
    
    float cellVolume = 0.0f;
    Eigen::MatrixXd cellDisplacement;

    float vanMisesStress = 0.0f;
    std::vector<float> quadratureMisesStress = {};

    void operator*=(const Coeffs& coeffs);
};