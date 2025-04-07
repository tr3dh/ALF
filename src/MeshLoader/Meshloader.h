#pragma once

#include "defines.h"

typedef uint16_t NodeIndex; 

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
    const static std::vector<SymEngine::map_basic_basic> subs;

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

    std::map<NodeIndex, Node2D> m_Nodes = {};
    std::map<NodeIndex, Quad4Cell> m_Cells = {};

    bool loadFromFile(const std::string& path){

        LOG << "Lade Mesh ..." << std::endl;

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
        LOG << "Mesh geladen aus '" << path << "'" <<  std::endl;
        LOG << "cell Type : " << cellType << std::endl;
        LOG << "Nodes : " << m_Nodes.size() << std::endl;
        LOG << "Cells : " << m_Cells.size() << std::endl;
        LOG << std::endl;

        return true;
    }

    Eigen::SparseMatrix<float> m_kSystem, m_uSystem, m_fSystem;

    void createStiffnesMatrix(){

        LOG << "Creating Stiffnes Matrix" << std::endl;

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

        SymEngine::DenseMatrix CMatrix(Quad4Cell::s_nDimensions + 1, Quad4Cell::s_nDimensions + 1,
            {stiffnessCoeff, stiffnessCoeff * Poission, toExpression(0), stiffnessCoeff * Poission, stiffnessCoeff,
                toExpression(0), toExpression(0), toExpression(0), (1-Poission)/2});

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

                    Jacoby.set(locKoord, globKoord, sum);
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

        LOG << "Finished Creating Stiffnes Matrix" << std::endl;
        LOG << std::endl;
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

    void fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations){

        //
        LOG << "Reading node Fixations ..." << std::endl;

        std::vector<NodeIndex> indicesToRemove = {};
        m_uSystem = Eigen::SparseMatrix<float>(m_Nodes.size() * Quad4Cell::s_nDimensions, 1);

        for(const auto& [index, dirVec] : nodeFixations){
            for(const auto& direction : dirVec){

                //
                CRITICAL_ASSERT(direction < Quad4Cell::s_nDimensions, "Ungültige Richtungsangebe");
                LOG << "Fixing node " << index << "\t\tin " << (direction == 0 ? "x" : "y") << " direction" << std::endl;

                indicesToRemove.emplace_back(Quad4Cell::s_nDimensions * (index - 1) + direction);
            }
        }

        // Absteigend sortieren
        std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<NodeIndex>());

        LOG << "Removing indices {";
        for(const auto& i : indicesToRemove){
            LOG << i << ",";
        }
        LOG << "}" << std::endl;

        removeSparseRow(m_uSystem, indicesToRemove);
        removeSparseRow(m_fSystem, indicesToRemove);
        removeSparseRowAndCol<NodeIndex>(m_kSystem, indicesToRemove);

        CRITICAL_ASSERT(m_fSystem.rows() == m_kSystem.rows() && m_uSystem.rows() == m_kSystem.rows(), "Matritzen Dimensionen von u,f und K stimmen nicht überein");
    } 
};