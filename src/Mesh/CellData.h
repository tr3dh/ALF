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

// 2D
#define SIG_XX_2D(STRESS) (STRESS)(0,0)
#define SIG_YY_2D(STRESS) (STRESS)(1,0)
#define T_XY_2D(STRESS)   (STRESS)(2,0)

// 3D
#define SIG_XX_3D(STRESS) (STRESS)(0,0)
#define SIG_YY_3D(STRESS) (STRESS)(1,0)
#define SIG_ZZ_3D(STRESS) (STRESS)(2,0)
#define T_ZY_3D(STRESS)   (STRESS)(3,0)
#define T_ZX_3D(STRESS)   (STRESS)(4,0)
#define T_YX_3D(STRESS)   (STRESS)(5,0)

struct CellData{

    static CellData nullRef;

    // Wrapper für die verschieden dimensionalen van-Mises-Spannungs Berechnungen
    // Ermittelt Netzdimension aus stress Matrix und gibt entsprechenden van-Mises-Spannung zurück 
    static float vanMisesXd(const Eigen::MatrixXd& stress);

    // vanMises Stress berechnung für verschieden dimensionale Netze
    static float vanMises2D(const Eigen::MatrixXd& stress);
    static float vanMises3D(const Eigen::MatrixXd& stress);

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

typedef std::unordered_map<CellIndex, CellData> DataSet;