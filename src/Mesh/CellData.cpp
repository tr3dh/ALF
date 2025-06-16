#include "CellData.h"

CellData::CellData(const CellPrefab& prefab) : m_prefab(prefab){

    cellDisplacement = Eigen::MatrixXf(m_prefab.nNodes * m_prefab.nDimensions, 1);
    cellDisplacement.setZero();

    strain = Eigen::MatrixXf(m_prefab.nDimensions * (m_prefab.nDimensions + 1)/2, 1);
    strain.setZero();

    stress = Eigen::MatrixXf(m_prefab.nDimensions * (m_prefab.nDimensions + 1)/2,1);
    stress.setZero();

    quadratureStrain.reserve(m_prefab.nNodes);
    quadratureStress.reserve(m_prefab.nNodes);
};

// Copy-Konstruktor
CellData::CellData(const CellData& other)
    : m_prefab(other.m_prefab),
    strain(other.strain),
    stress(other.stress),
    quadratureStrain(other.quadratureStrain),
    quadratureStress(other.quadratureStress),
    cellVolume(other.cellVolume),
    cellDisplacement(other.cellDisplacement),
    vanMisesStress(other.vanMisesStress),
    quadratureMisesStress(other.quadratureMisesStress)
{

}

CellData& CellData::operator=(const CellData& other) {

    if (this != &other) {
        strain = other.strain;
        stress = other.stress;
        quadratureStrain = other.quadratureStrain;
        quadratureStress = other.quadratureStress;
        cellVolume = other.cellVolume;
        cellDisplacement = other.cellDisplacement;
        vanMisesStress = other.vanMisesStress;
        quadratureMisesStress = other.quadratureMisesStress;
    }
    return *this;
}

float CellData::vanMisesXd(const Eigen::MatrixXf& stress){

    switch (stress.size()){
        case 1:{

            return stress(0,0);
            break;
        }
        case 3:{

            return vanMises2D(stress);
            break;
        }
        case 6:{

            return vanMises3D(stress);
            break;
        }
        default:{

            RETURNING_ASSERT(TRIGGER_ASSERT, "übergebene Stress Matrix hat invalide Größe " + std::to_string(stress.size()),(double)0);
            break;
        }
    }
    return (double)0;

}

float CellData::vanMises2D(const Eigen::MatrixXf& stress){

    return std::sqrt(
        std::pow(SIG_XX_2D(stress), 2) +
        std::pow(SIG_YY_2D(stress), 2) -
        SIG_XX_2D(stress) * SIG_YY_2D(stress) +
        3 * std::pow(T_XY_2D(stress), 2)
    );
}

float CellData::vanMises3D(const Eigen::MatrixXf& stress){
        
    return std::sqrt(
        0.5 * (
            std::pow(SIG_XX_3D(stress) - SIG_YY_3D(stress), 2) +
            std::pow(SIG_YY_3D(stress) - SIG_ZZ_3D(stress), 2) +
            std::pow(SIG_ZZ_3D(stress) - SIG_XX_3D(stress), 2)
        ) +
        3.0 * (
            std::pow(T_YX_3D(stress), 2) +
            std::pow(T_ZY_3D(stress), 2) +
            std::pow(T_ZX_3D(stress), 2)
        )
    );
}

void CellData::calculateCellStrainAndStress(){

    // 1, 2, 3, 4, ...
    if(quadratureStrain.size() > 0 && quadratureStress.size() > 0){
        for(int nodeNum = 0; nodeNum < m_prefab.nNodes; nodeNum++){

            strain += quadratureStrain[nodeNum];
            stress += quadratureStress[nodeNum];
        }
    }

    strain*=(1/cellVolume);
    stress*=(1/cellVolume);

    calculateVanMisesStress();
}

void CellData::calculateVanMisesStress(){

    //
    quadratureMisesStress.clear();
    quadratureMisesStress.reserve(m_prefab.nNodes);

    if(quadratureStress.size() > 0){

        for(size_t nodeNum = 0; nodeNum < m_prefab.nNodes; nodeNum++){

            quadratureMisesStress.emplace_back(
                vanMisesXd(quadratureStress[nodeNum]));
        }
    }

    vanMisesStress = vanMisesXd(stress);
}

float CellData::getData(const MeshData& data, int globKoord, int forQuadraturePoint, bool returnAbs) const{

    float nullRef = 0;
    float& fdata = nullRef;

    switch(data){
        case MeshData::STRAIN:{

            if(forQuadraturePoint != -1){
                fdata = quadratureStrain[forQuadraturePoint](globKoord,0);
            }else{
                fdata = strain(globKoord,0);
            }

            break;
        }
        case MeshData::STRESS:{

            if(forQuadraturePoint != -1){
                fdata = quadratureStress[forQuadraturePoint](globKoord,0);
            } else {
                fdata = stress(globKoord,0);
            }

            break;
        }
        case MeshData::VANMISES_STRESS:{

            if(forQuadraturePoint != -1){
                fdata = quadratureMisesStress[forQuadraturePoint];
            } else {
                fdata = vanMisesStress;
            }

            break;
        }
        case MeshData::INNER_VARIABLE:{

            if(forQuadraturePoint != -1){
                // fdata = innerVariableContainer[...];
            } else {
                fdata = innerVariable(globKoord,0);
            }

        }
        default:{
            break;
        }
    }

    return returnAbs ? std::abs(fdata) : fdata;
}

void CellData::operator*=(const Coeffs& coeffs){

    // displacement
    cellDisplacement *= coeffs.displacementCoeff;

    // strain
    for(auto& qstrain : quadratureStrain){
        qstrain *= coeffs.strainCoeff;
    }
    strain *= coeffs.strainCoeff;

    // stress
    for(auto& qstress : quadratureStress){
        qstress *= coeffs.stressCoeff;
    }
    stress *= coeffs.stressCoeff;

    // vanMises
    calculateVanMisesStress();
}