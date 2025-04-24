#include "Cell.h"

Cell::Cell() : m_cellPrefab(CellPrefab::nullRef), m_cellData(m_cellPrefab){};

Cell::Cell(const prefabIndex& prefIndex, const std::vector<NodeIndex>& nodeIndices) :
    m_prefabIndex(prefIndex), m_nodeIndices(nodeIndices), m_cellPrefab(getCachedCellPrefab(prefIndex)),
    m_cellData(m_cellPrefab){

    //
    ASSERT(m_nodeIndices.size() == m_cellPrefab.nNodes, "Ungültige Anzahl an Nodes übergeben, " +
        std::to_string(m_cellPrefab.nNodes) + " erforderlich " + std::to_string(m_nodeIndices.size()) + " erhalten");
}

// nicht const Koordinaten abfrage -> Einträge können über zurückgegebene Referenz bearbeitet werden
const NodeIndex& Cell::operator[](size_t index) const{

    CRITICAL_ASSERT(index < m_nodeIndices.size(), "Abgefragter nodeIndex existiert in Element nicht");
    return m_nodeIndices[index];
}

// Logging
std::ostream& operator<<(std::ostream& os, const Cell& cell) {

    os << "Cell-" << cell.m_cellPrefab.label << " {";
    for (const auto& [i, index] : std::views::enumerate(cell.m_nodeIndices)) {
        if (i > 0) os << ", ";
        os << index;
    }
    os << "}";
    return os;
}

prefabIndex Cell::getPrefabIndex() const{
    return m_prefabIndex;
}

const CellPrefab& Cell::getPrefab() const{
    return m_cellPrefab;
}

CellData& Cell::getCellData(){
    return m_cellData;
}

const CellData& Cell::getCellData() const{
    return m_cellData;
}