#pragma once

#include "Cell.h"
#include "Coloration.h"

// Für Vector2 (z.B. sf::Vector2f, sf::Vector2i, ...)
template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

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

    static constexpr char tokenIndicator = '*';                                                     // gibt an mit welchem Token die entsprechenden Zeilen beginnen in denen der
    static constexpr std::string tokenIndicatorString = "*";                                        // folgende Abschnitt definiert wird 
    static constexpr std::string nodeToken = "Node";
    static constexpr std::string cellToken = "Element";
    static constexpr std::string endToken = "End Part";

    std::string meshPath = NULLSTR;

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
        LOG << LOG_BLUE << "-- Lade Mesh aus file " << path << "" << endl;

        // Check ob file existiert und in erforderlichen Format vorhanden ist
        CRITICAL_ASSERT(fs::exists(fs::path(path)), "Angegebener Pfad für Netz existiert nicht");
        CRITICAL_ASSERT(string::endsWith(path, "inp"), "Ungültige File Endung, Programm erwartet ein *.inp file");

        //
        meshPath = path;

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
        std::string cellPrefLabel = NULLSTR;            // Label des Prefabs
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
                else if(string::startsWith(line, tokenIndicatorString)){

                    readMode = ReadMode::IGNORE;
                    continue;
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
                        LOG << LOG_BLUE << "   Dimension : " << nDimensions << " D" << endl;
                        LOG << LOG_BLUE << "   Node Bennenung ab Nr. " << nodeNumOffset << endl;
                        LOG << endl;
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
                    ASSERT(nodeCount == cellPrefNodes, "Eingelesenes Element hat ungültige Anzahl an Nodes, Prefab Nodes : " + std::to_string(cellPrefNodes)
                        + " übergebene Nodes : " + std::to_string(nodeCount));

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
        LOG << endl;
        LOG << LOG_BLUE << "-- Ladevorgang abgeschlossen, geladen : " << m_Nodes.size() << " Nodes | " << m_Cells.size() << " Elemente" << endl;
        LOG << endl;

        return true;
    }

    //
    SymEngine::DenseMatrix SymCMatrix;
    Eigen::MatrixXd CMatrix;

    //
    std::map<NodeIndex, Expression>  m_cachedJDets = {};
    std::map<NodeIndex, SymEngine::DenseMatrix> m_cachedBMats = {};

    bool createStiffnessMatrix(){

        //
        LOG << LOG_BLUE << "-- Creating Stiffnes Matrix" << endl;

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

        SymCMatrix = SymEngine::DenseMatrix(nDimensions + 1, nDimensions + 1,
                    {One, Poission, Null,
                     Poission, One, Null,
                     Null, Null, (1-Poission)/2});

        SymCMatrix.mul_scalar(stiffnessCoeff, SymCMatrix);

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
            BMatrixT.mul_matrix(SymCMatrix,BMatrixT);
            BMatrixT.mul_matrix(BMatrix, kInt);
            kInt.mul_scalar(jDet*thickness, kInt);

            // aufsummieren der KMatritzen Einträge für die verschiedenen Integrationspunkte

            //
            kCell.setZero();

            // 1, 2, 3, 4, ...
            for(uint8_t nodeNum = 0; nodeNum < currentCellPrefab.nNodes; nodeNum++){

                //
                if(!subMatrix(kInt, kCell, currentCellPrefab.quadraturePoints[nodeNum], currentCellPrefab.weights[nodeNum], true)){

                    _ERROR << "Substitution fehlgeschlagen bei Element " << +cellIndex << endl;
                    _ERROR << " Weigth " << currentCellPrefab.weights[nodeNum];

                    _ERROR << "kInt " << kInt << endl;
                    return false;
                }
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

        LOG << LOG_BLUE << "-- Finished Creating Stiffnes Matrix" << endl;
        LOG << endl;

        //
        return true;
    }

    struct Force{
        uint8_t direction;
        float amount;
    };

    void applyForces(const std::map<NodeIndex, std::vector<Force>>& externalForces){

        //
        LOG << "-- Aplying loads ..." << endl;

        // für Konstruktion der sparse Matrix
        std::vector<Eigen::Triplet<float>> triplets = {};

        m_fSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * nDimensions, 1);

        for(const auto& [index, forces] : externalForces){
            for(const auto& force : forces){

                CRITICAL_ASSERT(force.direction < nDimensions, "Ungültige Richtungsangebe");
                LOG << "   Node " << +index << " Force " << force.amount << " N\t\tin direction " << g_globalKoords[force.direction] << endl;
                triplets.emplace_back((index-nodeNumOffset) * nDimensions + force.direction, 0, force.amount);
            }
        }

        LOG << endl;

        m_fSystem.setFromTriplets(triplets.begin(), triplets.end());
    }

    std::vector<NodeIndex> m_indicesToRemove = {};
    void fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations){

        //
        LOG << "-- Aplying node Constraints ..." << endl;

        m_indicesToRemove.clear();
        m_uSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * nDimensions, 1);

        for(const auto& [index, dirVec] : nodeFixations){
            for(const auto& direction : dirVec){

                //
                CRITICAL_ASSERT(direction < nDimensions, "Ungültige Richtungsangebe");
                LOG << "   Fixing node " << index << "\t\t\tin direction " << g_globalKoords[direction] << endl;

                m_indicesToRemove.emplace_back(nDimensions * (index - nodeNumOffset) + direction);
            }
        }

        LOG << endl;

        // Absteigend sortieren
        std::sort(m_indicesToRemove.begin(), m_indicesToRemove.end(), std::greater<NodeIndex>());

        LOG << "-- Removing indices {";
        for(const auto& i : m_indicesToRemove){
            LOG << i << ",";
        }
        LOG << "}" << endl;
        LOG << endl;

        removeSparseRow(m_uSystem, m_indicesToRemove);
        removeSparseRow(m_fSystem, m_indicesToRemove);
        removeSparseRowAndCol<NodeIndex>(m_kSystem, m_indicesToRemove);

        CRITICAL_ASSERT(m_fSystem.rows() == m_kSystem.rows() && m_uSystem.rows() == m_kSystem.rows(), "Matritzen Dimensionen von u,f und K stimmen nicht überein");
    }

    bool readBoundaryConditions(const std::string& path = NULLSTR){
        
        std::string boundaryFilePath = path;
        if(boundaryFilePath == NULLSTR){
            boundaryFilePath = meshPath.substr(0, meshPath.find_last_of('.')) + ".fem";
        }        

        CRITICAL_ASSERT(string::endsWith(boundaryFilePath, ".fem"), "Übergebener Pfad endet auf ungültige File Endung, erwartetes format : *.fem");

        LOG << LOG_BLUE << "-- Reading file : " << boundaryFilePath << endl;
        LOG << endl;

        // Check ob Pfad existiert
        CRITICAL_ASSERT(fs::exists(boundaryFilePath), "Angegebener Pfad : '" + boundaryFilePath + "' existiert nicht");

        // Check ob fstream den file ohne Fehler geladen bekommt
        std::ifstream BoundaryConditionFile(boundaryFilePath);
        CRITICAL_ASSERT(BoundaryConditionFile, "Fehler beim Laden von '" + path + "'");

        // Check ob nlohmann json den file geparst bekommt
        nlohmann::json ffData = nlohmann::json::parse(BoundaryConditionFile, nullptr, true, true);
        
        //
        if(!ffData.contains("Constraints") || !ffData["Constraints"].is_array()){

            ASSERT(TRIGGER_ASSERT, "Constraints konnten nicht gelesen werden");

            return false;  
        }

        std::map<NodeIndex, std::vector<uint8_t>> constraints = {};
        for(const auto& constraint : ffData["Constraints"]){

            for(const auto& [node, dirs] : constraint.items()){

                constraints.try_emplace(string::convert<NodeIndex>(node), dirs.get<std::vector<uint8_t>>());
            }
        }

        //
        if(!ffData.contains("Loads") || !ffData["Loads"].is_array()){

            ASSERT(TRIGGER_ASSERT, "Loads konnten nicht gelesen werden");

            return false;  
        }

        std::map<NodeIndex, std::vector<Force>> loads = {};
        for(const auto& load : ffData["Loads"]){

            for(const auto& [node, forceList] : load.items()){

                const NodeIndex index = string::convert<NodeIndex>(node);
                loads.try_emplace(index);
                
                for(const auto& [_, force] : forceList.items()){

                    for(const auto& [dir, amount] : force.items()){

                        const uint8_t direction = string::convert<uint8_t>(dir);
                        for(const auto& [_,isolatedAmount] : amount.items()){
                        
                            loads[index].emplace_back(direction, isolatedAmount.get<float>());
                        }
                    }
                }
            }
        }

        applyForces(loads);
        fixNodes(constraints);

        return true;
    }

    void solve(){

        // cholesky solver für dünnbestze/positiv semi definite Matritzen
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
        
        solver.compute(m_kSystem);
        m_uSystem = solver.solve(m_fSystem);

        std::sort(m_indicesToRemove.begin(), m_indicesToRemove.end());
        addSparseRow(m_uSystem, m_indicesToRemove);

        m_defNodes = m_Nodes;
        for(const auto& [index, node] : m_Nodes){

            for(const auto& coord : isoKoords){
                m_defNodes[index][coord] += m_uSystem.coeff((index-nodeNumOffset) * nDimensions + coord,0);
            }
        }
    }

    //
    void calculateStrainAndStress(){

        LOG << LOG_BLUE << "-- Calculate Strain and Stress" << endl;

        CMatrix = Eigen::MatrixXd(nDimensions + 1, nDimensions + 1);
        subMatrix(SymCMatrix,CMatrix,{});

        Eigen::MatrixXd BMatrix(nDimensions + 1, nDimensions * m_Cells.begin()->second.getPrefab().nNodes);
        float jDet = 0.0f;

        //
        int nodeNum = 0;
        for(const auto& [cellIndex, cell] : m_Cells){

            CellData& r_cellData = m_Cells[cellIndex].getCellData();
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

    void display(const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0, bool displayOnDeformedMesh = false, bool displayOnQuadraturePoints = false,
        const sf::Vector2f& padding = {50,50}){

        if(nDimensions != 2){
            ASSERT(TRIGGER_ASSERT, "Rendering bislang nur für 2D implementiert");
        }

        // Render Window
        sf::RenderWindow window(sf::VideoMode(1800,1000), std::string(magic_enum::enum_name(displayedData)) + " - " + std::to_string(globKoord));
        window.setFramerateLimit(0);

        // Autofit
        sf::Vector2f min = {m_Nodes.begin()->second[0], m_Nodes.begin()->second[1]};
        sf::Vector2f max = min;

        for(const auto& [index, node] : m_Nodes){

            if(node[0] < min.x){min.x = node[0]; }
            else if(node[0] > max.x){max.x = node[0]; }

            if(node[1] < min.y){min.y = node[1]; }
            else if(node[1] > max.y){max.y = node[1]; }
        }

        for(const auto& [index, node] : m_defNodes){

            if(node[0] < min.x){min.x = node[0]; }
            else if(node[0] > max.x){max.x = node[0]; }

            if(node[1] < min.y){min.y = node[1]; }
            else if(node[1] > max.y){max.y = node[1]; }
        }

        sf::Vector2u winSize = window.getSize();

        // erforderliche Größe -> min bis max muss auf größe winsize - 2 * padding kommen (an jedem Rand einmal)
        sf::Vector2f targetSize = {winSize.x - 2 * padding.x, winSize.y - 2 * padding.y};
        sf::Vector2f size = {max.x - min.x, max.y - min.y};
        sf::Vector2f scalingVec = {targetSize.x/size.x, targetSize.y/size.y};

        float scaling = (scalingVec.x > scalingVec.y) ? scalingVec.y : scalingVec.x;

        sf::Vector2f midOfMesh = {min.x + size.x/2, min.y + size.y/2};
        sf::Vector2f midOfWin = {winSize.x/2, winSize.y/2};

        sf::Vector2f offset = {-(midOfWin.x - scaling * midOfMesh.x), -(midOfWin.y - (scaling * midOfMesh.y))};

        float fData = 0.0f, fmin = 0, fmax = 0;

        if(displayOnQuadraturePoints){

            for(const auto& [cellIndex, cell] : m_Cells){

                const CellData& data = cell.getCellData();

                for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){

                    fData = data.getData(displayedData, globKoord, localNodeNum);

                    if(fData < fmin){
                        fmin = fData;
                    }
                    else if(fData > fmax){
                        fmax = fData;
                    }

                }
            }
        } else {

            CellIndex maxInd,minInd;

            for(const auto& [cellIndex, cell] : m_Cells){
            
                const CellData& data = cell.getCellData();
                fData = data.getData(displayedData, globKoord);

                if(fData < fmin){
                    fmin = fData;
                    minInd = cellIndex;
                }
                else if(fData > fmax){
                    fmax = fData;
                    maxInd = cellIndex;
                }
            }
            LOG << "   maximale Größe : " << fmax << " bei Elem " << maxInd << endl;
            LOG << "   minimale Größe : " << fmin << " bei Elem " << minInd << endl;
        }

        // mittiger punkt soll mittig im Fenster liegen
        while (window.isOpen()) {

            sf::Event event;
            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {

                    window.close();
                }
            }

            window.clear(sf::Color::Black);

            int rad = 4;
            sf::CircleShape dot(rad);
            dot.setOrigin(rad, rad);

            sfLine line;
            line.setThickness(2);

            sfQuad quad;

            dynNodeXd<float> nullRefNode;
            dynNodeXd<float>& node1 = nullRefNode, node2 = nullRefNode;
            dynNodeXd<float>& defnode1 = nullRefNode, defnode2 = nullRefNode;

            NodeIndex nodeNum1, nodeNum2;
            NodeIndex previousNum, lastNum, nextNum;
            sf::Vector2f point1, point2;

            sfPolygon poly;
            poly.setPointCount(m_Cells.begin()->second.getPrefab().nNodes);

            std::vector<sf::Vector2f> points = {}, subpoints = {};
            for(const auto& [index, cell] : m_Cells){

                const CellPrefab& r_prefab = cell.getPrefab();

                points.clear();
                points.reserve(r_prefab.nNodes);
                
                for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                    // Refs für weniger overhead
                    nodeNum1 = localNodeNum;
                    node1 = (displayOnDeformedMesh ? m_defNodes : m_Nodes)[cell[nodeNum1]];
                    point1 = {(node1[0] * scaling) - offset.x, (node1[1] * scaling) - offset.y};
                    point1.y = window.getSize().y - point1.y;

                    //
                    points.emplace_back(point1);
                }

                if(!displayOnQuadraturePoints){

                    poly.setPoints(points);
                    poly.setFillColor(getColorByValue(cell.getCellData().getData(displayedData, globKoord), fmin, fmax));
                    poly.draw(window);

                    // quad.positionVerticies(points);
                    // quad.colorVerticies();
                    // quad.draw(window);
                }
                else{

                    sf::Vector2f midPoint = {0,0};

                    for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){
                        midPoint.x += points[localNodeNum].x;
                        midPoint.y += points[localNodeNum].y;
                    }

                    midPoint.x /= 4;
                    midPoint.y /= 4;

                    for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                        //
                        subpoints.clear();
                        subpoints.reserve(r_prefab.nNodes);

                        //
                        previousNum = localNodeNum;
                        lastNum = localNodeNum == 0 ? 3 : localNodeNum - 1;
                        nextNum = localNodeNum == 3 ? 0 : localNodeNum + 1;

                        // Refs für weniger overhead
                        subpoints.emplace_back(points[localNodeNum]);
                        subpoints.emplace_back((points[localNodeNum].x + points[nextNum].x)/2, (points[localNodeNum].y + points[nextNum].y)/2);
                        subpoints.emplace_back(midPoint);
                        subpoints.emplace_back((points[localNodeNum].x + points[lastNum].x)/2, (points[localNodeNum].y + points[lastNum].y)/2);

                        quad.positionVerticies(subpoints);
                        quad.colorVerticies(getColorByValue(cell.getCellData().getData(displayedData, globKoord, localNodeNum), fmin, fmax));
                        quad.draw(window);
                    }
                }
            }

            // undef mesh
            dot.setFillColor(sf::Color::White);
            line.colorVerticies(sf::Color::White);
            for(const auto& [index, cell] : m_Cells){

                const CellPrefab& r_prefab = cell.getPrefab();

                for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                    // Refs für weniger overhead
                    nodeNum1 = localNodeNum;
                    nodeNum2 = (localNodeNum == r_prefab.nNodes - 1) ? 0 : localNodeNum + 1;

                    node1 = m_Nodes[cell[nodeNum1]];
                    node2 = m_Nodes[cell[nodeNum2]];

                    //
                    point1 = {(node1[0] * scaling) - offset.x, (node1[1] * scaling) - offset.y};
                    point2 = {(node2[0] * scaling) - offset.x, (node2[1] * scaling) - offset.y};

                    point1.y = window.getSize().y - point1.y;
                    point2.y = window.getSize().y - point2.y;

                    //
                    dot.setPosition(point1);
                    window.draw(dot);

                    //
                    line.positionVerticies(point1, point2);
                    line.draw(window);
                }
            }

            // deformed mesh
            dot.setFillColor(sf::Color::Red);
            line.colorVerticies(sf::Color::Red);
            for(const auto& [index, cell] : m_Cells){

                const CellPrefab& r_prefab = cell.getPrefab();

                for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                    // Refs für weniger overhead
                    nodeNum1 = localNodeNum;
                    nodeNum2 = (localNodeNum == r_prefab.nNodes - 1) ? 0 : localNodeNum + 1;

                    defnode1 = m_defNodes[cell[nodeNum1]];
                    defnode2 = m_defNodes[cell[nodeNum2]];

                    //
                    point1 = {(defnode1[0] * scaling) - offset.x, (defnode1[1] * scaling) - offset.y};
                    point2 = {(defnode2[0] * scaling) - offset.x, (defnode2[1] * scaling) - offset.y};

                    //
                    point1.y = window.getSize().y - point1.y;
                    point2.y = window.getSize().y - point2.y;

                    //
                    dot.setPosition(point1);
                    window.draw(dot);

                    //
                    line.positionVerticies(point1, point2);
                    line.draw(window);
                }
            }

            window.display();
        }
    }
};