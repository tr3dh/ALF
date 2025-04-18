#pragma once

#include "Cell.h"

class IsoMesh{

public:

    IsoMesh() = default;

private:

    enum class ReadMode : uint8_t{

        NODES,
        CELLS,
        IGNORE,
        PASS,
        NONE,
    };

    static constexpr char tokenIndicator = '*';                 // gibt an mit welchem Token die entsprechenden Zeilen beginnen in denen der
                                                                // folgende Abschnitt definiert wird 
    static constexpr std::string nodeToken = "Node";
    static constexpr std::string cellToken = "Element";
    static constexpr std::string endToken = "End Part";

    size_t nDimensions = 0;
    size_t nodeNumOffset = 0;
    std::map<NodeIndex, dynNodeXd<float>> m_Nodes = {}, m_defNodes = {};
    std::map<CellIndex, Cell> m_Cells = {};

    std::vector<uint8_t> isoKoords = {};
    std::vector<uint8_t> globKoords = {};

    Eigen::SparseMatrix<float> m_kSystem, m_uSystem, m_fSystem;

public:

    bool loadFromFile(const std::string& path){

        //
        LOG << BLUE << "-- Lade Mesh aus file " << path << "" << endl;

        // Check ob file existiert und in erforderlichen Format vorhanden ist
        CRITICAL_ASSERT(fs::exists(fs::path(path)), "Angegebener Pfad für Netzt existiert nicht");
        CRITICAL_ASSERT(string::endsWith(path, "inp"), "Ungültige File Endung, Programm erwartet ein *.inp file");

        // Laden des files
        std::ifstream file(path);
        CRITICAL_ASSERT(file, "Datei konnte nicht geöffnet werden");

        // Decl der Variable für Einlesen der Daten
        bool readFile = true;                           // Abbruch bedingung
        std::string line;                               // container für aktuelle Zeile
        ReadMode readMode = ReadMode::NONE;             // gibt Verarbeitung der aktuellen Zeile vor

        // Wenn beim lesen des Files ein Element typ angeführt wird, wird versucht aus einem gleichnamigen *.ISOPARAM
        // file ein prefab in den prefab Cache zu laden

        // Stats für Elementprefab dessen Member aktuell eingelesen werden
        NodeIndex cellPrefIndex = -1;                   // Identifikation des Prefabs im entsprechenden Cache
        std::string cellPrefLabel = "__INVALID__";      // Label des Prefabs
        size_t cellPrefNodes = 0;                       // Anzahl an Nodes für Member des Prefabs

        while (readFile && std::getline(file, line)) {
        
            static size_t nodeDimension, nodeCount;
            static std::vector<std::string> lineSplits;
            static std::vector<float> nodeKoords;
            static std::vector<NodeIndex> nodeIndices;

            // !! vorab resize für m_nodes und m_cells damit durch die vermiedene dynamische und ständige allokierung kein overhead entsteht ??
            if(line[0] == tokenIndicator){

                if(string::contains(line, tokenIndicator + nodeToken)){
                
                    //
                    readMode = ReadMode::NODES;
                    continue;
                }
                else if(string::contains(line, tokenIndicator + cellToken)){
                    
                    readMode = ReadMode::CELLS;
                    cellPrefLabel = line.substr(string::findLast(line, "=") + 1, line.length() - 1);
                    cellPrefIndex = cacheCellPrefab(cellPrefLabel);
                    cellPrefNodes = getCachedCellPrefab(cellPrefIndex).nNodes;

                    nodeIndices.clear();
                    nodeIndices.reserve(cellPrefNodes);

                    for(size_t i = 0; i < cellPrefNodes; i++){
                        nodeIndices.emplace_back(0);
                    }

                    continue;
                }
                else if(string::contains(line, tokenIndicator + endToken)){
    
                    readMode = ReadMode::PASS;
                    break;
                }
            }

            switch(readMode){
                case ReadMode::NODES:{

                    // line splitten und trimmen sodass koordinaten und Index bereinigt von Leerzeichne im vektor stehen
                    lineSplits = string::split(line,',');
                    string::trimVec(lineSplits, ' ');

                    // eine Nummerierung und der Rest die entsprechenden Koordinaten
                    nodeDimension = lineSplits.size() - 1;
                    
                    // bereits initialisiert ??
                    if(!nDimensions){

                        // Dimension für Netz anhand der ersten eigelesenen Node setzen
                        nDimensions = nodeDimension;
                    }
                    if(!m_Nodes.size()){
                        //
                        nodeKoords.clear();
                        nodeKoords.reserve(nDimensions);

                        for(size_t dim = 0; dim < nDimensions; dim++){
                            nodeKoords.emplace_back();
                        }

                        nodeNumOffset = string::convert<NodeIndex>(lineSplits[0]);
                        ASSERT(string::convert<NodeIndex>(lineSplits[0]) >= 0, "Fehlerhafte Node Indizierung, beginnt mit negativem Wert");

                        //
                        LOG << BLUE << "-- " << nDimensions << " dimensionales Netz wird aus " << path << " generiert" << endl;
                        LOG << BLUE << "-- Node Bennenung beginnt mit Nr. " << nodeNumOffset << endl; 
                    }

                    ASSERT(nodeDimension == nDimensions, "Eingelesenes Mesh hat ungültige Dimension");
                
                    // hier try_emplace weil emplace aufgrund fehlender pair Konstruktoren nen error schmeißt
                    for(const auto& [i, split] : std::views::enumerate(lineSplits)){

                        // Element Index an erster Stelle in Zeile
                        if(!i){ continue; }

                        // danach folgen die eigentlichen Koordinaten
                        nodeKoords[i-1] = string::convert<float>(lineSplits[i]);
                    }
                    
                    m_Nodes.try_emplace(string::convert<NodeIndex>(lineSplits[0]), nDimensions, nodeKoords);

                    break;
                }
                case ReadMode::CELLS:{

                    lineSplits = string::split(line,',');
                    string::trimVec(lineSplits, ' ');
                    
                    nodeCount = lineSplits.size() - 1;
                    ASSERT(nodeCount == cellPrefNodes, "Eigelesenes Element hat ungültige Anzahl an Nodes");

                    for(const auto& [i, nodeIndex] : std::views::enumerate(lineSplits)){
                        
                        if(!i){ continue; }

                        nodeIndices[i-1] = string::convert<NodeIndex>(lineSplits[i]);
                    }

                    //
                    m_Cells.try_emplace(string::convert<CellIndex>(lineSplits[0]), cellPrefIndex, nodeIndices);

                    break;
                }
                case ReadMode::PASS:{
                    
                    readFile = false;
                    break;
                }
                default:{
                        
                    break;
                }
            }
        }

        //
        isoKoords.reserve(nDimensions);
        globKoords.reserve(nDimensions);

        //
        for(uint8_t i = 0; i < nDimensions; i++){
            
            //
            isoKoords.emplace_back(i);
            globKoords.emplace_back(i);
        }

        //
        LOG << BLUE << "-- " << nDimensions << " dimensionales Mesh geladen aus '" << path << "'" << endl;
        LOG << BLUE << "-- Mesh geladen mit " << m_Nodes.size() << " Nodes | " << m_Cells.size() << " Elementen" << endl;
        LOG << endl;

        return true;
    }

    //
    SymEngine::DenseMatrix CMatrix;

    //
    std::map<NodeIndex, Expression>  m_cachedJDets = {};
    std::map<NodeIndex, SymEngine::DenseMatrix> m_cachedBMats = {};

    bool createStiffnessMatrix(){

        //
        LOG << BLUE << "-- Creating Stiffnes Matrix" << endl;

        //
        prefabIndex currentPrefabIndex = m_Cells.begin()->second.getPrefabIndex();
        CellPrefab currentCellPrefab = getCachedCellPrefab(currentPrefabIndex);

        // Jacobi Matrix bestimmen
        Expression sum = NULL_EXPR;

        //
        SymEngine::DenseMatrix Jacoby(nDimensions,nDimensions),
                               jInv(nDimensions,nDimensions);
        Expression jDet = NULL_EXPR;

        SymEngine::DenseMatrix BMatrix(nDimensions + 1, currentCellPrefab.nNodes * nDimensions),
                               BMatrixT(currentCellPrefab.nNodes * nDimensions, nDimensions + 1);

        SymEngine::DenseMatrix kInt(currentCellPrefab.nNodes * nDimensions, currentCellPrefab.nNodes * nDimensions);
        Eigen::MatrixXd kCell(currentCellPrefab.nNodes * nDimensions, currentCellPrefab.nNodes * nDimensions);

        m_kSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * nDimensions, m_Nodes.size() * nDimensions);

        // Entspricht numerischesLimit vom unsignedInt der für als NodeIndex definiert worden ist -1
        // für uint16_t : 65535 -> Wert der sicher nie in den Nodeindices vorkommt und wenn doch muss
        // Nodeindex dann über einen größeren uint definiert werden
        NodeIndex globalNodeNum_Row = -1, globalNodeNum_Col = -1;
        
        //
        Expression thickness = toExpression(0.1);
        Expression ElastModule = toExpression(20000);
        Expression Poission = toExpression(0.2); 
        Expression stiffnessCoeff = ElastModule/(1-Poission*Poission);
        Expression One = toExpression(1);
        Expression Null = toExpression(0);

        ASSERT(nDimensions == 2, "für " << std::to_string(nDimensions) << " dimensionales Netz nicht implementiert");

        CMatrix = SymEngine::DenseMatrix(nDimensions + 1, nDimensions + 1,
                    {One, Poission, Null,
                     Poission, One, Null,
                     Null, Null, (1-Poission)/2});

        CMatrix.mul_scalar(stiffnessCoeff, CMatrix);

        // die Ableitungen der ShapeFunktionen hier nochmal in anderer Form abspeichern
        // in der Berechnung für die Einträge der B Matitzen müssen jedes mal
        // [dN1Dr dN1ds], [dN2Dr dN2ds], ... mit der Invertierte Jacoby multipliziert werden
        // um nicht jedes Mal (pro Element) die gleichen 4 Matritzen mit Zugriffen in das Array
        // und herauskopieren der Expression in eine neue 2 * 2 Matrix erstellen zu müssen
        // wird das hier vorab erledig
        // -> evtl SpeicherStruktur im Element (hier Quad4Cell) direkt auf diese Form anpassen
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
                                        m_Nodes[cell[locNode]][globKoord];
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
                    BMatrix.set(nDimensions, locNode * nDimensions + (nDimensions - 1 - globKoord),
                        shapeFDerivsForGlobKoords[locNode].get(0,globKoord));
                }
            }

            //
            m_cachedJDets.emplace(cellIndex,jDet);
            m_cachedBMats.emplace(cellIndex, BMatrix);

            // Kmatrix Ermitteln
            clearMatrix(kInt);

            BMatrix.transpose(BMatrixT);

            // nur vorrübergehende Lösung, später wird schon vorab in die shape Funktionen eingesetzt
            // und Jacoby und B Matrix direkt mit den entsprechenden eingesetzten Werte über Eigen berechnet
            // (-> schnellere und weniger umständliche Matrix multiplikation da eigen Kette ermöglicht -> A*B*C'*...)
            // entspricht kInt = B'*C*B*jDet*t
            BMatrixT.mul_matrix(CMatrix,BMatrixT);
            BMatrixT.mul_matrix(BMatrix, kInt);
            kInt.mul_scalar(jDet*thickness, kInt);

            // aufsummieren der KMatritzen Einträge für die verschiedenen Integrationspunkte

            //
            kCell.setZero();

            // 1, 2, 3, 4, ...
            for(uint8_t nodeNum = 0; nodeNum < currentCellPrefab.nNodes; nodeNum++){

                //
                subMatrix(kInt, kCell, currentCellPrefab.quadraturePoints[nodeNum], true);
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
                                * m_Nodes.size());                                                      // * anzahl elemente 
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

                    // alternativ für dense Matritzen :
                    // m_kSystem.block<Quad4Cell::s_nDimensions, Quad4Cell::s_nDimensions>(globalNodeNum_Row * Quad4Cell::s_nDimensions, globalNodeNum_Col * Quad4Cell::s_nDimensions) +=\
                        kCell.block<Quad4Cell::s_nDimensions, Quad4Cell::s_nDimensions>(nodeNum_Row * Quad4Cell::s_nDimensions, nodeNum_Col * Quad4Cell::s_nDimensions).sparseView();
                    
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

        LOG << BLUE << "-- Finished Creating Stiffnes Matrix\n" << endl;
        LOG << BLUE << m_kSystem.toDense().block(0,0,20,20) << endl;
        LOG << endl;

        //
        return true;
    }
};