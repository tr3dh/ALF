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

    // std::map<NodeIndex, Quad4Cell> m_Cells = {};

    size_t nDimensions = 0;
    size_t nodeNumOffset = 0;
    std::map<NodeIndex, dynNodeXd<float>> m_Nodes = {}, m_defNodes = {};
    std::map<CellIndex, Cell> m_Cells = {};

public:

    bool loadFromFile(const std::string& path){

        LOG << BLUE << "-- Lade Mesh aus file " << path << " --" << endl;

        CRITICAL_ASSERT(fs::exists(fs::path(path)), "Angegebener Pfad für Netzt existiert nicht");
        CRITICAL_ASSERT(string::endsWith(path, "inp"), "Ungültige File Endung, Programm erwartet ein *.inp file");

        std::ifstream file(path);
        CRITICAL_ASSERT(file, "Datei konnte nicht geöffnet werden");

        bool readFile = true;
        std::string line;
        ReadMode readMode = ReadMode::NONE;

        NodeIndex cellPrefIndex = -1;
        std::string cellPrefLabel = "__INVALID__";
        size_t cellPrefNodes = 0;

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
                        LOG << BLUE << "-- " << nDimensions << " dimensionales Netz wird aus " << path << " generiert --" << endl;
                        LOG << BLUE << "-- Node Bennenung beginnt mit Nr. " << nodeNumOffset << " --" << endl; 
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
        LOG << BLUE << "-- Mesh geladen aus '" << path << "' --" << endl;
        LOG << BLUE << "-- Mesh geladen mit " << m_Nodes.size() << " Nodes | " << m_Cells.size() << " Elementen --" << endl;
        LOG << endl;

        return true;
    }

};