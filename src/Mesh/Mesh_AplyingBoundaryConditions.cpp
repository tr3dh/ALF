#include "Mesh.h"

void IsoMesh::applyForces(const std::map<NodeIndex, std::vector<Force>>& externalForces){

    //
    LOG << "-- Aplying m_loads ..." << ENDL;

    m_fSystem = Eigen::SparseMatrix<float>(m_nodes.size() * nDimensions, 1);

    for(const auto& [index, forces] : externalForces){
        for(const auto& force : forces){

            CRITICAL_ASSERT(force.direction < nDimensions, "Ungültige Richtungsangebe");
            LOG << "   Node " << +index << " Force " << force.amount << " N\t\tin direction " << g_globalKoords[force.direction] << ENDL;
            loadTriplets.emplace_back((index-nodeNumOffset) * nDimensions + force.direction, 0, force.amount);
        }
    }

    LOG << ENDL;

    m_fSystem.setFromTriplets(loadTriplets.begin(), loadTriplets.end());
}

void IsoMesh::fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations){

    //
    LOG << "-- Aplying node Constraints ..." << ENDL;

    m_indicesToRemove.clear();
    m_uSystem = Eigen::MatrixXf(m_nodes.size() * nDimensions, 1);

    for(const auto& [index, dirVec] : nodeFixations){
        for(const auto& direction : dirVec){

            //
            CRITICAL_ASSERT(direction < nDimensions, "Ungültige Richtungsangebe");
            LOG << "   Fixing node " << index << "\t\t\tin direction " << g_globalKoords[direction] << ENDL;

            m_indicesToRemove.emplace_back(nDimensions * (index - nodeNumOffset) + direction);
        }
    }

    LOG << ENDL;

    m_indicesToAdd = m_indicesToRemove;

    // Absteigend und aufsteigend sortieren
    std::sort(m_indicesToRemove.begin(), m_indicesToRemove.end(), std::greater<NodeIndex>());       // Absteigend
    std::sort(m_indicesToAdd.begin(), m_indicesToAdd.end());                                        // Aufsteigend

    LOG << "-- Removing indices {";
    for(const auto& i : m_indicesToRemove){
        LOG << i << ",";
    }
    LOG << "}" << ENDL;
    LOG << ENDL;

    removeDenseRows(m_uSystem, m_indicesToRemove);
    removeSparseRow(m_fSystem, m_indicesToRemove);
    removeSparseRowAndCol<NodeIndex>(m_kSystem, m_indicesToRemove);

    CRITICAL_ASSERT(m_fSystem.rows() == m_kSystem.rows() && m_uSystem.rows() == m_kSystem.rows(), "Matritzen Dimensionen von u,f und K stimmen nicht überein");
}

bool IsoMesh::readBoundaryConditions(bool apply, const std::string& path){
        
    std::string boundaryFilePath = path;
    if(boundaryFilePath == NULLSTR){
        boundaryFilePath = meshPath.substr(0, meshPath.find_last_of('.')) + ".Constraints";
    }        

    CRITICAL_ASSERT(string::endsWith(boundaryFilePath, ".Constraints"), "Übergebener Pfad endet auf ungültige File Endung, erwartetes format : *.fem");

    LOG << LOG_BLUE << "-- Reading file : " << boundaryFilePath << ENDL;
    LOG << ENDL;

    // Check ob Pfad existiert
    CRITICAL_ASSERT(fs::exists(boundaryFilePath), "Angegebener Pfad : '" + boundaryFilePath + "' existiert nicht");

    // Check ob fstream den file ohne Fehler geladen bekommt
    std::ifstream BoundaryConditionFile(boundaryFilePath);
    CRITICAL_ASSERT(BoundaryConditionFile, "Fehler beim Laden von '" + path + "'");

    // Check ob nlohmann json den file geparst bekommt
    nlohmann::json ffData = nlohmann::json::parse(BoundaryConditionFile, nullptr, true, true);
    
    //
    if(!ffData.contains("Constraints") || !ffData["Constraints"].is_array()){

        ASSERT(TRIGGER_ASSERT, "Constraints konnten nicht gelesen werden");

        return false;  
    }

    for(const auto& constraint : ffData["Constraints"]){

        for(const auto& [node, dirs] : constraint.items()){

            m_constraints.try_emplace(string::convert<NodeIndex>(node), dirs.get<std::vector<uint8_t>>());
        }
    }

    //
    if(!ffData.contains("Loads") || !ffData["Loads"].is_array()){

        ASSERT(TRIGGER_ASSERT, "Loads konnten nicht gelesen werden");

        return false;  
    }

    for(const auto& load : ffData["Loads"]){

        for(const auto& [node, forceList] : load.items()){

            const NodeIndex index = string::convert<NodeIndex>(node);
            m_loads.try_emplace(index);
            
            for(const auto& [_, force] : forceList.items()){

                for(const auto& [dir, amount] : force.items()){

                    const uint8_t direction = string::convert<uint8_t>(dir);
                    for(const auto& [_,isolatedAmount] : amount.items()){
                    
                        m_loads[index].emplace_back(direction, isolatedAmount.get<float>());
                    }
                }
            }
        }
    }

    if(!apply){
        return true;
    }

    applyForces(m_loads);
    fixNodes(m_constraints);

    return true;
}