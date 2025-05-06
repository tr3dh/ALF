#include "Mesh.h"

void acvanceDataSet(const DataSet& source, DataSet& target, const Coeffs& coeffs){

    // alle Einträge in neues Set kopieren
    target = source;

    //  Koeffizienten auf alle einträge in der neuen map beaufsclagen
    for(auto& [_, data] : target){
        data *= coeffs;
    }
}

const NodeSet& IsoMesh::getUndeformedNodes() const{
    return m_nodes;
}

const NodeSet& IsoMesh::getDeformedNodes() const{
    return m_defNodes;
}

const DataSet& IsoMesh::getCellData() const{
    return m_cellData;
}

const Eigen::SparseMatrix<float>& IsoMesh::getDisplacement() const{
    return m_uSystem;
}

const IsoMeshMaterial& IsoMesh::getMaterial() const{
    return m_material;
}

IsoMeshMaterial& IsoMesh::getMaterial(){
    return m_material;
}