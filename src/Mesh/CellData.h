#pragma once

#include "CellPrefabCache.h"

enum class MeshData : uint8_t{

    DISPLACEMENT,
    STRAIN,     
    STRESS,
    VANMISES_STRESS,
    NONE,
};

struct CellData{

    static CellData nullRef;

    CellData(const CellPrefab& prefab) : m_prefab(prefab){
       
        cellDisplacement = Eigen::MatrixXd(m_prefab.nNodes * m_prefab.nDimensions, 1);
        cellDisplacement.setZero();

        strain = Eigen::MatrixXd(m_prefab.nDimensions * (m_prefab.nDimensions + 1)/2, 1);
        strain.setZero();

        stress = Eigen::MatrixXd(m_prefab.nDimensions * (m_prefab.nDimensions + 1)/2,1);
        stress.setZero();

        quadratureStrain.reserve(m_prefab.nNodes);
        quadratureStress.reserve(m_prefab.nNodes);
    };

    void calculateCellStrainAndStress(){

        // 1, 2, 3, 4, ...
        for(int nodeNum = 0; nodeNum < m_prefab.nNodes; nodeNum++){

            strain += quadratureStrain[nodeNum];
            stress += quadratureStress[nodeNum];
        }

        strain*=(1/cellVolume);
        stress*=(1/cellVolume);

        calculateVanMisesStress();
    }

    void calculateVanMisesStress(){

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

    float getData(const MeshData& data, int globKoord, int forQuadraturePoint = -1, bool returnAbs = false) const{

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

    const CellPrefab& m_prefab;

    Eigen::MatrixXd strain, stress;
    std::vector<Eigen::MatrixXd> quadratureStrain = {}, quadratureStress = {};
    
    float cellVolume = 0.0f;
    Eigen::MatrixXd cellDisplacement;

    float vanMisesStress = 0.0f;
    std::vector<float> quadratureMisesStress = {};
};