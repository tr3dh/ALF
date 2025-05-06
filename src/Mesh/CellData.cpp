#include "CellData.h"

CellData::CellData(const CellPrefab& prefab) : m_prefab(prefab){

    cellDisplacement = Eigen::MatrixXd(m_prefab.nNodes * m_prefab.nDimensions, 1);
    cellDisplacement.setZero();

    strain = Eigen::MatrixXd(m_prefab.nDimensions * (m_prefab.nDimensions + 1)/2, 1);
    strain.setZero();

    stress = Eigen::MatrixXd(m_prefab.nDimensions * (m_prefab.nDimensions + 1)/2,1);
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

void CellData::calculateCellStrainAndStress(){

    // 1, 2, 3, 4, ...
    for(int nodeNum = 0; nodeNum < m_prefab.nNodes; nodeNum++){

        strain += quadratureStrain[nodeNum];
        stress += quadratureStress[nodeNum];
    }

    strain*=(1/cellVolume);
    stress*=(1/cellVolume);

    calculateVanMisesStress();
}

void CellData::calculateVanMisesStress(){

    //
    quadratureMisesStress.clear();
    quadratureMisesStress.reserve(m_prefab.nNodes);

    for(size_t nodeNum = 0; nodeNum < m_prefab.nNodes; nodeNum++){

        quadratureMisesStress.emplace_back(
            std::sqrt(std::pow(quadratureStress[nodeNum](0,0),2) + std::pow(quadratureStress[nodeNum](1,0),2) -
            quadratureStress[nodeNum](0,0) * quadratureStress[nodeNum](1,0) + 3 * std::pow(quadratureStress[nodeNum](2,0),2))
        );
    }

    vanMisesStress = std::sqrt(std::pow(stress(0,0),2) + std::pow(stress(1,0),2) - stress(0,0) * stress(1,0) + 3 * std::pow(stress(2,0),2));
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