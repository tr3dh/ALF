#include "Mesh.h"

float g_cellsWireFrameThickness2D = 2.0f;
float g_cellsNodeRadius2D = 4.0f;

void IsoMesh::display(const DataSet& dataSet, const MeshData& displayedData, const int& globKoord, const std::vector<NodeSet*>& nodeSets,
    const std::vector<Color*> setColors, int displayOnNodeSet, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const Vector2& winSize, const Vector2& frameOffset, const Vector2& padding){

    //
    RETURNING_ASSERT(setColors.size() >= nodeSets.size(), "zu wenig Farben fürs Rendering der nodeSet angegeben",);

    // Autofit
    // ...

    switch (nDimensions){

        // 2D Renering
        case 2:{

            // Autofit
            Vector2 min = {nodeSets[0]->begin()->second[0], nodeSets[0]->begin()->second[1]};
            Vector2 max = min;

            for(const auto& nodeSet : nodeSets){

                for(const auto& [index, node] : *nodeSet){

                    if(node[0] < min.x){min.x = node[0]; }
                    else if(node[0] > max.x){max.x = node[0]; }
            
                    if(node[1] < min.y){min.y = node[1]; }
                    else if(node[1] > max.y){max.y = node[1]; }
                }
            }

            // erforderliche Größe -> min bis max muss auf größe winsize - 2 * padding kommen (an jedem Rand einmal)
            Vector2 targetSize = {winSize.x - 2 * padding.x, winSize.y - 2 * padding.y};
            Vector2 size = {max.x - min.x, max.y - min.y};
            Vector2 scalingVec = {targetSize.x/size.x, targetSize.y/size.y};

            float scaling = (scalingVec.x > scalingVec.y) ? scalingVec.y : scalingVec.x;

            Vector2 midOfMesh = {min.x + size.x/2, min.y + size.y/2};
            Vector2 midOfWin = {winSize.x/2.0f, winSize.y/2.0f};

            // Berechnung für die y-Komponente des render Offsets mag etwas willkürlich erscheinen
            // der Termo kommt so zustande, da der Offset fürs invertierte Rendering der punkte angepasst werden muss
            //
            // für x Richtung ist die Berechnung ohne invertiertes Rendering simpel, die x Stelle an der eine Node gerendert wird ergibt sich als:
            //
            //          x = (node.x * scaling) - offset.x
            //
            // für den bekannten Netz Mittelpunkt soll die Node exakt ins Frame Zentrum gerendert werden
            //
            //          frameOffset.x = x(midOfMesh.x)    
            //      <=> frameOffset.x = midOfMesh.x * scaling) - offset.x
            //      <=> offset.x = midOfMesh.x * scaling - frameOffset.x
            // 
            // gerendert wird in y Richtung nach dem Schema:
            //
            //          y = winSize.y - ((node.y * scaling) - offset.y)
            //
            // dieser Term kommt über Skalierung und Offsetten der y Komponente zustande die dann an der Fenstermitte gespiegelt wird
            // 
            //          y = winSize.y - y | y = node.y * scaling - offset.y
            //
            // es ist der frameoffset gegeben, dabei handelt es sich um den Mittelpunkt des frames in den gezeichnet werden soll
            // winSize ist die größe des frames in den gezeichnet werden soll
            // damit muss bei korrektem rendering die Netzmitte auf den frame Mittelpunkt gezeichnet werden
            // das bedeutet
            //           
            //          y(midOfMesh.y) = frameOffset.y
            //
            // nach Einsetzen lässt sich der Term nach der einzig unbekannte Größe offset.y umgeformt werden
            //
            //          frameOffset.y = y(midOfMesh.y)                                               // -> Einsetzen
            //      <=> frameOffset.y = winSize.y - ((midOfMesh.y * scaling) - offset.y)             // rüberziehen weil subtraktion nicht assoziativ
            //      <=> frameOffset.y + ((midOfMesh.y * scaling) - offset) = winSize.y
            //      <=> offset.y = frameOffset.y + (midOfMesh.y * scaling) - winSize.y
            //
            Vector2 offset = {scaling * midOfMesh.x - (frameOffset.x == -1 ? midOfWin.x : frameOffset.x),
                                scaling * midOfMesh.y + (frameOffset.y == -1 ? midOfWin.y : frameOffset.y) - winSize.y};

            float fData = 0.0f, fmin = 0, fmax = 0;
            CellIndex maxInd,minInd;

            for(const auto& [cellIndex, cell] : m_Cells){
            
                const CellData& data = dataSet.at(cellIndex);
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
            // LOG << "   maximaler " << magic_enum::enum_name(displayedData) << " : " << fmax << " bei Elem " << maxInd << endl;
            // LOG << "   minimaler " << magic_enum::enum_name(displayedData) << " : " << fmin << " bei Elem " << minInd << endl;
            // LOG << endl;

            RETURNING_ASSERT(nodeSets.size() > displayOnNodeSet, "displayOnNodeSet Index ist ungültig",);
            const NodeSet* d_nodeSet = nodeSets[displayOnNodeSet];

            const CellPrefab& r_prefab = m_Cells.begin()->second.getPrefab();
            Vector2 vertices[r_prefab.nNodes];

            NodeIndex prevNum, lastNum, nextNum;
            dynNodeXd<float> nullRef; 
            dynNodeXd<float>& prevNode = nullRef, lastNode = nullRef, nextNode = nullRef;

            for(const auto& [index, cell] : m_Cells){

                ASSERT(r_prefab.pID == cell.getPrefab().pID, "Multi Elementnetze sind bislang nicht implementiert");

                for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                    prevNum = localNodeNum;
                    lastNum = localNodeNum == 0 ? r_prefab.nNodes - 1 : localNodeNum - 1;
                    nextNum = localNodeNum == r_prefab.nNodes - 1 ? 0 : localNodeNum + 1;

                    prevNode = d_nodeSet->at(cell[prevNum]);
                    lastNode = d_nodeSet->at(cell[lastNum]);
                    nextNode = d_nodeSet->at(cell[nextNum]);

                    vertices[prevNum] = (Vector2){(prevNode[0] * scaling) - offset.x, winSize.y - ((prevNode[1] * scaling) - offset.y)};
                }
                
                DrawTriangleFan(vertices, r_prefab.nNodes, getColorByValue(dataSet.at(index).getData(displayedData, globKoord), fmin, fmax));
            }

            // zeichne Linien
            for(const auto& [setIndex, nodeSet] : std::views::enumerate(nodeSets)){
                for(const auto& [index, cell] : m_Cells){

                    ASSERT(r_prefab.pID == cell.getPrefab().pID, "Multi Elementnetze sind bislang nicht implementiert");

                    for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                        prevNum = localNodeNum;
                        lastNum = localNodeNum == 0 ? r_prefab.nNodes - 1 : localNodeNum - 1;
                        nextNum = localNodeNum == r_prefab.nNodes - 1 ? 0 : localNodeNum + 1;

                        prevNode = nodeSet->at(cell[prevNum]);
                        lastNode = nodeSet->at(cell[lastNum]);
                        nextNode = nodeSet->at(cell[nextNum]);

                        DrawLineEx((Vector2){(prevNode[0] * scaling) - offset.x, winSize.y - ((prevNode[1] * scaling) - offset.y)},
                                    (Vector2){(nextNode[0] * scaling) - offset.x, winSize.y - ((nextNode[1] * scaling) - offset.y)},
                                    g_cellsWireFrameThickness2D, *setColors[setIndex]);
                    }
                }
            }
            
            // Zeichne punkte
            for(const auto& [setIndex, nodeSet] : std::views::enumerate(nodeSets)){

                for(const auto& [index, node] : *nodeSet){

                    DrawCircle((node[0] * scaling) - offset.x, winSize.y - ((node[1] * scaling) - offset.y), g_cellsNodeRadius2D, *setColors[setIndex]);
                }
            }

            break;
        }
        // 3D Rendering
        case 3:{

            //
            const CellPrefab& r_pref = m_Cells.begin()->second.getPrefab();

            // Check nach Änderung des Element typs im Netz, zb bei neu Laden etc.
            // wird auch beim ersten Durchlauf getriggert da die validen prefab IDs im prefab Cache mit 1 beginnen
            // Rendering funktion geht von einheitlichem Element typen im Netz aus also pID oder prefab der ersten Zelle gleich denen
            // jeder weiteren
            static prefabIndex pID = 0;
            if(pID != r_pref.pID){

                pID = r_pref.pID;
                initCellRenderer(r_pref);
            }

            std::map<CellIndex, float> values = {};
            for (const auto& [cellIndex, cellData] : dataSet) {
                values.try_emplace(cellIndex, cellData.getData(displayedData, globKoord));
            }

            float minValue = 0, maxValue = 0;

            if (!values.empty()) {
                // Über die Werte iterieren, nicht die Map-Paare!
                auto [minIt, maxIt] = std::minmax_element(
                    values.begin(), values.end(),
                    [](const auto& a, const auto& b) { return a.second < b.second; }
                );
                minValue = minIt->second;
                maxValue = maxIt->second;
            }

            for (const auto& [cellIndex, cell] : m_Cells) {

                renderCell(cell, *nodeSets[displayOnNodeSet], getColorByValue(values[cellIndex], minValue, maxValue));
            }

            for (const auto& [cellIndex, cell] : m_Cells) {

                for(const auto& [idx, set] : std::views::enumerate(nodeSets)){

                    renderCellsWireFrame(cell, *set, *setColors[idx]);
                }
            }

            break;
        }
        default:{

            // Rendering nur für 2D/3D Systeme implementiert
            RETURNING_ASSERT(TRIGGER_ASSERT, "Angabe der ungültigen Dimension " + std::to_string(nDimensions) + " fürs Mesh Rendering",);

            break;
        }
    }
}