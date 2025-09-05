#include "Mesh.h"

bool IsoMesh::createStiffnessMatrix(){

    //
    LOG << LOG_BLUE << "-- Creating Stiffnes Matrix" << ENDL;

    //
    prefabIndex currentPrefabIndex = m_Cells.begin()->second.getPrefabIndex();
    const CellPrefab& currentCellPrefab = getCachedCellPrefab(currentPrefabIndex);

    // Jacobi Matrix bestimmen
    Expression sum = NULL_EXPR;

    //
    SymEngine::DenseMatrix Jacoby(nDimensions,nDimensions),
                           jInv(nDimensions,nDimensions);

    Expression jDet = NULL_EXPR;

    SymEngine::DenseMatrix BMatrix(nDimensions*(nDimensions + 1)/2, currentCellPrefab.nNodes * nDimensions);
    Eigen::MatrixXf subsBMatrix(nDimensions*(nDimensions + 1)/2, currentCellPrefab.nNodes * nDimensions);
    Eigen::MatrixXf kCell(currentCellPrefab.nNodes * nDimensions, currentCellPrefab.nNodes * nDimensions);

    m_kSystem = Eigen::SparseMatrix<float>(m_nodes.size() * nDimensions, m_nodes.size() * nDimensions);

    // Entspricht numerischesLimit vom unsignedInt der für als NodeIndex definiert worden ist -1
    // für uint16_t : 65535 -> Wert der sicher nie in den Nodeindices vorkommt und wenn doch muss
    // Nodeindex dann über einen größeren uint definiert werden
    NodeIndex globalNodeNum_Row = -1, globalNodeNum_Col = -1;
    
    // IsoMeshMaterial erstellt mit geladenen IsoMeshMaterialparametern E,v,t den Elastizitätstensor E
    m_material.createElasticityTensor(SymCMatrix, nDimensions);

    //
    CMatrix = Eigen::MatrixXf(SymCMatrix.nrows(), SymCMatrix.nrows());
    subMatrix(SymCMatrix, CMatrix, {});

    // Hier werden die Multiplikationen der Ansatzfunktionsableitungen (liegen in Prefab vor als SymengineMatrix)
    // und der Jacoby Inversen zwischengespeichert für die einzelnen Elementnodes
    // also shapeFDerivsForGlobKoords[localNode] = jInv @ shapeFDerivsForGlobKoords[locNode]
    std::vector<SymEngine::DenseMatrix> shapeFDerivsForGlobKoords = {};

    // für Konstruktion der sparse Matrix
    std::vector<Eigen::Triplet<float>> triplets = {};

    // 1,2,3,4,5,6,...
    for(const auto& [cellIndex, cell] : m_Cells){

        //
        ASSERT(cell.getPrefabIndex() == currentPrefabIndex, "Multi Element netze noch nicht implementiert");

        // wenn verschiedene Elementtypen auftauchen müssen B, jacoby und Kcell und kInt reshaped werden
        // erster reshape kann in der forschleife erfolgen wenn currentPrefab index hier gesetzt wird und bei Abweichung
        // dann der reshape erfolgt 

        //
        clearMatrix(Jacoby);
        
        // r, s, t, ...
        for(const auto& locKoord : isoKoords){
            
            // x, y, z, ...
            for(const auto& globKoord : globKoords){

                //
                sum = NULL_EXPR;

                // 1, 2, 3, 4, ...
                for(int locNode = 0; locNode< currentCellPrefab.nNodes; locNode++){
                    
                    // r,x,1 ... r,x,4
                    // r,y,1 ... r,y,4
                    // s,x,1 ... s,x,4
                    // s,y,1 ... s,y,4

                    // entspricht Jacoby[global, local] += shapeFunktion * globKoord
                    sum = sum + currentCellPrefab.shapeFunctionDerivatives[locNode].get(0,locKoord) *\
                                    m_nodes[cell[locNode]][globKoord];
                } 

                Jacoby.set(globKoord, locKoord, sum);
            }
        }

        jDet = Jacoby.det();
        Jacoby.inv(jInv);

        // [dN1dx, dN1dy,
        //  dN2dx, ...]
        // Matrix ermitteln

        // 1, 2, 3, 4, ...
        shapeFDerivsForGlobKoords.clear();
        shapeFDerivsForGlobKoords.reserve(currentCellPrefab.nNodes);
        for(int locNode = 0; locNode < currentCellPrefab.nNodes; locNode++){

            shapeFDerivsForGlobKoords.emplace_back(1,nDimensions);
            currentCellPrefab.shapeFunctionDerivatives[locNode].mul_matrix(jInv, shapeFDerivsForGlobKoords[locNode]);
        }

        // B Matrix ermitteln
        clearMatrix(BMatrix);

        // mit .get wird in symengine Pointer auf das entsprechende Element in der Matrix zurückgegeben
        // etwas suboptimal, später evtl. über vorabReferenzierung optimieren
        // reicht aber für den Moment

        // 1, 2, 3, 4, ...
        for(uint8_t locNode = 0; locNode < currentCellPrefab.nNodes; locNode++){

            // x, y, z, ...
            for(const auto& globKoord : globKoords){

                // Eintrag in oberen Reihen (versetzt um globKoords)
                BMatrix.set(globKoord, locNode * nDimensions + globKoord, shapeFDerivsForGlobKoords[locNode].get(0,globKoord));

                // unterste Reihe (Quad4Cell::s_nDimensions + 1) für Scherung
                if(nDimensions == 2){

                    // in untere Spalte wird dNi/dy dNi/dx geschrieben
                    BMatrix.set(nDimensions, locNode * nDimensions + (nDimensions - 1 - globKoord),
                        shapeFDerivsForGlobKoords[locNode].get(0,globKoord));

                } else if(nDimensions == 3) {
                    
                    // pro Reihe den eintrag auf der Diagonalen der unteren 3x3 Matrix freilassen und auf die anderen Einträge
                    // den jeweils anderen schreiben
                    // ->   0 dNi/dz dNi/dy
                    //      dNi/dz 0 dNi/dx
                    //      dNi/dy dNi/dx 0
                    for(uint8_t shearRow = 0; shearRow < 3; shearRow++){
                        
                        //
                        if(globKoord != shearRow){

                            // swap der verbliebenen indices != globKoord über swapped = 3-globKoord-shearRow
                            BMatrix.set(nDimensions + shearRow, locNode * nDimensions + globKoord,
                                shapeFDerivsForGlobKoords[locNode].get(0,3-globKoord-shearRow));
                        }
                    }
                }
            }
        }

        // nötig und exakt ??
        expandMatrix(BMatrix);
        sym::roundMatrix(BMatrix);
        
        SymEngine::expand(jDet);
        sym::roundAllNumbers(jDet);

        //
        m_cachedJDets.emplace(cellIndex,jDet);
        m_cachedBMats.emplace(cellIndex, BMatrix);

        // Kmatrix Ermitteln
        kCell.setZero();
        subsBMatrix.setZero();

        // nur vorrübergehende Lösung, später wird schon vorab in die shape Funktionen eingesetzt
        // und Jacoby und B Matrix direkt mit den entsprechenden eingesetzten Werte über Eigen berechnet
        // (-> schnellere und weniger umständliche Matrix multiplikation da eigen Kette ermöglicht -> A*B*C'*...)
        // entspricht kCell = B'*C*B*jDet*weight*t
        for(uint8_t nodeNum = 0; nodeNum < currentCellPrefab.nNodes; nodeNum++){

            //
            if(!subMatrix(BMatrix, subsBMatrix, currentCellPrefab.quadraturePoints[nodeNum])){

                _ERROR << "Substitution fehlgeschlagen bei BMatrix von Element " << +cellIndex << ENDL;
                _ERROR << "Weigth " << currentCellPrefab.weights[nodeNum];

                return false;
            }

            kCell += subsBMatrix.transpose() * CMatrix * subsBMatrix *
                SymEngine::eval_double(*jDet->subs(currentCellPrefab.quadraturePoints[nodeNum])) * currentCellPrefab.weights[nodeNum] *
                (nDimensions == 2 ? m_material.t : 1);
        }

        // Elementsteifigkeits Matrix [kCell] ermittlelt
        // Assembierung bzw. Eintrag in Steifigkeitsmatrix des Gesamtsystems
        NodeIndex nodeNum_Row, nodeNum_Col = 0;
        NodeIndex globKoord_Row = 0, globKoord_Col = 0;

        // Eintrag in glob K Matrix
        NodeIndex kGlobRow = 0, kGlobCol = 0;

        //                                                                                          // erwartete Schreibzugriffe auf triplets Vektor (-> in glob KMatrix) :
        triplets.reserve((std::pow(currentCellPrefab.nNodes*nDimensions, 2)/2                       // + hälfte aller Einträge aus der Elementsteifigkeitsmatrix  
                            + currentCellPrefab.nNodes*nDimensions/2)                               // + hälfte der Diagonalelemente -> alle Einträge aus der oberen Dreiecksmatrix
                            * m_nodes.size());                                                      // * anzahl elemente 
        
        //
        for(nodeNum_Row = 0; nodeNum_Row < currentCellPrefab.nNodes; nodeNum_Row++){

            //
            for(nodeNum_Col = 0; nodeNum_Col < currentCellPrefab.nNodes; nodeNum_Col++){

                // Durch die beiden schleifen wird so jeder erste Eintrag eines <dimension> x <dimension> Block
                // in der lokalen Steifigkeitsmatrix abgelaufen
                // die for schleife über die globalen Koordinaten bzw. die Blockkopien (effizienter) sorgt dann dafür,
                // dass jeder Eintrag dieses Blocks übertragen wird
                
                globalNodeNum_Row = cell[nodeNum_Row] - nodeNumOffset;
                globalNodeNum_Col = cell[nodeNum_Col] - nodeNumOffset;

                // x, y, z, ...
                for(globKoord_Row = 0; globKoord_Row < nDimensions; globKoord_Row++){

                    // x, y, z, ...
                    for(globKoord_Col = 0; globKoord_Col < nDimensions; globKoord_Col++){

                        kGlobRow = globalNodeNum_Row * nDimensions + globKoord_Row;
                        kGlobCol = globalNodeNum_Col * nDimensions + globKoord_Col;

                        // nur obere Hälfte berechnen, da Matrix symmetrisch
                        if(kGlobCol < kGlobRow){
                            continue;
                        }

                        triplets.emplace_back(kGlobRow, kGlobCol, \
                            kCell(nodeNum_Row * nDimensions + globKoord_Row, nodeNum_Col * nDimensions + globKoord_Col));
                    }
                }
            }
        }
    }

    //
    m_kSystem.setFromTriplets(triplets.begin(), triplets.end());
    Eigen::SparseMatrix<float> temp = m_kSystem.transpose().triangularView<Eigen::StrictlyLower>();
    m_kSystem += temp;

    LOG << LOG_BLUE << "-- Finished Creating Stiffnes Matrix" << ENDL;
    LOG << ENDL;

    //
    return true;
}