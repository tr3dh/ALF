#include "Mesh.h"

const char IsoMesh::tokenIndicator = '*';  
const std::string IsoMesh::tokenIndicatorString = "*";
const std::string IsoMesh::nodeToken = "Node";
const std::string IsoMesh::cellToken = "Element";
const std::string IsoMesh::endToken = "End Part";

enum class IsoMesh::ReadMode : uint8_t{

    NODES,                      // Nodes einlesen
    CELLS,                      // Elemente einlesen
    SUPPRESS,                   // Übergehe aktuelle Sektion
    PASS,                       // Lesevorgang bzw. Zeilenweise Auswertung wird abgebrochen
    NONE,                       // Default wert
};

bool IsoMesh::loadFromFile(const std::string& path){

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

    size_t nodeDimension, nodeCount;
    std::vector<std::string> lineSplits;
    std::vector<float> nodeKoords;
    std::vector<NodeIndex> nodeIndices;

    while (readFile && std::getline(file, line)) {
    
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

                readMode = ReadMode::SUPPRESS;
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
                if(!m_nodes.size()){

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
                
                m_nodes.try_emplace(string::convert<NodeIndex>(lineSplits[0]), nDimensions, nodeKoords);

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
    LOG << LOG_BLUE << "-- Ladevorgang abgeschlossen, geladen : " << m_nodes.size() << " Nodes | " << m_Cells.size() << " Elemente" << endl;
    LOG << endl;

    return true;
}

bool IsoMesh::loadIsoMeshMaterial(const std::string& path){

    m_material.loadFromFile(path == NULLSTR ?
        meshPath.substr(0, string::findLast(meshPath, ".")) + IsoMeshMaterial::fileSuffix : path);

    return true;
}