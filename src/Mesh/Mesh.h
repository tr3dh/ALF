#pragma once

#include "Cell.h"
#include "Material.h"
#include "Rendering/CellRenderer.h"

void acvanceDataSet(const DataSet& source, DataSet& target, const Coeffs& coeffs);

class IsoMesh{

public:

    IsoMesh();

private:

    enum class ReadMode : uint8_t;

    struct Force;

    static const char tokenIndicator;                                                     // gibt an mit welchem Token die entsprechenden Zeilen beginnen in denen der
    static const std::string tokenIndicatorString;                                        // folgende Abschnitt definiert wird 
    static const std::string nodeToken;
    static const std::string cellToken;
    static const std::string endToken;

    std::string meshPath = NULLSTR;

    size_t nDimensions = 0;
    size_t nodeNumOffset = 0;
    NodeSet m_nodes = {}, m_defNodes = {};
    DataSet m_cellData;
    CellSet m_Cells = {};

    std::vector<uint8_t> isoKoords = {};
    std::vector<uint8_t> globKoords = {};

    Eigen::SparseMatrix<float> m_kSystem, m_uSystem, m_fSystem;

    //
    SymEngine::DenseMatrix SymCMatrix;
    Eigen::MatrixXd CMatrix;

    //
    std::map<NodeIndex, Expression>  m_cachedJDets = {};
    std::map<NodeIndex, SymEngine::DenseMatrix> m_cachedBMats = {};

    std::vector<NodeIndex> m_indicesToRemove = {};

    std::string m_matPath = NULLSTR;
    IsoMeshMaterial m_material;

public:

    bool loadFromFile(const std::string& path);

    bool loadIsoMeshMaterial(const std::string& path = NULLSTR);

    bool createStiffnessMatrix();

    void applyForces(const std::map<NodeIndex, std::vector<Force>>& externalForces);

    void fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations);

    bool readBoundaryConditions(const std::string& path = NULLSTR);

    static void displaceNodes(NodeSet& nodes, const Eigen::SparseMatrix<float>& displacement, size_t nodeNumOffset);
    void solve();

    void calculateStrainAndStress(bool calculateOnQuadraturePoints = false);

    void display(const DataSet& dataSet, const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0,
        const std::vector<const NodeSet*>& nodeSets = {}, const std::vector<Color*> setColors = {}, int displayOnNodeSet = 0,
        bool displayOnDeformedMesh = false, bool displayOnQuadraturePoints = false,
        const Vector2& winSize = {100,100}, const Vector2& frameOffset = {-1,-1}, const Vector2& padding = {50,50});

    const NodeSet& getUndeformedNodes() const;
    const NodeSet& getDeformedNodes() const;
    const CellSet& getCells() const;
    const Eigen::SparseMatrix<float>& getStiffnesMatrix() const;
    const DataSet& getCellData() const;
    const Eigen::SparseMatrix<float>& getDisplacement() const;

    void saveMaterial();
    const std::string& getMaterialPath();
    const IsoMeshMaterial& getMaterial() const;
    IsoMeshMaterial& getMaterial();

    //
    void setSelfPointer(IsoMesh* ptr);
    IsoMesh* getSelfPointer();
    IsoMesh* g_self = nullptr;
};