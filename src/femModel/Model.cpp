#include "Model.h"

const std::string FemModel::fileSuffix = ".model";
const std::string FemModel::cachePath = "../bin/MODEL.CACHE";

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

    const std::string linMatPath = string::split(fs::path(path).filename().string(),'.')[0] + IsoMeshMaterial::fileSuffix;
    
    m_materialIsLinear = (fs::exists(linMatPath));

    m_isoMesh.setSelfPointer(&this->m_isoMesh);
    m_isoMesh.loadFromFile(m_meshPath);
    m_isoMesh.loadIsoMeshMaterial();

    if(m_isoMesh.createStiffnessMatrix()){
        
        m_isoMesh.readBoundaryConditions();
        m_isoMesh.solve();

        m_isoMesh.calculateStrainAndStress();
    }

    // roadmap unsicherheitsanalyse
    sampling();
}

void FemModel::importPdf(const std::string& pdfPath){

    

}

void FemModel::display(const MeshData& displayedData, const int& globKoord, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const Vector2& winSize, const Vector2& frameOffset, const Vector2& padding, bool splitScreen, bool splitScreenVertical){

    static std::vector<Color*> colors = {&undeformedFrame, &deformedFrame, &deformedFramePlusXi, &deformedFrameMinusXi}; 
        
    Vector2 leftCorner = frameOffset - winSize/2, rightCorner = leftCorner + frameOffset; 

    Vector2 frameSize;
    Vector2 frameOffsets[3];

    if(m_isoMesh.getCells().begin()->second.getPrefab().nDimensions == 3){

        // 3D autofit
        Vector3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
        Vector3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        NodeSet undefNodes = m_isoMesh.getUndeformedNodes(), defNodes = m_isoMesh.getDeformedNodes();
        std::vector<NodeSet*> sets = {&undefNodes, &defNodes, &n_upperXi, &n_lowerXi};
        
        for(const auto& set : sets)
            for (const auto& node : *set) {
                const auto& pos = node.second;
                Vector3 v = {pos[0], pos[1], pos[2]};
                if (v.x < min.x) min.x = v.x;
                if (v.y < min.y) min.y = v.y;
                if (v.z < min.z) min.z = v.z;
                if (v.x > max.x) max.x = v.x;
                if (v.y > max.y) max.y = v.y;
                if (v.z > max.z) max.z = v.z;
        }

        modelCenter = {
            (min.x + max.x) * 0.5f,
            (min.z + max.z) * 0.5f,
            (min.y + max.y) * 0.5f
        };

        modelExtend = {
            max.x - min.x,
            max.z - min.z,
            max.y - min.y,
        };

        maxModelExtent = fmaxf(fmaxf(modelExtend.x, modelExtend.y), modelExtend.z);
    }

    if(splitScreenVertical){

        // Vertikaler Split
        frameSize = {winSize.x, winSize.y/3};

        frameOffsets[0] = {frameOffset.x, leftCorner.y + 1*frameSize.y/2};
        frameOffsets[1] = {frameOffset.x, leftCorner.y + 3*frameSize.y/2};
        frameOffsets[2] = {frameOffset.x, leftCorner.y + 5*frameSize.y/2};

    } else {

        // Horizontaler Split
        frameSize = {winSize.x/3, winSize.y};

        frameOffsets[0] = {leftCorner.x + 1*frameSize.x/2, frameOffset.y};
        frameOffsets[1] = {leftCorner.x + 3*frameSize.x/2, frameOffset.y};
        frameOffsets[2] = {leftCorner.x + 5*frameSize.x/2, frameOffset.y};
    }

    if(splitScreen && m_isoMesh.getUndeformedNodes().begin()->second.getDimension() == 2){

        m_isoMesh.display(m_isoMesh.getCellData(), displayedData, globKoord, {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes(), &n_upperXi, &n_lowerXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[0], padding/3);
        m_isoMesh.display(data_upperXi, displayedData, globKoord, {&n_upperXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[1], padding/3);
        m_isoMesh.display(data_lowerXi, displayedData, globKoord, {&n_lowerXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[2], padding/3);

    } else {

        m_isoMesh.display(m_isoMesh.getCellData(), displayedData, globKoord, {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes(), &n_upperXi, &n_lowerXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, winSize, frameOffset, padding);
    }
}

void FemModel::sampling(){

    // Monte Carlo für pdf
    const IsoMeshMaterial& mat = m_isoMesh.getMaterial();

    m_samples.clear();
    rejectionSampling(mat.pdf_xi, m_samples, mat.nSamples, mat.xi_min, mat.xi_max, mat.tolerance, mat.segmentation);

    //
    std::tie(m_mean, m_deviation) = processSamples(m_samples);

    // fÜr Werte von xi plotten und berechnen
    float upperXi = m_deviation;
    float lowerXi = -m_deviation;

    //
    u_upperXi = m_isoMesh.getDisplacement().toDense() * (1-upperXi);
    u_lowerXi = m_isoMesh.getDisplacement().toDense() * (1-lowerXi);

    //
    n_upperXi = m_isoMesh.getUndeformedNodes();
    n_lowerXi = n_upperXi;

    IsoMesh::displaceNodes(n_upperXi, u_upperXi.sparseView(), n_upperXi.begin()->first);
    IsoMesh::displaceNodes(n_lowerXi, u_lowerXi.sparseView(), n_lowerXi.begin()->first);

    acvanceDataSet(m_isoMesh.getCellData(), data_upperXi, {1.0f-upperXi,1.0f-upperXi,1.0f-upperXi*upperXi});
    acvanceDataSet(m_isoMesh.getCellData(), data_lowerXi, {1.0f-lowerXi,1.0f-lowerXi,1.0f-lowerXi*lowerXi});
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