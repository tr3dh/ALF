#pragma once

#include "defines.h"

typedef uint16_t NodeIndex; 

struct Node2D{

    Node2D() = default;
    Node2D(const float& xIn, const float& yIn) : x(xIn), y(yIn){}

    float x = 0.0f, y = 0.0f;

    //
    friend std::ostream& operator<<(std::ostream& os, const Node2D& node) {
        os << "{" << node.x << ", " << node.y << "}";
        return os;
    }
};

struct Quad4Cell{

    Quad4Cell() = default;
    Quad4Cell(NodeIndex a, NodeIndex b, NodeIndex c, NodeIndex d) 
        : m_cellNodes{a, b, c, d} {}

    Quad4Cell(const std::vector<NodeIndex>& nodeIndices){

        CRITICAL_ASSERT(nodeIndices.size() == 4, "Ungültige Anzahl an Nodes für Konstrukt des Quad4 cells übergeben");

        for(size_t counter = 0; counter < nodeIndices.size(); counter++){
            m_cellNodes[counter] = nodeIndices[counter];
        }
    }

    NodeIndex m_cellNodes[4] = {0,0,0,0};

    //
    friend std::ostream& operator<<(std::ostream& os, const Quad4Cell& cell) {
        
        os << "{"; 
        for(const auto& [i, nodeInd] : std::views::enumerate(cell.m_cellNodes)){
            
            if(i != 0){
                os << ", ";
            }

            os << nodeInd;
        }
        os << "}";
        return os;
    }
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

        LOG << "cell Type : " << cellType << std::endl;
        LOG << "Nodes : " << m_Nodes.size() << std::endl;
        LOG << "Cells : " << m_Cells.size() << std::endl;

        return true;
    }
};