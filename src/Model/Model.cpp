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

FemModel::FemModel(){

    //
    const std::string modelPath = getPathFromCache();
    
    if(modelPath != NULLSTR){
        loadFromFile(modelPath);
    }
};

FemModel::FemModel(const std::string& path) : m_modelPath(path){

    //
    LOG << "-- Modell aus " << m_modelPath << " konstruiert" << endl;

    m_meshPath = path + "/" + string::split(fs::path(path).filename().string(),'.')[0] + ".inp";
    LOG << "   Lade Mesh aus " << m_meshPath << endl;
    LOG << endl;

    IsoMesh isomesh;
    isomesh.loadFromFile(m_meshPath);
    isomesh.loadIsoMeshMaterial();

    if(isomesh.createStiffnessMatrix()){

        isomesh.readBoundaryConditions();

        isomesh.solve();
        isomesh.calculateStrainAndStress();

        //isomesh.display(MeshData::VANMISES_STRESS, 0, false, false, {100,100});
    }

    // roadmap unsicherheitsanalyse

    // für zufallsverteilte variable xi
    int Xi = 0.1;

    DataSet advancedData;
    acvanceDataSet(isomesh.getCellData(), advancedData, {1.0f-Xi,1.0f-Xi,1.0f-Xi*Xi});

    NodeSet n0 = isomesh.getUndeformedNodes();
    Eigen::SparseMatrix<float> u_xi = isomesh.getDisplacement() * (1-Xi);

    IsoMesh::displaceNodes(n0, u_xi, n0.begin()->first);

    // Monte Carlo für pdf
    rejectionSampling(isomesh.getMaterial().pdf, m_samples, 100000);

    //
    processSamples(m_samples);
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