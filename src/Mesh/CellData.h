#pragma once

#include "CellPrefabCache.h"

enum class MeshData : uint8_t{

    DISPLACEMENT,
    STRAIN,     
    STRESS,
    VANMISES_STRESS,
    INNER_VARIABLE,
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
    static float vanMisesXd(const Eigen::MatrixXf& stress);

    // vanMises Stress berechnung für verschieden dimensionale Netze
    static float vanMises2D(const Eigen::MatrixXf& stress);
    static float vanMises3D(const Eigen::MatrixXf& stress);

    CellData();
    CellData(const CellPrefab& prefab);

    void calculateCellStrainAndStress();
    void calculateVanMisesStress();
    float getData(const MeshData& data, int globKoord, int forQuadraturePoint = -1, bool returnAbs = false) const;

    void operator*=(const Coeffs& coeffs);

    void toByteSequence(ByteSequence& seq) const;
    void fromByteSequence(ByteSequence& seq);

    prefabIndex m_prefIdx = 0; 
    Eigen::MatrixXf strain, stress, innerVariable;
    
    float cellVolume = 0.0f;
    Eigen::MatrixXf cellDisplacement;

    float vanMisesStress = 0.0f;
};

typedef std::unordered_map<CellIndex, CellData> DataSet;