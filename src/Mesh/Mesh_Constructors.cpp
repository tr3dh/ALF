#include "Mesh.h"

IsoMesh::IsoMesh() = default;

void IsoMesh::toByteSequence(ByteSequence& seq) const {

    seq.insertMultiple(
        nDimensions,
        nodeNumOffset,
        m_nodes,
        m_defNodes,
        m_cellData,
        m_Cells,
        isoKoords,
        globKoords,
        m_constraints,
        m_loads,
        loadTriplets,
        m_kSystem,
        m_fSystem,
        m_uSystem,
        m_cachedJDets,
        // m_cachedBMats,
        m_indicesToRemove,
        m_indicesToAdd,
        m_matPath,
        // m_material,
        SymCMatrix,
        CMatrix
    );
}

void IsoMesh::fromByteSequence(ByteSequence& seq) {

    seq.extractMultiple(
        CMatrix,
        SymCMatrix,
        // m_material,
        m_matPath,
        m_indicesToAdd,
        m_indicesToRemove,
        // m_cachedBMats,   hier kommst zum segfault, aber auch nur während des reloads in der UI
        m_cachedJDets,
        m_uSystem,
        m_fSystem,
        m_kSystem,
        loadTriplets,
        m_loads,
        m_constraints,
        globKoords,
        isoKoords,
        m_Cells,
        m_cellData,
        m_defNodes,
        m_nodes,
        nodeNumOffset,
        nDimensions
    );
}