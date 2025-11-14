#include "Mesh.h"

void IsoMesh::displaceNodes(NodeSet& nodes, const Eigen::MatrixXf& displacement, size_t nodeNumOffset){

    // size gibt hier rows * cols zurück
    size_t dimension = nodes.begin()->second.getDimension();
    if((size_t)displacement.size() != dimension * nodes.size()){
        
        ASSERT(TRIGGER_ASSERT, "übergebene Nodes und Displacement sind inkompatibel");
        return;
    }

    for(const auto& [index, node] : nodes){

        for(size_t coord = 0; coord < dimension; coord++){
            nodes[index][coord] += displacement((index-nodeNumOffset) * dimension + coord,0);
        }
    }
}

void IsoMesh::solve(){

    // cholesky solver für dünnbestze/positiv semi definite Matritzen
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
    
    solver.compute(m_kSystem);
    m_uSystem = solver.solve(m_fSystem);

    addDenseRows(m_uSystem, Eigen::RowVectorXf::Zero(1), m_indicesToAdd);

    m_defNodes = m_nodes;
    displaceNodes(m_defNodes, m_uSystem.sparseView(), nodeNumOffset);
}

//
void IsoMesh::calculateStrainAndStress(DataSet& dataSet, const Eigen::MatrixXf& displacement, bool calculateOnQuadraturePoints){

    // LOG << "-- Calculate Strain and Stress" << ENDL;

    Eigen::MatrixXf BMatrix(nDimensions*(nDimensions + 1)/2, nDimensions * m_Cells.begin()->second.getPrefab().nNodes);
    float jDet = 0.0f;

    //
    dataSet.reserve(m_Cells.size());

    //
    int nodeNum = 0;
    for(const auto& [cellIndex, cell] : m_Cells){

        dataSet.try_emplace(cellIndex, cell.getPrefab());

        CellData& r_cellData = dataSet.at(cellIndex);
        const CellPrefab& r_prefab = m_Cells[cellIndex].getPrefab();

        // 1, 2, 3, 4, ...
        for(nodeNum = 0; nodeNum < r_prefab.nNodes; nodeNum++){

            // x, y, z, ...
            for(const auto& globKoord : globKoords){
                
                r_cellData.cellDisplacement(nodeNum * nDimensions + globKoord, 0) = displacement((cell[nodeNum] - nodeNumOffset) * nDimensions + globKoord, 0);
            }
        }

        // 1, 2, 3, 4, ...
        for(nodeNum = 0; nodeNum < r_prefab.nNodes; nodeNum++){

            try{
                subMatrix(m_cachedBMats[cellIndex], BMatrix, r_prefab.quadraturePoints[nodeNum]);
            } catch(...){
                LOG << "SymEngine Matrix : " << m_cachedBMats[cellIndex].nrows() << " " << m_cachedBMats[cellIndex].ncols() <<
                        "Substitutions Container (Eigen Dense) " << BMatrix.rows() << " " << BMatrix.cols() << ENDL;

                RETURNING_ASSERT(TRIGGER_ASSERT, "substition von cached BMatrix in Eigen BMatrix Container fehlgeschlagen ",);
            }

            jDet = SymEngine::eval_double(*m_cachedJDets[cellIndex]->subs(r_prefab.quadraturePoints[nodeNum]));

            // hier gibt es zwei optionen :
            // 1. die entsprechenden Größen für Quadraturpunkte errechnen
            // . Vorteil : Größen an Quadraturpunkten abrufbar
            // . Nachteil : langsamer aufgrund von vielen lese/schreibzugriffen in den Heap
            // 2. direkt Größen für gesamte Zellen errechnen
            // . Vorteil : schneller
            // . Nachteil : größen an Quadraturpunkten nicht abrufbar
            // -> Steuerung über bool param

            r_cellData.quadratureStrain[nodeNum] = BMatrix * r_cellData.cellDisplacement * jDet * r_prefab.weights[nodeNum];
            r_cellData.quadratureStress[nodeNum] = CMatrix * BMatrix * r_cellData.cellDisplacement * jDet * r_prefab.weights[nodeNum];

            r_cellData.strain += r_cellData.quadratureStrain[nodeNum];
            r_cellData.stress += r_cellData.quadratureStress[nodeNum];
            
            r_cellData.cellVolume += jDet * r_prefab.weights[nodeNum];
        }

        r_cellData.calculateCellStrainAndStress();
    }
}