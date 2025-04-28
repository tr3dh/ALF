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

    nlohmann::json j;
    j["FemModel"] = path;
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
    LOG << endl;
}

bool FemModel::loadFromFile(const std::string& path){

    //
    RETURNING_ASSERT(path != NULLSTR, "angegebener Pfadstring ist nicht initialisiert",false);
    RETURNING_ASSERT(fs::is_directory(path), "angegebener Pfad ist kein Ordner", false);
    RETURNING_ASSERT(string::endsWith(path, fileSuffix), "invalide Pfadendung",false);

    // reset
    *this = FemModel(path);

    //
    return true;
}

void FemModel::storePathInCache(){
    
    cacheFilePath(m_modelPath);
}