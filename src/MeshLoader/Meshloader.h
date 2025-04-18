#pragma once

#include "defines.h"

struct Node2D{

    Node2D() = default;
    Node2D(const float& xIn, const float& yIn) : x(xIn), y(yIn){}

    float x = 0.0f, y = 0.0f;

    // Zugriff per Indexoperator
    float& operator[](size_t index) {
        switch(index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Index muss 0 oder 1 sein");
        }
    }

    //
    friend std::ostream& operator<<(std::ostream& os, const Node2D& node) {
        os << "{" << node.x << ", " << node.y << "}";
        return os;
    }
};

struct Quad4Cell{

    constexpr static int s_nNodes = 4;
    constexpr static int s_nDimensions = 2;

    static Expression s_shapeFunctions[s_nNodes];
    static Expression s_shapeFunctionDerivatives[s_nNodes][s_nDimensions];

    //
    const static double gauss_pt; // 1.0/sqrt(3.0);
            
    //
    static std::vector<SymEngine::map_basic_basic> subs;

    static void deriveShapeFunctions(){
        for(const auto& [i, shapeFunktion] : std::views::enumerate(s_shapeFunctions)){
            s_shapeFunctionDerivatives[i][0] = shapeFunktion->diff(r);
            s_shapeFunctionDerivatives[i][1] = shapeFunktion->diff(s);
        }
    }

    Quad4Cell() = default;
    Quad4Cell(NodeIndex a, NodeIndex b, NodeIndex c, NodeIndex d) 
        : m_cellNodes{a, b, c, d} {}

    Quad4Cell(const std::vector<NodeIndex>& nodeIndices){

        CRITICAL_ASSERT(nodeIndices.size() == 4, "Ungültige Anzahl an Nodes für Konstrukt des Quad4 cells übergeben");

        for(size_t counter = 0; counter < nodeIndices.size(); counter++){
            m_cellNodes[counter] = nodeIndices[counter];
        }
    }

    //
    friend std::ostream& operator<<(std::ostream& os, const Quad4Cell& cell) {
        
        os << "{"; 
        for(const auto& [i, nodeInd] : std::views::enumerate(cell.m_cellNodes)){
            
            if(i != 0){ os << ", "; }
            os << nodeInd;
        }
        os << "}";
        return os;
    }

    NodeIndex m_cellNodes[s_nNodes] = {0,0,0,0};
};

class Mesh{

public:

    Mesh() = default;

    enum class ReadMode : uint8_t{

        NODES,
        CELLS,
        IGNORE,
        PASS,
        NONE,
    };

    static constexpr char tokenIndicator = '*';                 // gibt an mit welchem Token die entsprechenden Zeilen beginnen in denen der
                                                                // folgende Abschnitt definiert wird 
    static constexpr std::string nodeToken = "*Node";
    static constexpr std::string cellToken = "*Element";
    static constexpr std::string endToken = "*End Part";

    std::map<NodeIndex, Node2D> m_Nodes = {}, m_defNodes = {};
    std::map<NodeIndex, Quad4Cell> m_Cells = {};

    static void genInpFile(const std::string& fileName){

        if(fs::exists(fs::path(fileName))){

            //
            fs::remove(fs::path(fileName));
            LOG << "File removed " << fileName << endl; LOG << endl;
        }

        std::ofstream file(fileName);



        file.close();

    }

    bool loadFromFile(const std::string& path){

        LOG << "Lade Mesh ..." << endl;

        CRITICAL_ASSERT(fs::exists(fs::path(path)), "Angegebener Pfad für Netzt existiert nicht");
        CRITICAL_ASSERT(string::endsWith(path, "inp"), "Ungültige File Endung, Programm erwartet ein *.inp file");

        std::ifstream file(path);
        CRITICAL_ASSERT(file, "Datei konnte nicht geöffnet werden");

        bool readFile = true;
        std::string line;
        std::string cellType;
        ReadMode readMode = ReadMode::NONE;

        while (readFile && std::getline(file, line)) {
        
            // !! vorab resize für m_nodes und m_cells damit durch die vermiedene dynamische und ständige allokierung kein overhead entsteht
            if(line[0] == '*'){

                if(string::contains(line, nodeToken)){
                
                    //
                    readMode = ReadMode::NODES;
                    continue;
                }
                else if(string::contains(line, cellToken)){
                    
                    readMode = ReadMode::CELLS;
                    cellType = line.substr(string::findLast(line, "=") + 1, line.length() - 1);

                    CRITICAL_ASSERT(cellType == "CPS4R", "Invalider celltyp im Mesh file");

                    continue;
                }
                else if(string::contains(line, endToken)){
    
                    readMode = ReadMode::PASS;
                    break;
                }
            }

            static NodeIndex nodeDimension, nodeCount;
            static std::vector<std::string> lineSplits;

            switch(readMode){

                case ReadMode::NODES:{
                    
                    lineSplits = string::split(line,',');
                    string::trimVec(lineSplits, ' ');

                    // eine Nummerierung und der Rest die entsprechenden Koordinaten
                    nodeDimension = lineSplits.size() - 1; 
                    CRITICAL_ASSERT(nodeDimension == 2, "Eingelesenes Mesh hat ungültige Dimension");
                
                    // hier try_emplace weil emplace aufgrund fehlender pair Konstruktoren nen error schmeißt
                    m_Nodes.try_emplace(string::convert<NodeIndex>(lineSplits[0]),
                        string::convert<float>(lineSplits[1]), string::convert<float>(lineSplits[2]));

                    break;
                }
                case ReadMode::CELLS:{

                    static std::vector<NodeIndex> nodeIndices;

                    lineSplits = string::split(line,',');
                    string::trimVec(lineSplits, ' ');
                    
                    nodeCount = lineSplits.size() - 1;
                    CRITICAL_ASSERT(nodeCount == 4, "Eigelesenes Mesh hat ungültiges cell");

                    nodeIndices.clear();
                    nodeIndices.reserve(nodeCount);
                    for(const auto& [i, nodeIndex] : std::views::enumerate(lineSplits)){
                        
                        if(i == 0){ continue;}

                        nodeIndices.emplace_back(string::convert<NodeIndex>(lineSplits[i]));
                    }

                    m_Cells.emplace(string::convert<NodeIndex>(lineSplits[0]), nodeIndices);

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
        LOG << "Mesh geladen aus '" << path << "'" <<  endl;
        LOG << "cell Type : " << cellType << endl;
        LOG << "Nodes : " << m_Nodes.size() << endl;
        LOG << "Cells : " << m_Cells.size() << endl;
        LOG << endl;

        return true;
    }

    Eigen::SparseMatrix<float> m_kSystem, m_uSystem, m_fSystem;

    std::map<NodeIndex, Expression>  m_cachedJDets = {};
    std::map<NodeIndex, SymEngine::DenseMatrix> m_cachedBMats = {};

    SymEngine::DenseMatrix CMatrix;

    void createStiffnesMatrix(){

        LOG << "Creating Stiffnes Matrix" << endl;

        // Jacobi Matrix bestimmen
        Expression sum = NULL_EXPR;
        SymEngine::DenseMatrix Jacoby(Quad4Cell::s_nDimensions,Quad4Cell::s_nDimensions),
                                jInv(Quad4Cell::s_nDimensions,Quad4Cell::s_nDimensions),
                                BMatrix(Quad4Cell::s_nDimensions + 1, Quad4Cell::s_nNodes * 2),
                                BMatrixT(Quad4Cell::s_nNodes * 2, Quad4Cell::s_nDimensions + 1),
                                kInt(Quad4Cell::s_nNodes * 2, Quad4Cell::s_nNodes * 2);

        Eigen::MatrixXd kCell(Quad4Cell::s_nNodes * 2, Quad4Cell::s_nNodes * 2);

        m_kSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * Quad4Cell::s_nDimensions, m_Nodes.size() * Quad4Cell::s_nDimensions);

        Expression jDet = NULL_EXPR;

        // Entspricht numerischesLimit vom unsignedInt der für als NodeIndex definiert worden ist -1
        // für uint16_t : 65535 -> Wert der sicher nie in den Nodeindices vorkommt und wenn doch muss
        // Nodeindex dann über einen größeren uint definiert werden
        NodeIndex globalNodeNum_Row = -1, globalNodeNum_Col = -1;
        
        //
        Expression thickness = toExpression(0.1);
        Expression ElastModule = toExpression(20000);
        Expression Poission = toExpression(0.2); 
        Expression stiffnessCoeff = ElastModule/(1-Poission*Poission);

        CMatrix = SymEngine::DenseMatrix(Quad4Cell::s_nDimensions + 1, Quad4Cell::s_nDimensions + 1,
            {stiffnessCoeff, stiffnessCoeff * Poission, toExpression(0), stiffnessCoeff * Poission, stiffnessCoeff,
                toExpression(0), toExpression(0), toExpression(0), stiffnessCoeff * (1-Poission)/2});

        // die Ableitungen der ShapeFunktionen hier nochmal in anderer Form abspeichern
        // in der Berechnung für die Einträge der B Matitzen müssen jedes mal
        // [dN1Dr dN1ds], [dN2Dr dN2ds], ... mit der Invertierte Jacoby multipliziert werden
        // um nicht jedes Mal (pro Element) die gleichen 4 Matritzen mit Zugriffen in das Array
        // und herauskopieren der Expression in eine neue 2 * 2 Matrix erstellen zu müssen
        // wird das hier vorab erledig
        // -> evtl SpeicherStruktur im Element (hier Quad4Cell) direkt auf diese Form anpassen
        std::vector<SymEngine::DenseMatrix> shapeFDerivs = {};
        std::vector<SymEngine::DenseMatrix> shapeFDerivsForGlobKoords = {};

        // für Konstruktion der sparse Matrix
        std::vector<Eigen::Triplet<float>> triplets = {};

        shapeFDerivs.reserve(Quad4Cell::s_nDimensions);
        for(int row = 0; row < Quad4Cell::s_nNodes; row++){

            shapeFDerivs.emplace_back(1,Quad4Cell::s_nDimensions);

            for(int col = 0; col < Quad4Cell::s_nDimensions; col++){

                shapeFDerivs[row].set(0, col, Quad4Cell::s_shapeFunctionDerivatives[row][col]);
            }
        }

        // 1,2,3,4,5,6,...
        for(const auto& [cellIndex, cell] : m_Cells){

            //
            clearMatrix(Jacoby);
            
            // r, s, t, ...
            for(int locKoord = 0; locKoord < Quad4Cell::s_nDimensions; locKoord++){
                
                // x, y, z, ...
                for(int globKoord = 0; globKoord < Quad4Cell::s_nDimensions; globKoord++){

                    //
                    sum = NULL_EXPR;

                    // 1, 2, 3, 4, ...
                    for(int locNode = 0; locNode< Quad4Cell::s_nNodes; locNode++){
                        
                        // r,x,1 ... r,x,4
                        // r,y,1 ... r,y,4
                        // s,x,1 ... s,x,4
                        // s,y,1 ... s,y,4

                        // entspricht Jacoby[global, local] += shapeFunktion * globKoord
                        sum = sum + Quad4Cell::s_shapeFunctionDerivatives[locNode][locKoord] *\
                                        m_Nodes[cell.m_cellNodes[locNode]][globKoord];
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
            shapeFDerivsForGlobKoords.reserve(Quad4Cell::s_nNodes);
            for(int locNode = 0; locNode < Quad4Cell::s_nNodes; locNode++){

                shapeFDerivsForGlobKoords.emplace_back(1,Quad4Cell::s_nDimensions);
                shapeFDerivs[locNode].mul_matrix(jInv, shapeFDerivsForGlobKoords[locNode]);
            }

            // B Matrix ermitteln
            clearMatrix(BMatrix);

            // mit .get wird in symengine Pointer auf das entsprechende Element in der Matrix zurückgegeben
            // etwas suboptimal, später evtl. über vorabReferenzierung optimieren
            // reicht aber für den Moment

            // 1, 2, 3, 4, ...
            for(int locNode = 0; locNode < Quad4Cell::s_nNodes; locNode++){

                // x, y, z, ...
                for(int globKoord = 0; globKoord < Quad4Cell::s_nDimensions; globKoord++){

                    // Eintrag in oberen Reihen (versetzt um globKoords)
                    BMatrix.set(globKoord, locNode * Quad4Cell::s_nDimensions + globKoord, shapeFDerivsForGlobKoords[locNode].get(0,globKoord));

                    // unterste Reihe (Quad4Cell::s_nDimensions + 1) für Scherung
                    BMatrix.set(Quad4Cell::s_nDimensions, locNode * Quad4Cell::s_nDimensions + (Quad4Cell::s_nDimensions - 1 - globKoord),
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
            for(int nodeNum = 0; nodeNum < Quad4Cell::s_nNodes; nodeNum++){

                //
                subMatrix(kInt, kCell, Quad4Cell::subs[nodeNum], true);
            }

            // Elementsteifigkeits Matrix [kCell] ermittlelt
            // Assembierung bzw. Eintrag in Steifigkeitsmatrix des Gesamtsystems
            NodeIndex nodeNum_Row, nodeNum_Col = 0;
            NodeIndex globKoord_Row = 0, globKoord_Col = 0;

            // Eintrag in glob K Matrix
            NodeIndex kGlobRow = 0, kGlobCol = 0;

            //                                                                                          // erwartete Schreibzugriffe auf triplets Vektor (-> in glob KMatrix) :
            triplets.reserve((std::pow(Quad4Cell::s_nNodes*Quad4Cell::s_nDimensions, 2)/2               // + hälfte aller Einträge aus der Elementsteifigkeitsmatrix  
                                + Quad4Cell::s_nNodes*Quad4Cell::s_nDimensions/2)                       // + hälfte der Diagonalelemente -> alle Einträge aus der oberen Dreiecksmatrix
                                * m_Nodes.size());                                                      // * anzahl elemente 
            //
            for(nodeNum_Row = 0; nodeNum_Row < Quad4Cell::s_nNodes; nodeNum_Row++){

                //
                for(nodeNum_Col = 0; nodeNum_Col < Quad4Cell::s_nNodes; nodeNum_Col++){

                    // Durch die beiden schleifen wird so jeder erste Eintrag eines <dimension> x <dimension> Block
                    // in der lokalen Steifigkeitsmatrix abgelaufen
                    // die for schleife über die globalen Koordinaten bzw. die Blockkopien (effizienter) sorgt dann dafür,
                    // dass jeder Eintrag dieses Blocks übertragen wird
                    
                    globalNodeNum_Row = cell.m_cellNodes[nodeNum_Row] - 1;
                    globalNodeNum_Col = cell.m_cellNodes[nodeNum_Col] - 1;

                    // alternativ für dense Matritzen :
                    // m_kSystem.block<Quad4Cell::s_nDimensions, Quad4Cell::s_nDimensions>(globalNodeNum_Row * Quad4Cell::s_nDimensions, globalNodeNum_Col * Quad4Cell::s_nDimensions) +=\
                        kCell.block<Quad4Cell::s_nDimensions, Quad4Cell::s_nDimensions>(nodeNum_Row * Quad4Cell::s_nDimensions, nodeNum_Col * Quad4Cell::s_nDimensions).sparseView();
                    
                    // x, y, z, ...
                    for(globKoord_Row = 0; globKoord_Row < Quad4Cell::s_nDimensions; globKoord_Row++){

                        // x, y, z, ...
                        for(globKoord_Col = 0; globKoord_Col < Quad4Cell::s_nDimensions; globKoord_Col++){

                            kGlobRow = globalNodeNum_Row * Quad4Cell::s_nDimensions + globKoord_Row;
                            kGlobCol = globalNodeNum_Col * Quad4Cell::s_nDimensions + globKoord_Col;

                            // nur obere Hälfte berechnen, da Matrix symmetrisch
                            if(kGlobCol < kGlobRow){
                                continue;
                            }

                            triplets.emplace_back(kGlobRow, kGlobCol, \
                                kCell(nodeNum_Row * Quad4Cell::s_nDimensions + globKoord_Row, nodeNum_Col * Quad4Cell::s_nDimensions + globKoord_Col));
                        }
                    }
                }
            }
        }

        //
        m_kSystem.setFromTriplets(triplets.begin(), triplets.end());
        Eigen::SparseMatrix<float> temp = m_kSystem.transpose().triangularView<Eigen::StrictlyLower>();
        m_kSystem += temp;

        LOG << "Finished Creating Stiffnes Matrix\n" << endl;
        LOG << m_kSystem.toDense().block(0,0,20,20) << endl;
        LOG << endl;
    }

    struct Force{
        uint8_t direction;
        float amount;
    };

    void applyForces(const std::map<NodeIndex, std::vector<Force>>& externalForces){

        // für Konstruktion der sparse Matrix
        std::vector<Eigen::Triplet<float>> triplets = {};

        m_fSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * Quad4Cell::s_nDimensions, 1);

        for(const auto& [index, forces] : externalForces){
            for(const auto& force : forces){

                CRITICAL_ASSERT(force.direction < Quad4Cell::s_nDimensions, "Ungültige Richtungsangebe");
                triplets.emplace_back((index-1) * Quad4Cell::s_nDimensions + force.direction, 0, force.amount);
            }
        }

        m_fSystem.setFromTriplets(triplets.begin(), triplets.end());
    }

    std::vector<NodeIndex> m_indicesToRemove = {};
    void fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations){

        //
        LOG << "Reading node Fixations ..." << endl;

        m_indicesToRemove.clear();
        m_uSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * Quad4Cell::s_nDimensions, 1);

        for(const auto& [index, dirVec] : nodeFixations){
            for(const auto& direction : dirVec){

                //
                CRITICAL_ASSERT(direction < Quad4Cell::s_nDimensions, "Ungültige Richtungsangebe");
                LOG << "Fixing node " << index << "\t\tin " << (direction == 0 ? "x" : "y") << " direction" << endl;

                m_indicesToRemove.emplace_back(Quad4Cell::s_nDimensions * (index - 1) + direction);
            }
        }

        // Absteigend sortieren
        std::sort(m_indicesToRemove.begin(), m_indicesToRemove.end(), std::greater<NodeIndex>());

        LOG << "Removing indices {";
        for(const auto& i : m_indicesToRemove){
            LOG << i << ",";
        }
        LOG << "}" << endl;

        removeSparseRow(m_uSystem, m_indicesToRemove);
        removeSparseRow(m_fSystem, m_indicesToRemove);
        removeSparseRowAndCol<NodeIndex>(m_kSystem, m_indicesToRemove);

        CRITICAL_ASSERT(m_fSystem.rows() == m_kSystem.rows() && m_uSystem.rows() == m_kSystem.rows(), "Matritzen Dimensionen von u,f und K stimmen nicht überein");
    }

    void solve(){

        // cholesky solver für dünnbestze/positiv semi definite Matritzen
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
        
        solver.compute(m_kSystem);
        m_uSystem = solver.solve(m_fSystem);

        std::sort(m_indicesToRemove.begin(), m_indicesToRemove.end());
        addSparseRow(m_uSystem, m_indicesToRemove);

        for(const auto& [index, node] : m_Nodes){

            m_defNodes[index] = Node2D(node.x + m_uSystem.coeff((index-1) * Quad4Cell::s_nDimensions, 0), node.y + m_uSystem.coeff((index-1) * Quad4Cell::s_nDimensions + 1, 0));
        }
    }

    enum class MeshData : uint8_t{

        DISPLACEMENT,
        STRAIN,
        STRESS,
        VANMISES_STRESS,
        NONE,
    };

    struct CellData{

        static CellData nullRef;

        CellData(){
           
            cellDisplacement = Eigen::MatrixXd(8,1);
            cellDisplacement.setZero();

            strain = Eigen::MatrixXd(3,1);
            strain.setZero();

            stress = Eigen::MatrixXd(3,1);
            stress.setZero();

            quadratureStrain.reserve(4);
            quadratureStress.reserve(4);
        };

        void calculateCellStrainAndStress(){

            // 1, 2, 3, 4, ...
            for(int nodeNum = 0; nodeNum < Quad4Cell::s_nNodes; nodeNum++){

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
            quadratureMisesStress.reserve(4);

            for(size_t nodeNum = 0; nodeNum < 4; nodeNum++){

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

        Eigen::MatrixXd strain, stress;
        std::vector<Eigen::MatrixXd> quadratureStrain = {}, quadratureStress = {};
        float cellVolume = 0.0f;
        Eigen::MatrixXd cellDisplacement;

        float vanMisesStress = 0.0f;
        std::vector<float> quadratureMisesStress = {};
    };

    Eigen::SparseMatrix<float> m_strainSystem, m_stressSystem;
    std::map<NodeIndex, CellData> m_cellData = {};
    void calculateStrainAndStress(){

        // später nach elem typ anpassen
        Eigen::MatrixXd epsilon_e(3,1), sigma_e(3,1), cMatrix(3,3);

        subMatrix(CMatrix,cMatrix,{});

        Eigen::MatrixXd BMatrix(3,8);
        float jDet = 0.0f;
        float cellVolume = 0;
        Eigen::MatrixXd cellDisplacement(8,1);

        //
        int nodeNum = 0;
        for(const auto& [cellIndex, cell] : m_Cells){

            //
            m_cellData.try_emplace(cellIndex);
            CellData& r_cellData = m_cellData[cellIndex];

            // Elementverschiebung aus globalem Verschiebungsvektor extrahieren

            // 1, 2, 3, 4, ...
            for(nodeNum = 0; nodeNum < Quad4Cell::s_nNodes; nodeNum++){

                // x, y, z, ...
                for(int globKoord = 0; globKoord < Quad4Cell::s_nDimensions; globKoord++){
                    
                    r_cellData.cellDisplacement(nodeNum * Quad4Cell::s_nDimensions + globKoord, 0) = m_uSystem.coeffRef((cell.m_cellNodes[nodeNum] - 1) * Quad4Cell::s_nDimensions + globKoord, 0);
                }
            }

            // 1, 2, 3, 4, ...
            for(nodeNum = 0; nodeNum < Quad4Cell::s_nNodes; nodeNum++){

                subMatrix(m_cachedBMats[cellIndex], BMatrix,Quad4Cell::subs[nodeNum]);
                jDet = SymEngine::eval_double(*m_cachedJDets[cellIndex]->subs(Quad4Cell::subs[nodeNum]));

                r_cellData.quadratureStrain.emplace_back(BMatrix * r_cellData.cellDisplacement * jDet);
                r_cellData.quadratureStress.emplace_back(cMatrix * BMatrix * r_cellData.cellDisplacement * jDet);

                r_cellData.cellVolume += jDet;
            }

            r_cellData.calculateCellStrainAndStress();
        }
    }

    sf::Color getColorByValue(float val, float min, float max) {

        // Normalisiere den Wert auf [0, 1]
        float normalized = (val - min) / (max - min);
        normalized = std::clamp(normalized, 0.0f, 1.0f);

        // Regenbogenfarben (HSB/HSV-basiert)
        float hue = (1.0f - normalized) * 0.666f; // Blau (0.666) → Rot (0.0)
        sf::Color color = sf::Color::Black;

        if (hue < 0.166f) {
            // Rot zu Gelb (R=255, G steigt)
            color.r = 255;
            color.g = static_cast<sf::Uint8>(hue * 6 * 255);
        } else if (hue < 0.333f) {
            // Gelb zu Grün (G=255, R fällt)
            color.g = 255;
            color.r = static_cast<sf::Uint8>((0.333f - hue) * 6 * 255);
        } else if (hue < 0.5f) {
            // Grün zu Türkis (G=255, B steigt)
            color.g = 255;
            color.b = static_cast<sf::Uint8>((hue - 0.333f) * 6 * 255);
        } else if (hue < 0.666f) {
            // Türkis zu Blau (B=255, G fällt)
            color.b = 255;
            color.g = static_cast<sf::Uint8>((0.666f - hue) * 6 * 255);
        } else {
            // Blau (Vollton)
            color.b = 255;
        }

        return color;
    }
    
    void display(const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0, bool displayOnDeformedMesh = false, bool displayOnQuadraturePoints = false,
        const int& offset = -200, const int& scaling = 3500){

        std::string dir;
        switch (globKoord){
            case 0:{
                dir = "x";
                break;
            }
            case 1:{
                dir = "y";
                break;
            }
            case 2:{
                dir = "xy";
                break;
            }
            default:{
                dir = "NONE";
                break;
            }
        }

        // Render Window
        sf::RenderWindow window(sf::VideoMode(1200,800), std::string(magic_enum::enum_name(displayedData)) + " - " + dir);
        window.setFramerateLimit(0);

        while (window.isOpen()) {

            sf::Event event;
            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {

                    window.close();
                }
            }

            window.clear(sf::Color::Transparent);

            // Rendere Daten

            //
            int rad = 4;
            sf::CircleShape dot(rad);
            dot.setOrigin(rad, rad);

            sfLine line;
            line.setThickness(2);

            sfQuad quad;

            Node2D nullRefNode;
            Node2D& node1 = nullRefNode, node2 = nullRefNode;
            Node2D& defnode1 = nullRefNode, defnode2 = nullRefNode;

            NodeIndex nodeNum1, nodeNum2;
            NodeIndex previousNum, lastNum, nextNum;
            sf::Vector2f point1, point2;

            float fData = 0.0f, min = 0, max = 0;

            if(displayOnQuadraturePoints){

                for(const auto& [cellIndex, data] : m_cellData){

                    for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){

                        fData = data.getData(displayedData, globKoord, localNodeNum);
    
                        if(fData < min){
                            min = fData;
                        }
                        else if(fData > max){
                            max = fData;
                        }

                    }
                }
            } else {

                for(const auto& [cellIndex, data] : m_cellData){
                
                    fData = data.getData(displayedData, globKoord);
    
                    if(fData < min){
                        min = fData;
                    }
                    else if(fData > max){
                        max = fData;
                    }
    
                }
            }

            std::vector<sf::Vector2f> points = {}, subpoints = {};
            for(const auto& [index, cell] : m_Cells){

                points.clear();
                points.reserve(4);
                
                for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){

                    // Refs für weniger overhead
                    nodeNum1 = localNodeNum;
                    node1 = (displayOnDeformedMesh ? m_defNodes : m_Nodes)[cell.m_cellNodes[nodeNum1]];
                    point1 = {(node1.x * scaling) - offset, (node1.y * scaling) - offset};
                    point1.y = window.getSize().y - point1.y;

                    //
                    points.emplace_back(point1);
                }

                if(!displayOnQuadraturePoints){
                    quad.positionVerticies(points);
                    quad.colorVerticies(getColorByValue(m_cellData[index].getData(displayedData, globKoord), min, max));
                    quad.draw(window);
                }
                else{

                    sf::Vector2f midPoint = {0,0};

                    for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){
                        midPoint.x += points[localNodeNum].x;
                        midPoint.y += points[localNodeNum].y;
                    }

                    midPoint.x /= 4;
                    midPoint.y /= 4;

                    for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){

                        //
                        subpoints.clear();
                        subpoints.reserve(4);

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
                        quad.colorVerticies(getColorByValue(m_cellData[index].getData(displayedData, globKoord, localNodeNum), min, max));
                        quad.draw(window);
                    }
                }
            }

            // undef mesh
            dot.setFillColor(sf::Color::White);
            line.colorVerticies(sf::Color::White);
            for(const auto& [index, cell] : m_Cells){

                for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){

                    // Refs für weniger overhead
                    nodeNum1 = localNodeNum;
                    nodeNum2 = (localNodeNum == Quad4Cell::s_nNodes - 1) ? 0 : localNodeNum + 1;

                    node1 = m_Nodes[cell.m_cellNodes[nodeNum1]];
                    node2 = m_Nodes[cell.m_cellNodes[nodeNum2]];

                    //
                    point1 = {(node1.x * scaling) - offset, (node1.y * scaling) - offset};
                    point2 = {(node2.x * scaling) - offset, (node2.y * scaling) - offset};

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

                for(size_t localNodeNum = 0; localNodeNum < Quad4Cell::s_nNodes; localNodeNum++){

                    // Refs für weniger overhead
                    nodeNum1 = localNodeNum;
                    nodeNum2 = (localNodeNum == Quad4Cell::s_nNodes - 1) ? 0 : localNodeNum + 1;

                    defnode1 = m_defNodes[cell.m_cellNodes[nodeNum1]];
                    defnode2 = m_defNodes[cell.m_cellNodes[nodeNum2]];

                    //
                    point1 = {(defnode1.x * scaling) - offset, (defnode1.y * scaling) - offset};
                    point2 = {(defnode2.x * scaling) - offset, (defnode2.y * scaling) - offset};

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