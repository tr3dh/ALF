#include "Mesh.h"

void IsoMesh::displaceNodes(NodeSet& nodes, const Eigen::SparseMatrix<float>& displacement, size_t nodeNumOffset){

    // size gibt hier rows * cols zurück
    size_t dimension = nodes.begin()->second.getDimension();
    if((size_t)displacement.size() != dimension * nodes.size()){
        
        ASSERT(TRIGGER_ASSERT, "übergebene Nodes und Displacement sind inkompatibel");
        return;
    }

    for(const auto& [index, node] : nodes){

        for(size_t coord = 0; coord < dimension; coord++){
            nodes[index][coord] += displacement.coeff((index-nodeNumOffset) * dimension + coord,0);
        }
    }
}

void IsoMesh::solve(){

    // cholesky solver für dünnbestze/positiv semi definite Matritzen
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
    
    solver.compute(m_kSystem);
    m_uSystem = solver.solve(m_fSystem);

    std::sort(m_indicesToRemove.begin(), m_indicesToRemove.end());
    addSparseRow(m_uSystem, m_indicesToRemove);

    m_defNodes = m_nodes;
    displaceNodes(m_defNodes, m_uSystem, nodeNumOffset);
}

//
void IsoMesh::calculateStrainAndStress(){

    LOG << "-- Calculate Strain and Stress" << endl;

    CMatrix = Eigen::MatrixXd(nDimensions + 1, nDimensions + 1);
    subMatrix(SymCMatrix,CMatrix,{});

    Eigen::MatrixXd BMatrix(nDimensions + 1, nDimensions * m_Cells.begin()->second.getPrefab().nNodes);
    float jDet = 0.0f;

    //
    m_cellData.reserve(m_Cells.size());

    //
    int nodeNum = 0;
    for(const auto& [cellIndex, cell] : m_Cells){

        m_cellData.try_emplace(cellIndex, cell.getPrefab());

        CellData& r_cellData = m_cellData.at(cellIndex);
        const CellPrefab& r_prefab = m_Cells[cellIndex].getPrefab();

        // 1, 2, 3, 4, ...
        for(nodeNum = 0; nodeNum < r_prefab.nNodes; nodeNum++){

            // x, y, z, ...
            for(const auto& globKoord : globKoords){
                
                r_cellData.cellDisplacement(nodeNum * nDimensions + globKoord, 0) = m_uSystem.coeffRef((cell[nodeNum] - nodeNumOffset) * nDimensions + globKoord, 0);
            }
        }

        // 1, 2, 3, 4, ...
        for(nodeNum = 0; nodeNum < r_prefab.nNodes; nodeNum++){

            subMatrix(m_cachedBMats[cellIndex], BMatrix, r_prefab.quadraturePoints[nodeNum]);
            jDet = SymEngine::eval_double(*m_cachedJDets[cellIndex]->subs(r_prefab.quadraturePoints[nodeNum]));

            r_cellData.quadratureStrain.emplace_back(BMatrix * r_cellData.cellDisplacement * jDet * r_prefab.weights[nodeNum]);
            r_cellData.quadratureStress.emplace_back(CMatrix * BMatrix * r_cellData.cellDisplacement * jDet * r_prefab.weights[nodeNum]);

            r_cellData.cellVolume += jDet * r_prefab.weights[nodeNum];
        }

        r_cellData.calculateCellStrainAndStress();
    }
}