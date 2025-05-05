#include "Model.h"

const std::string FemModel::fileSuffix = ".model";
const std::string FemModel::cachePath = "../bin/MODEL.CACHE";

std::string FemModel::getPathFromCache(){

    // in ../bin nach cache suchen
    if(fs::exists(cachePath)){

        std::ifstream file(cachePath);
        nlohmann::json j = nlohmann::json::parse(file);

        return j.value("FemModel",NULLSTR);
    }

    return NULLSTR;
}

void FemModel::cacheFilePath(const std::string& path){

    std::string parentDir = fs::current_path().parent_path().string();

    nlohmann::json j;
    j["FemModel"] = ".." + path.substr(string::findFirst(path, parentDir) + parentDir.size(), path.size());
    std::ofstream file(cachePath);
    file << j.dump(4); // 4 für Einrücken
}

FemModel::FemModel() = default;

bool FemModel::loadFromCache(){

    //
    const std::string modelPath = getPathFromCache();

    if(modelPath != NULLSTR){
        return loadFromFile(modelPath);
    }

    return false;
}

void FemModel::reload(){

    if(initialzed()){
        *this = FemModel(m_modelPath);
    } else {
        _ERROR << "kein valider Pfad im Modell hinterlegt\n" << endl;
    }
}

void FemModel::unload(){
    *this = FemModel();
}

FemModel::FemModel(const std::string& path) : m_modelPath(path){

    //
    LOG << "-- Modell aus " << m_modelPath << " konstruiert" << endl;

    m_meshPath = path + "/" + string::split(fs::path(path).filename().string(),'.')[0] + ".inp";
    LOG << "   Lade Mesh aus " << m_meshPath << endl;
    LOG << endl;

    m_isoMesh.loadFromFile(m_meshPath);
    m_isoMesh.loadIsoMeshMaterial();

    if(m_isoMesh.createStiffnessMatrix()){

        m_isoMesh.readBoundaryConditions();

        m_isoMesh.solve();
        m_isoMesh.calculateStrainAndStress();
    }

    // roadmap unsicherheitsanalyse
    sampling();

    // fÜr Werte von xi plotten und berechnen
    int Xi = 0.1;

    DataSet advancedData;
    acvanceDataSet(m_isoMesh.getCellData(), advancedData, {1.0f-Xi,1.0f-Xi,1.0f-Xi*Xi});

    NodeSet n0 = m_isoMesh.getUndeformedNodes();
    Eigen::SparseMatrix<float> u_xi = m_isoMesh.getDisplacement() * (1-Xi);

    IsoMesh::displaceNodes(n0, u_xi, n0.begin()->first);
}

void FemModel::display(const MeshData& displayedData, const int& globKoord, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const Vector2& winSize, const Vector2& frameOffset, const Vector2& padding){

        m_isoMesh.display(displayedData, globKoord, {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes()}, {WHITE, RED, GREEN, YELLOW}, 0, displayOnDeformedMesh, displayOnQuadraturePoints, winSize, frameOffset, padding);
}

void FemModel::sampling(){

    // Monte Carlo für pdf
    const IsoMeshMaterial& mat = m_isoMesh.getMaterial();
    rejectionSampling(mat.pdf_xi, m_samples, mat.nSamples, mat.xi_min, mat.xi_max, mat.tolerance, mat.segmentation);

    //
    auto [mean, deviation] = processSamples(m_samples);
    m_mean = mean;
    m_deviation = deviation;
}

bool FemModel::loadFromFile(const std::string& path){

    //
    RETURNING_ASSERT(path != NULLSTR, "angegebener Pfadstring ist nicht initialisiert",false);
    RETURNING_ASSERT(fs::is_directory(path), "angegebener Pfad " + path + " ist kein Ordner", false);
    RETURNING_ASSERT(string::endsWith(path, fileSuffix), "invalide Pfadendung",false);

    // reset
    *this = FemModel(path);
    
    //
    return true;
}

void FemModel::storePathInCache(){
    
    cacheFilePath(m_modelPath);
}

const std::vector<float>& FemModel::getSamples(){
    return m_samples;
}

bool FemModel::initialzed() const{

    return m_modelPath != NULLSTR;
}

const std::string& FemModel::getSource() const{
    return m_modelPath;
};

const IsoMesh& FemModel::getMesh() const{
    return m_isoMesh;
}

IsoMesh& FemModel::getMesh(){
    return m_isoMesh;
}