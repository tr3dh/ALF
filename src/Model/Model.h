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

    bool loadFromFile(const std::string& path);
    void storePathInCache();

private:

    std::string m_modelPath = NULLSTR;
};