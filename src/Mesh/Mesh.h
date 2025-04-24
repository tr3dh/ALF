#pragma once

#include "Cell.h"

class IsoMesh{

public:

    IsoMesh();

private:

    enum class ReadMode : uint8_t;

    static const char tokenIndicator;                                                     // gibt an mit welchem Token die entsprechenden Zeilen beginnen in denen der
    static const std::string tokenIndicatorString;                                        // folgende Abschnitt definiert wird 
    static const std::string nodeToken;
    static const std::string cellToken;
    static const std::string endToken;

    std::string meshPath = NULLSTR;

    size_t nDimensions = 0;
    size_t nodeNumOffset = 0;
    std::map<NodeIndex, dynNodeXd<float>> m_Nodes = {}, m_defNodes = {};
    std::map<CellIndex, Cell> m_Cells = {};

    std::vector<uint8_t> isoKoords = {};
    std::vector<uint8_t> globKoords = {};

    Eigen::SparseMatrix<float> m_kSystem, m_uSystem, m_fSystem;

public:

    bool loadFromFile(const std::string& path);

    //
    SymEngine::DenseMatrix SymCMatrix;
    Eigen::MatrixXd CMatrix;

    //
    std::map<NodeIndex, Expression>  m_cachedJDets = {};
    std::map<NodeIndex, SymEngine::DenseMatrix> m_cachedBMats = {};

    bool createStiffnessMatrix();

    struct Force;

    void applyForces(const std::map<NodeIndex, std::vector<Force>>& externalForces);

    std::vector<NodeIndex> m_indicesToRemove = {};
    void fixNodes(const std::map<NodeIndex, std::vector<uint8_t>>& nodeFixations);

    bool readBoundaryConditions(const std::string& path = NULLSTR);

    void solve();

    //
    void calculateStrainAndStress();

    void display(const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0, bool displayOnDeformedMesh = false, bool displayOnQuadraturePoints = false,
        const sf::Vector2f& padding = {50,50});
};