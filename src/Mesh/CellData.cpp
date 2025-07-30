#include "CellData.h"

std::pair<size_t, size_t> get2DIndexFromLinear(size_t linearIndex, const std::vector<size_t>& dims) {
    
    if (dims[0] == 0 && dims[1] == 0) {
        return {0, 0};
    }
    // Zeilen/Spalten Vektoren
    else if (dims[0] == 0 && dims[1] != 0) {
        return {linearIndex, 0};
    }
    else if (dims[0] != 0 && dims[1] == 0) {
        return {0, linearIndex};
    }
    // Matrix
    else {

        size_t major = dims[1];
        size_t row = linearIndex / major;
        size_t col = linearIndex % major;
        return {row, col};
    }
}

CellData::CellData() = default;

CellData::CellData(const CellPrefab& prefab){

    m_prefIdx = prefab.pID;

    cellDisplacement = Eigen::MatrixXf(prefab.nNodes * prefab.nDimensions, 1);
    cellDisplacement.setZero();

    strain = Eigen::MatrixXf(prefab.nDimensions * (prefab.nDimensions + 1)/2, 1);
    strain.setZero();

    stress = Eigen::MatrixXf(prefab.nDimensions * (prefab.nDimensions + 1)/2,1);
    stress.setZero();

    innerVariable = Eigen::MatrixXf(0,0);
};

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

    strain*=(1/cellVolume);
    stress*=(1/cellVolume);

    calculateVanMisesStress();
}

void CellData::calculateVanMisesStress(){

    vanMisesStress = vanMisesXd(stress);
}

float CellData::getData(const MeshData& data, int globKoord, int forQuadraturePoint, bool returnAbs) const{

    float nullRef = 0;
    float& fdata = nullRef;

    switch(data){
        case MeshData::STRAIN:{

            fdata = strain(globKoord,0);

            break;
        }
        case MeshData::STRESS:{

            fdata = stress(globKoord,0);

            break;
        }
        case MeshData::VANMISES_STRESS:{

            fdata = vanMisesStress;
            
            break;
        }
        case MeshData::INNER_VARIABLE:{

            if(innerVariable.size() == 0){
                fdata = 0;
            }
            else {

                auto [x,y] = get2DIndexFromLinear(globKoord, {(size_t)innerVariable.rows(), (size_t)innerVariable.cols()});
                fdata = innerVariable(x,y);
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

    strain *= coeffs.strainCoeff;
    stress *= coeffs.stressCoeff;

    // vanMises
    calculateVanMisesStress();
}

void CellData::toByteSequence(ByteSequence& seq) const {

    seq.insertMultiple(m_prefIdx, stress, strain, innerVariable, cellVolume, cellDisplacement, vanMisesStress);
}

void CellData::fromByteSequence(ByteSequence& seq){

    seq.extractMultiple(vanMisesStress, cellDisplacement, cellVolume, innerVariable, strain, stress, m_prefIdx);
}