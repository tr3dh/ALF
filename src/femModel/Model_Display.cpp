#include "Model.h"


void FemModel::display(const MeshData& displayedData, const int& globKoord, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const Vector2& winSize, const Vector2& frameOffset, const Vector2& padding, bool splitScreen, bool splitScreenVertical){

    static timePoint frameStart = chrono::now();

    static size_t frameCounter = 0; 
    static std::vector<Color*> colors = {&undeformedFrame, &deformedFrame, &deformedFramePlusXi, &deformedFrameMinusXi}; 
        
    Vector2 leftCorner = frameOffset - winSize/2, rightCorner = leftCorner + frameOffset; 

    Vector2 frameSize;
    Vector2 frameOffsets[3];

    static std::vector<NodeSet*> nodeSetsForDisplay;

    if(!m_isoMesh.getMaterial().isLinear){

        if((chrono::now() - frameStart).count() / 1000000000.0f > m_deltaTime){
            
            frameStart = chrono::now();
            
            if(frameCounter >= m_simulationSteps - 1){
                frameCounter = 0;
            } else {
                frameCounter++;
            }
        }

        auto& r_currentFrame = m_simulationFrames[frameCounter];
        nodeSetsForDisplay = {&m_isoMesh.getUndeformedNodes(), &r_currentFrame.deformedNodes};
    }
    // Check ob Material überhaupt als zufallsverteilt angenommen werden soll
    // und ob das sampling schon stattgefunden hat
    else if(m_isoMesh.getMaterial().hasPdf && !n_upperXi.empty() && !n_lowerXi.empty()){

        nodeSetsForDisplay = {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes(), &n_upperXi, &n_lowerXi};
    } else {

        nodeSetsForDisplay = {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes()};
    }

    if(m_isoMesh.getCells().begin()->second.getPrefab().nDimensions == 3){

        // 3D autofit
        Vector3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
        Vector3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        for(const auto& set : nodeSetsForDisplay)
            for (const auto& node : *set) {
                const auto& pos = node.second;
                Vector3 v = {pos[0], pos[1], pos[2]};
                if (v.x < min.x) min.x = v.x;
                if (v.y < min.y) min.y = v.y;
                if (v.z < min.z) min.z = v.z;
                if (v.x > max.x) max.x = v.x;
                if (v.y > max.y) max.y = v.y;
                if (v.z > max.z) max.z = v.z;
        }

        modelCenter = {
            (min.x + max.x) * 0.5f,
            (min.z + max.z) * 0.5f,
            (min.y + max.y) * 0.5f
        };

        modelExtend = {
            max.x - min.x,
            max.z - min.z,
            max.y - min.y,
        };

        maxModelExtent = fmaxf(fmaxf(modelExtend.x, modelExtend.y), modelExtend.z);
    }

    if(splitScreenVertical){

        // Vertikaler Split
        frameSize = {winSize.x, winSize.y/3};

        frameOffsets[0] = {frameOffset.x, leftCorner.y + 1*frameSize.y/2};
        frameOffsets[1] = {frameOffset.x, leftCorner.y + 3*frameSize.y/2};
        frameOffsets[2] = {frameOffset.x, leftCorner.y + 5*frameSize.y/2};

    } else {

        // Horizontaler Split
        frameSize = {winSize.x/3, winSize.y};

        frameOffsets[0] = {leftCorner.x + 1*frameSize.x/2, frameOffset.y};
        frameOffsets[1] = {leftCorner.x + 3*frameSize.x/2, frameOffset.y};
        frameOffsets[2] = {leftCorner.x + 5*frameSize.x/2, frameOffset.y};
    }

    for(const auto& [idx, set] : std::views::enumerate(nodeSetsForDisplay)){

        if(set->size() <= 0){
            return;
        }
        //RETURNING_ASSERT(set->size() > 0, "Angegebenes NodeSet für Rendering an Postion " + std::to_string(idx) + " ist leer",);
    }

    if(!m_isoMesh.getMaterial().isLinear){

        auto& r_currentFrame = m_simulationFrames[frameCounter];
        m_isoMesh.display(r_currentFrame.cellDataSet, displayedData, globKoord, nodeSetsForDisplay, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, winSize, frameOffset, padding);
    }
    else if(splitScreen && m_isoMesh.getUndeformedNodes().begin()->second.getDimension() == 2 && m_isoMesh.getMaterial().hasPdf){

        m_isoMesh.display(m_isoMesh.getCellData(), displayedData, globKoord, nodeSetsForDisplay, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[0], padding/3);
        m_isoMesh.display(data_upperXi, displayedData, globKoord, {&n_upperXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[1], padding/3);
        m_isoMesh.display(data_lowerXi, displayedData, globKoord, {&n_lowerXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[2], padding/3);

    } else {

        m_isoMesh.display(m_isoMesh.getCellData(), displayedData, globKoord, nodeSetsForDisplay, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, winSize, frameOffset, padding);
    }
}