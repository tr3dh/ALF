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

    struct Force{

        uint8_t direction = -1;
        float amount = 0;
    };

    static const char tokenIndicator;                                                     // gibt an mit welchem Token die entsprechenden Zeilen beginnen in denen der
    static const std::string tokenIndicatorString;                                        // folgende Abschnitt definiert wird 
    static const std::string nodeToken;
    static const std::string cellToken;
    static const std::string endToken;

    std::string meshPath = NULLSTR;

public:
    size_t nDimensions = 0;
    size_t nodeNumOffset = 0;
    NodeSet m_nodes = {}, m_defNodes = {};
    DataSet m_cellData;
    CellSet m_Cells = {};

public:

    std::vector<uint8_t> isoKoords = {};
    std::vector<uint8_t> globKoords = {};

    std::map<NodeIndex, std::vector<uint8_t>> m_constraints = {};
    std::map<NodeIndex, std::vector<Force>> m_loads = {};

public:

    //
    std::vector<Eigen::Triplet<float>> loadTriplets = {};
    Eigen::SparseMatrix<float> m_kSystem, m_fSystem;
    Eigen::MatrixXf m_uSystem;
    std::map<NodeIndex, Expression>  m_cachedJDets = {};
    std::map<NodeIndex, SymEngine::DenseMatrix> m_cachedBMats = {};

    std::vector<NodeIndex> m_indicesToRemove = {};
    std::vector<NodeIndex> m_indicesToAdd = {};

    std::string m_matPath = NULLSTR;
    IsoMeshMaterial m_material;

public:

    //
    SymEngine::DenseMatrix SymCMatrix;
    Eigen::MatrixXf CMatrix;

    bool loadFromFile(const std::string& path);

    bool loadIsoMeshMaterial(const std::string& path = NULLSTR);

    bool createStiffnessMatrix();

    void applyForces(const std::map<NodeIndex, std::vector<Force>>& externalForces);

    void fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations);

    bool readBoundaryConditions(bool apply = true, const std::string& path = NULLSTR);

    static void displaceNodes(NodeSet& nodes, const Eigen::MatrixXf& displacement, size_t nodeNumOffset);
    void solve();

    void calculateStrainAndStress(DataSet& dataSet, const Eigen::MatrixXf& displacement, bool calculateOnQuadraturePoints = false);

    void display(const DataSet& dataSet, const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0,
        const std::vector<NodeSet*>& nodeSets = {}, const std::vector<Color*> setColors = {}, int displayOnNodeSet = 0,
        bool displayOnDeformedMesh = false, bool displayOnQuadraturePoints = false,
        const Vector2& winSize = {100,100}, const Vector2& frameOffset = {-1,-1}, const Vector2& padding = {50,50});

    NodeSet& getUndeformedNodes();
    NodeSet& getDeformedNodes();
    const NodeSet& getUndeformedNodes() const;
    const NodeSet& getDeformedNodes() const;
    const CellSet& getCells() const;
    const Eigen::SparseMatrix<float>& getStiffnesMatrix() const;
    const DataSet& getCellData() const;
    const Eigen::MatrixXf& getDisplacement() const;

    void saveMaterial();
    const std::string& getMaterialPath();
    const IsoMeshMaterial& getMaterial() const;
    IsoMeshMaterial& getMaterial();

    //
    void setSelfPointer(IsoMesh* ptr);
    IsoMesh* getSelfPointer();
    IsoMesh* g_self = nullptr;
};