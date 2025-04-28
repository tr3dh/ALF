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

    const std::vector<float>& getSamples();

private:

    std::string m_modelPath = NULLSTR;
    std::string m_meshPath = NULLSTR;
    std::vector<float> m_samples = {};
};