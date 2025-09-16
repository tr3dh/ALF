#pragma once

#include "Mesh/Mesh.h"
#include "UncertaintyCalculation/UncertantyCalculation.h"
#include "symbolic/symbolicExpressionParser.h"
#include "Solver/NewtonRaphsonSolver.h"
#include "Serialization/JsonSerialization.h"

extern bool g_ignoreCacheValidationWhenLoadingModelCache;

// Container für die ErgebnisWerte eines Schritts der Zeitschrittintegration
struct SimulationFrame{

    //
    SimulationFrame() = default;

    SimulationFrame(const size_t& dofs){

    }

    void setupDisplacement(const size_t& dofs){
    
        displacement = Eigen::MatrixXf(dofs,1);
        displacement.fill(0);
    }

    void refillDisplacement(const std::vector<NodeIndex>& fixedDofs){

        // Für ermitteln Spannung und Dehnung muss zunächst der Spannungsvektor so aufgefüllt werden dass
        // er wieder von der Länge mit allen dofs übereinstimmt
        addDenseRows(displacement, Eigen::RowVectorXf::Zero(1), fixedDofs);
    }

    void toByteSequence(ByteSequence& seq) const;
    void fromByteSequence(ByteSequence& seq);

    //
    DataSet cellDataSet;
    Eigen::MatrixXf displacement;
    NodeSet deformedNodes;
};

template<typename JSON>
void to_json(JSON& j, const SimulationFrame& frame) {
    
    to_json(j["displacement"], frame.displacement);
    to_json(j["dataSet"], frame.cellDataSet);
    to_json(j["Deformed Nodes"], frame.deformedNodes);
}

extern Color undeformedFrame, deformedFrame, deformedFramePlusXi, deformedFrameMinusXi;

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
    void calculate();

    bool loadFromCache();
    bool loadFromFile(const std::string& path);
    void storePathInCache();

    bool loadCachedResults();
    void cacheResults();
    void validateCachedResults();

    void storeResults();

    void sampling();

    const std::vector<float>& getSamples();
    bool initialzed() const;

    const std::string& getSource() const;

    const IsoMesh& getMesh() const;
    IsoMesh& getMesh();

    const DataSet& getDataSet(int displayOnMesh = 0) const;

    void display(const MeshData& displayedData = MeshData::NONE, const int& globKoord = 0, int displayOnMesh = 0,
        const Vector2& winSize = {100,100}, const Vector2& frameOffset = {-1,-1}, const Vector2& padding = {50,50}, bool splitScreen = false, bool splitScreenVertical = true);

    void importPdf(const std::string& pdfPath);

    float m_deviation = 0, m_mean = 0;

    Vector3 modelCenter {0,0,0}, modelExtend = {0,0,0};
    float maxModelExtent = 0.0f, modelDistance = 0.0f;

    bool m_materialIsLinear = true;

public:

    std::string m_encoderKey = g_encoderKey;

    std::string m_modelPath = NULLSTR;
    std::string m_meshPath = NULLSTR;
    std::string m_matPath = NULLSTR;
    std::string m_constraintPath = NULLSTR;
    std::string m_resCachePath = NULLSTR;
    std::string m_vertexShaderPath = NULLSTR;
    std::string m_fragmentShaderPath = NULLSTR;
    std::string m_resultFilePath = NULLSTR;

private:

    std::vector<float> m_samples = {};

    IsoMesh m_isoMesh;

    NodeSet n_upperXi, n_lowerXi;
    Eigen::MatrixXf u_upperXi, u_lowerXi;
    DataSet data_upperXi, data_lowerXi;

    // Container der Simulationswerte
    std::vector<SimulationFrame> m_simulationFrames = {};

public:

    //
    bool useShader = false;

    // Zeitschrittintegration
    float m_simulationTime = 10.0f;
    float m_deltaTime = 0.1f;
    size_t m_simulationSteps = (size_t)(m_simulationTime/m_deltaTime);

    //
    bool animationPaused = false;
    int frameCounter = 0;

    //
    float minValue = 0, maxValue = 0;

    // fÜr Werte von xi plotten und berechnen
    float upperXi = 0;
    float lowerXi = 0;
};