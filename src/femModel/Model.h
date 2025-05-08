#pragma once

#include "Mesh/Mesh.h"
#include "UncertaintyCalculation/UncertantyCalculation.h"

class FemModel{

public:

    static std::string getPathFromCache();
    static void cacheFilePath(const std::string& path);

    const static std::string fileSuffix;
    const static std::string cachePath;

    FemModel();
    FemModel(const std::string& path);
    void reload();
    void unload();

    bool loadFromCache();
    bool loadFromFile(const std::string& path);
    void storePathInCache();

    void sampling();

    const std::vector<float>& getSamples();
    bool initialzed() const;

    const std::string& getSource() const;

    const IsoMesh& getMesh() const;
    IsoMesh& getMesh();

    void display(const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0, bool displayOnDeformedMesh = false, bool displayOnQuadraturePoints = false,
        const Vector2& winSize = {100,100}, const Vector2& frameOffset = {-1,-1}, const Vector2& padding = {50,50}, bool splitScreen = false, bool splitScreenVertical = true);

    void importPdf(const std::string& pdfPath);

    float m_deviation = 0, m_mean = 0;

private:

    std::string m_modelPath = NULLSTR;
    std::string m_meshPath = NULLSTR;
    std::string m_matPath = NULLSTR;
    std::vector<float> m_samples = {};

    IsoMesh m_isoMesh;

    NodeSet n_upperXi, n_lowerXi;
    Eigen::MatrixXf u_upperXi, u_lowerXi;
    DataSet data_upperXi, data_lowerXi;
};