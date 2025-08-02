#include "Model.h"

const std::string FemModel::fileSuffix = ".model";
const std::string FemModel::cachePath = "../bin/MODEL.CACHE";

void SimulationFrame::toByteSequence(ByteSequence& seq) const{
    seq.insertMultiple(cellDataSet, displacement, deformedNodes);
}

void SimulationFrame::fromByteSequence(ByteSequence& seq){
    seq.extractMultiple(deformedNodes, displacement, cellDataSet);
}

std::string FemModel::getPathFromCache(){

    // in ../bin nach cache suchen
    if(fs::exists(cachePath)){

        std::ifstream file(cachePath);
        nlohmann::json j = nlohmann::json::parse(file, nullptr, true, true);

        return j.value("FemModel",NULLSTR);
    }

    return NULLSTR;
}

void FemModel::cacheFilePath(const std::string& path){

    //
    if(!fs::exists(fs::path(FemModel::cachePath).parent_path())){
        fs::create_directory(fs::path(FemModel::cachePath).parent_path());
    }

    nlohmann::json j;
    j["FemModel"] = fs::relative(path, fs::current_path());
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

        const std::string modelPath = m_modelPath;

        unload();
        loadFromFile(modelPath);
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

    m_meshPath = path + "/" +  ".Mesh";
    LOG << "   Lade Mesh aus " << m_meshPath << endl;
    LOG << endl;

    m_resCachePath = path + "/" + ".RESULTCACHE";
    m_constraintPath = path + "/" + ".Constraints";
    m_matPath = path + "/" + ".Material";

    // Laden aller files damit gechachte Prefab Indices ihre Gültigkeit beibehalten
    for (const auto& entry : fs::directory_iterator("../Recc/Cells")) {
        if (entry.is_regular_file()) {

            // name ohne dateiendung
            cacheCellPrefab(entry.path().stem().string());
        }
    }

    //
    m_isoMesh.loadFromFile(m_meshPath);
    m_isoMesh.loadIsoMeshMaterial();

    //
    if(m_isoMesh.nDimensions == 3){

        initCellRenderer(m_isoMesh.m_Cells.begin()->second.getPrefab());

        // Shader setup
        // ...
    }

    // aus Cache laden
    if(loadCachedResults()){
        return;
    }

    calculate();
}

void FemModel::importPdf(const std::string& pdfPath){

    

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