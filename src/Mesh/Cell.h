#pragma once

#include "CellData.h"

class Cell{

public:

    Cell();

    Cell(const prefabIndex& prefIndex, const std::vector<NodeIndex>& nodeIndices);

    // nicht const Koordinaten abfrage -> Einträge können über zurückgegebene Referenz bearbeitet werden
    const NodeIndex& operator[](size_t index) const;

    // Logging
    friend std::ostream& operator<<(std::ostream& os, const Cell& cell);

    prefabIndex getPrefabIndex() const;

    const CellPrefab& getPrefab() const;

    void toByteSequence(ByteSequence& seq) const;
    void fromByteSequence(ByteSequence& seq);

private:

    prefabIndex m_prefabIndex = 0;
    std::vector<NodeIndex> m_nodeIndices = {};
};

typedef std::map<CellIndex, Cell> CellSet;