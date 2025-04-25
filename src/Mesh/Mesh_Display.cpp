#include "Mesh.h"

void IsoMesh::display(const MeshData& displayedData, const int& globKoord, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const sf::Vector2f& padding){

    if(nDimensions != 2){
        ASSERT(TRIGGER_ASSERT, "Rendering bislang nur für 2D implementiert");
    }

    // Render Window
    sf::RenderWindow window(sf::VideoMode(1800,1000), std::string(magic_enum::enum_name(displayedData)) + " - " + g_globalKoords[globKoord]->__str__());
    window.setFramerateLimit(0);

    sf::Image icon;
    if (!icon.loadFromFile("../Recc/Compilation/icon.png")) {
        // ...
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Autofit
    sf::Vector2f min = {m_nodes.begin()->second[0], m_nodes.begin()->second[1]};
    sf::Vector2f max = min;

    for(const auto& [index, node] : m_nodes){

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
    sf::Vector2f midOfWin = {winSize.x/2.0f, winSize.y/2.0f};

    sf::Vector2f offset = {-(midOfWin.x - scaling * midOfMesh.x), -(midOfWin.y - (scaling * midOfMesh.y))};

    float fData = 0.0f, fmin = 0, fmax = 0;

    if(displayOnQuadraturePoints){

        for(const auto& [cellIndex, cell] : m_Cells){

            const CellData& data = m_cellData.at(cellIndex);

            for(size_t localNodeNum = 0; localNodeNum < cell.getPrefab().nNodes; localNodeNum++){

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
        
            const CellData& data = m_cellData.at(cellIndex);
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
        LOG << "   maximaler " << magic_enum::enum_name(displayedData) << " : " << fmax << " bei Elem " << maxInd << endl;
        LOG << "   minimaler " << magic_enum::enum_name(displayedData) << " : " << fmin << " bei Elem " << minInd << endl;
        LOG << endl;
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

        // sfQuad quad;

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
                node1 = (displayOnDeformedMesh ? m_defNodes : m_nodes)[cell[nodeNum1]];
                point1 = {(node1[0] * scaling) - offset.x, (node1[1] * scaling) - offset.y};
                point1.y = window.getSize().y - point1.y;

                //
                points.emplace_back(point1);
            }

            if(!displayOnQuadraturePoints){

                poly.setPoints(points);
                poly.setFillColor(getColorByValue(m_cellData.at(index).getData(displayedData, globKoord), fmin, fmax));
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

                    // quad.positionVerticies(subpoints);
                    // quad.colorVerticies(getColorByValue(cell.getCellData().getData(displayedData, globKoord, localNodeNum), fmin, fmax));
                    // quad.draw(window);
                }
            }
        }

        // undef mesh
        dot.setFillColor(sf::Color::White);
        line.colorVerticies(sf::Color::White);
        for(const auto& [index, cell] : m_Cells){

            const CellPrefab& r_prefab = cell.getPrefab();

            for(uint8_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

                // Refs für weniger overhead
                nodeNum1 = localNodeNum;
                nodeNum2 = (localNodeNum == r_prefab.nNodes - 1) ? 0 : localNodeNum + 1;

                node1 = m_nodes[cell[nodeNum1]];
                node2 = m_nodes[cell[nodeNum2]];

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

            for(uint8_t localNodeNum = 0; localNodeNum < r_prefab.nNodes; localNodeNum++){

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