#include "Mesh.h"

void IsoMesh::display(const DataSet& dataSet, const MeshData& displayedData, const int& globKoord, const std::vector<const NodeSet*>& nodeSets,
    const std::vector<Color> setColors, int displayOnNodeSet, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const Vector2& winSize, const Vector2& frameOffset, const Vector2& padding){

    if(nDimensions != 2){
        ASSERT(TRIGGER_ASSERT, "Rendering bislang nur für 2D implementiert");
    }

    RETURNING_ASSERT(setColors.size() >= nodeSets.size(), "zu wenig Farben fürs Rendering der nodeSet angegeben",);

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
                            2, setColors[setIndex]);
            }
        }
    }
    
    // Zeichne punkte
    for(const auto& [setIndex, nodeSet] : std::views::enumerate(nodeSets)){

        for(const auto& [index, node] : *nodeSet){

            DrawCircle((node[0] * scaling) - offset.x, winSize.y - ((node[1] * scaling) - offset.y), 4, setColors[setIndex]);
        }
    }

    //     int rad = 4;
    //     sf::CircleShape dot(rad);
    //     dot.setOrigin(rad, rad);

    //     sfLine line;
    //     line.setThickness(2);

    //     // sfQuad quad;

    //     dynNodeXd<float> nullRefNode;
    //     dynNodeXd<float>& node1 = nullRefNode, node2 = nullRefNode;
    //     dynNodeXd<float>& defnode1 = nullRefNode, defnode2 = nullRefNode;

    //     NodeIndex nodeNum1, nodeNum2;
    //     NodeIndex previousNum, lastNum, nextNum;
    //     Vector2 point1, point2;

    //     sfPolygon poly;
    //     poly.setPointCount(m_Cells.begin()->second.getPrefab().nNodes);

    //     std::vector<Vector2> points = {}, subpoints = {};
    //     for(const auto& [index, cell] : m_Cells){

    //         const CellPrefab& r_prefab = cell.getPrefab();

    //         points.clear();
    //         points.reserve(r_prefab.nNodes);
            
    //         for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

    //             // Refs für weniger overhead
    //             nodeNum1 = localNodeNum;
    //             node1 = (displayOnDeformedMesh ? m_defNodes : m_nodes)[cell[nodeNum1]];
    //             point1 = {(node1[0] * scaling) - offset.x, (node1[1] * scaling) - offset.y};
    //             point1.y = window.getSize().y - point1.y;

    //             //
    //             points.emplace_back(point1);
    //         }

    //         if(!displayOnQuadraturePoints){

    //             poly.setPoints(points);
    //             poly.setFillColor(getColorByValue(m_cellData.at(index).getData(displayedData, globKoord), fmin, fmax));
    //             poly.draw(window);

    //             // quad.positionVerticies(points);
    //             // quad.colorVerticies();
    //             // quad.draw(window);
    //         }
    //         else{

    //             Vector2 midPoint = {0,0};

    //             for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){
    //                 midPoint.x += points[localNodeNum].x;
    //                 midPoint.y += points[localNodeNum].y;
    //             }

    //             midPoint.x /= 4;
    //             midPoint.y /= 4;

    //             for(size_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

    //                 //
    //                 subpoints.clear();
    //                 subpoints.reserve(r_prefab.nNodes);

    //                 //
    //                 previousNum = localNodeNum;
    //                 lastNum = localNodeNum == 0 ? 3 : localNodeNum - 1;
    //                 nextNum = localNodeNum == 3 ? 0 : localNodeNum + 1;

    //                 // Refs für weniger overhead
    //                 subpoints.emplace_back(points[localNodeNum]);
    //                 subpoints.emplace_back((points[localNodeNum].x + points[nextNum].x)/2, (points[localNodeNum].y + points[nextNum].y)/2);
    //                 subpoints.emplace_back(midPoint);
    //                 subpoints.emplace_back((points[localNodeNum].x + points[lastNum].x)/2, (points[localNodeNum].y + points[lastNum].y)/2);

    //                 // quad.positionVerticies(subpoints);
    //                 // quad.colorVerticies(getColorByValue(cell.getCellData().getData(displayedData, globKoord, localNodeNum), fmin, fmax));
    //                 // quad.draw(window);
    //             }
    //         }
    //     }

    //     // undef mesh
    //     dot.setFillColor(sf::Color::White);
    //     line.colorVerticies(sf::Color::White);
    //     for(const auto& [index, cell] : m_Cells){

    //         const CellPrefab& r_prefab = cell.getPrefab();

    //         for(uint8_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

    //             // Refs für weniger overhead
    //             nodeNum1 = localNodeNum;
    //             nodeNum2 = (localNodeNum == r_prefab.nNodes - 1) ? 0 : localNodeNum + 1;

    //             node1 = m_nodes[cell[nodeNum1]];
    //             node2 = m_nodes[cell[nodeNum2]];

    //             //
    //             point1 = {(node1[0] * scaling) - offset.x, (node1[1] * scaling) - offset.y};
    //             point2 = {(node2[0] * scaling) - offset.x, (node2[1] * scaling) - offset.y};

    //             point1.y = window.getSize().y - point1.y;
    //             point2.y = window.getSize().y - point2.y;

    //             //
    //             dot.setPosition(point1);
    //             window.draw(dot);

    //             //
    //             line.positionVerticies(point1, point2);
    //             line.draw(window);
    //         }
    //     }

    //     // deformed mesh
    //     dot.setFillColor(sf::Color::Red);
    //     line.colorVerticies(sf::Color::Red);
    //     for(const auto& [index, cell] : m_Cells){

    //         const CellPrefab& r_prefab = cell.getPrefab();

    //         for(uint8_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

    //             // Refs für weniger overhead
    //             nodeNum1 = localNodeNum;
    //             nodeNum2 = (localNodeNum == r_prefab.nNodes - 1) ? 0 : localNodeNum + 1;

    //             defnode1 = m_defNodes[cell[nodeNum1]];
    //             defnode2 = m_defNodes[cell[nodeNum2]];

    //             //
    //             point1 = {(defnode1[0] * scaling) - offset.x, (defnode1[1] * scaling) - offset.y};
    //             point2 = {(defnode2[0] * scaling) - offset.x, (defnode2[1] * scaling) - offset.y};

    //             //
    //             point1.y = window.getSize().y - point1.y;
    //             point2.y = window.getSize().y - point2.y;

    //             //
    //             dot.setPosition(point1);
    //             window.draw(dot);

    //             //
    //             line.positionVerticies(point1, point2);
    //             line.draw(window);
    //         }
    //     }

    //     window.display();

}