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

        LOG << "Mesh geladen aus '" << path << "'" <<  std::endl;
        LOG << "cell Type : " << cellType << std::endl;
        LOG << "Nodes : " << m_Nodes.size() << std::endl;
        LOG << "Cells : " << m_Cells.size() << std::endl;

        return true;
    }

    void createStiffnesMatrix(){

        LOG << "Creating Stiffnes Matrix" << std::endl;

        // Jacobi Matrix bestimmen
        Expression sum = NULL_EXPR;
        SymEngine::DenseMatrix Jacoby(Quad4Cell::s_nDimensions,Quad4Cell::s_nDimensions),
                                jInv(Quad4Cell::s_nDimensions,Quad4Cell::s_nDimensions),
                                BMatrix(Quad4Cell::s_nDimensions + 1, Quad4Cell::s_nNodes * 2);

        Expression jDet = NULL_EXPR;

        // die Ableitungen der ShapeFunktionen hier nochmal in anderer Form abspeichern
        // in der Berechnung für die Einträge der B Matitzen müssen jedes mal
        // [dN1Dr dN1ds], [dN2Dr dN2ds], ... mit der Invertierte Jacoby multipliziert werden
        // um nicht jedes Mal (pro Element) die gleichen 4 Matritzen mit Zugriffen in das Array
        // und herauskopieren der Expression in eine neue 2 * 2 Matrix erstellen zu müssen
        // wird das hier vorab erledig
        // -> evtl SpeicherStruktur im Element (hier Quad4Cell) direkt auf diese Form anpassen
        std::vector<SymEngine::DenseMatrix> shapeFDerivs = {};
        std::vector<SymEngine::DenseMatrix> shapeFDerivsForGlobKoords = {};

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

            LOG << BMatrix << std::endl;
        }

        LOG << "Finished Creating Stiffnes Matrix" << std::endl;
    }
};