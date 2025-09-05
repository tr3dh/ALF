#include "Cell.h"

Cell::Cell() = default;

Cell::Cell(const prefabIndex& prefIndex, const std::vector<NodeIndex>& nodeIndices) :
    m_prefabIndex(prefIndex), m_nodeIndices(nodeIndices){

    //
    ASSERT(m_nodeIndices.size() == getPrefab().nNodes, "Ungültige Anzahl an Nodes übergeben, " +
        std::to_string(getPrefab().nNodes) + " erforderlich " + std::to_string(m_nodeIndices.size()) + " erhalten");
}

// nicht const Koordinaten abfrage -> Einträge können über zurückgegebene Referenz bearbeitet werden
const NodeIndex& Cell::operator[](size_t index) const{

    CRITICAL_ASSERT(index < m_nodeIndices.size(), "Abgefragter nodeIndex " + std::to_string(index) + " existiert in Element nicht");
    return m_nodeIndices[index];
}

// Logging
std::ostream& operator<<(std::ostream& os, const Cell& cell) {

    os << "Cell-" << cell.getPrefab().label << " {";
    for (size_t i = 0; i < cell.m_nodeIndices.size(); i++){
        if (i > 0) os << ", ";
        os << cell.m_nodeIndices[i];
    }
    os << "}";
    return os;
}

prefabIndex Cell::getPrefabIndex() const{
    return m_prefabIndex;
}

const CellPrefab& Cell::getPrefab() const{
    return getCachedCellPrefab(m_prefabIndex);
}

void Cell::toByteSequence(ByteSequence& seq) const {
    seq.insertMultiple(m_nodeIndices,m_prefabIndex);
}

void Cell::fromByteSequence(ByteSequence& seq){
    seq.extractMultiple(m_prefabIndex,m_nodeIndices);
}