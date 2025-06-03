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
        loadFromFile(m_modelPath);
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

    m_isoMesh.setSelfPointer(&this->m_isoMesh);
    m_isoMesh.loadFromFile(m_meshPath);
    m_isoMesh.loadIsoMeshMaterial();

    if(m_isoMesh.getMaterial().isLinear){

        RETURNING_ASSERT(m_isoMesh.createStiffnessMatrix(), "KMatrix Erstellung fehlgeschlagen",);
        
        m_isoMesh.readBoundaryConditions();
        m_isoMesh.solve();

        m_isoMesh.calculateStrainAndStress();

        // if mat has pdf einfügen
        if(m_isoMesh.getMaterial().hasPdf){
            sampling();
        }

    } else {

        // KMatrix wird hier erstellt um BMatrix und Jacoby Matrix caches zu füllen, da diese Matritzen in jedem weiteren
        // Schritt der Zeitintegration benötigt werden und um die KMatrix und das Ergebnis der linearen Fem zum vergleich
        // heranzuziehen
        RETURNING_ASSERT(m_isoMesh.createStiffnessMatrix(), "KMatrix Erstellung fehlgeschlagen",);
        
        // Zeitschrittintegration
        const float simulationTime = 5.0f;
        const float deltaTime = 1.0f;
        const size_t simulationSteps = (size_t)(simulationTime/deltaTime);

        // refs
        const IsoMeshMaterial& mat = m_isoMesh.getMaterial();
        const auto& r_pref = m_isoMesh.getCells().begin()->second.getPrefab();

        //
        std::unordered_map<std::string, SymEngine::DenseMatrix> symbolTable = {};
        symbolTable["ElastTensor"] = m_isoMesh.SymCMatrix;

        // epsilon_v mit starwert zeros (mit Nullen gefüllt) initialisieren
        symbolTable["epsilon_v"] = SymEngine::DenseMatrix(m_isoMesh.m_cachedBMats.begin()->second.nrows(),1);
        SymEngine::zeros(symbolTable["epsilon_v"]);

        symbolTable["epsilon"] = SymEngine::DenseMatrix(6,1,
            {toExpression("e1"),toExpression("e2"),toExpression("e3"),toExpression("e4"),toExpression("e5"),toExpression("e6")});
        symbolTable["uCell"] = SymEngine::DenseMatrix(r_pref.nNodes * r_pref.nDimensions,1);

        SymEngine::zeros(symbolTable["uCell"]);
        symbolTable["B"] = m_isoMesh.m_cachedBMats.begin()->second;
        
        symbolTable["jDet"] = SymEngine::DenseMatrix(1,1,{NULL_EXPR});
        symbolTable["w"] = SymEngine::DenseMatrix(1,1,{NULL_EXPR});

        // Residuum entspricht K*u und ist dementsprechend ein Vektor
        SymEngine::DenseMatrix resCell(r_pref.nDimensions * r_pref.nNodes,1);
        SymEngine::DenseMatrix resGlobal(r_pref.nDimensions * m_isoMesh.getUndeformedNodes().size(),1);
        SymEngine::zeros(resGlobal);

        // Startwerte festlegen
        // ...

        // durch Integrationsschritte loopen
        for(size_t stepIdx = 0; stepIdx < simulationSteps; stepIdx++){

            // Elementsteifigkeitsmatrix aufstellen und assemblieren
            // . durch Elemente loopen
            // . durch quadraturpunkte loopen und für Matritzen (B und J) einsetzen
            // . innere Variable fortpflanzen
            // . Einsetzen
            // . Assemblierung

            // Zu Residuum umformulieren
            // . R = K*u-f

            // per Newton Rapshon solver lösen
            // . delta U finden für Lösung

            // startwerte anpassen

            for(const auto& [cellIdx, cell] : m_isoMesh.getCells()){

                // Position in caches : cellIdx

                // KMatrix aufstellen nach spannungsansatz

                // K Matrix bzw. Residuumsformulierung mit Spannungsansatz sigma
                // K_cell * u = sum(B^T*sigma*jdet*weight)

                // ...

                // Verschiebungsvektor aufstellen
                for(size_t localNodeNum = 0; localNodeNum < r_pref.nNodes; localNodeNum++){

                    //
                    for(const auto& koord : m_isoMesh.globKoords){

                        //
                        symbolTable["uCell"].set(localNodeNum * r_pref.nDimensions + koord, 0,
                            toExpression("u" + std::to_string((cell[localNodeNum]-m_isoMesh.nodeNumOffset)*r_pref.nDimensions+koord)));
                    }
                }

                // Matrix nullen
                SymEngine::zeros(resCell);

                // Loop durch quadrature Points
                for(size_t quadPoint = 0; quadPoint < r_pref.nNodes; quadPoint++){

                    //
                    subMatrix(m_isoMesh.m_cachedBMats[cellIdx], symbolTable["B"], r_pref.quadraturePoints[quadPoint]);
                    symbolTable["jDet"].set(0,0,m_isoMesh.m_cachedJDets[cellIdx]->subs(r_pref.quadraturePoints[quadPoint]));
                    symbolTable["w"].set(0,0,toExpression(r_pref.weights[quadPoint]));
                
                    //
                    resCell += evalSymbolicMatrixExpr("B^T*ElastTensor*(B*uCell+-epsilon_v)*jDet*w",symbolTable);
                }

                // LOG << cellIdx << endl;

                // Assemblierung in globale K Matrix bzw Residuumsvektor
                for(size_t localNodeNum = 0; localNodeNum < r_pref.nNodes; localNodeNum++){

                    //
                    for(const auto& koord : m_isoMesh.globKoords){

                        //
                        resGlobal.set((cell[localNodeNum] - m_isoMesh.nodeNumOffset) * r_pref.nDimensions + koord, 0, resCell.get(localNodeNum * r_pref.nDimensions + koord,0));
                    }
                }
            }

            // nötig und zeitsparend ???
            expandMatrix(resGlobal);
            sym::roundMatrix(resGlobal);

            // resGlobal - f = 0 mit NR lösen
            // >> u für Zeitschritt bekannt
            // >> Größen fortplanzen, Startwerte anpassen
            // >> datasets und nodesets cachen
            // LOG << resGlobal << endl;

            // im Idealfall ist das Residuum jetzt nur noch von den Einträgen des Verschiebungsvektors u abhängig
            // ...

            // erst Jacoby Matrix erstellen >> Residuum nach Vektor u ableiten
            // >> jede Komponente des Vektors resGlobal nach jeder Komponente von u ableiten
            // >> herauskommt eine Matrix
            // Beginn der Newton Raphson Iteration

            SymEngine::DenseMatrix jacobianMatrix(resGlobal.nrows(), resGlobal.nrows());
            SymEngine::zeros(jacobianMatrix);
            
            // in eigen sparse matrix einsortieren 
            for(size_t resRow = 0; resRow < resGlobal.nrows(); resRow++){

                //
                for(size_t uRow = 0; uRow < resGlobal.nrows(); uRow++){
                    
                    //
                    jacobianMatrix.set(resRow, uRow, resGlobal.get(resRow,0)->diff(SymEngine::symbol("u" + std::to_string(uRow))));
                }
            }

            //
            LOG << jacobianMatrix << endl;

            // hier Iteration und für jeden Schritt in Tangente und Residuum einsetzen
            // ...

            //
            Eigen::MatrixXd displacement(jacobianMatrix.nrows(),1);
            displacement.setZero();

            SymEngine::map_basic_basic subsDisplacement = {};
            // durch displacement loopen und einsetzen

            // while(residuum(u) != 0vec){ // bzw toleranz
            
                // u += -residuum(u)/tangente(u)
            // }

            return;
        }

        //
        LOG << "** verarbeite nicht lineares Material" << endl;
    }
}

void FemModel::importPdf(const std::string& pdfPath){

    

}

void FemModel::display(const MeshData& displayedData, const int& globKoord, bool displayOnDeformedMesh, bool displayOnQuadraturePoints,
    const Vector2& winSize, const Vector2& frameOffset, const Vector2& padding, bool splitScreen, bool splitScreenVertical){

    if(!m_isoMesh.getMaterial().isLinear){
        return;
    }

    static std::vector<Color*> colors = {&undeformedFrame, &deformedFrame, &deformedFramePlusXi, &deformedFrameMinusXi}; 
        
    Vector2 leftCorner = frameOffset - winSize/2, rightCorner = leftCorner + frameOffset; 

    Vector2 frameSize;
    Vector2 frameOffsets[3];

    static std::vector<NodeSet*> nodeSetsForDisplay;

    // Check ob Material überhaupt als zufallsverteilt angenommen werden soll
    // und ob das sampling schon stattgefunden hat
    if(m_isoMesh.getMaterial().hasPdf && !n_upperXi.empty() && !n_lowerXi.empty()){

        nodeSetsForDisplay = {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes(), &n_upperXi, &n_lowerXi};
    } else {

        nodeSetsForDisplay = {&m_isoMesh.getUndeformedNodes(), &m_isoMesh.getDeformedNodes()};
    }

    if(m_isoMesh.getCells().begin()->second.getPrefab().nDimensions == 3){

        // 3D autofit
        Vector3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
        Vector3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        for(const auto& set : nodeSetsForDisplay)
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

    for(const auto& [idx, set] : std::views::enumerate(nodeSetsForDisplay)){

        if(set->size() <= 0){
            return;
        }
        //RETURNING_ASSERT(set->size() > 0, "Angegebenes NodeSet für Rendering an Postion " + std::to_string(idx) + " ist leer",);
    }

    if(splitScreen && m_isoMesh.getUndeformedNodes().begin()->second.getDimension() == 2 && m_isoMesh.getMaterial().hasPdf){

        m_isoMesh.display(m_isoMesh.getCellData(), displayedData, globKoord, nodeSetsForDisplay, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[0], padding/3);
        m_isoMesh.display(data_upperXi, displayedData, globKoord, {&n_upperXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[1], padding/3);
        m_isoMesh.display(data_lowerXi, displayedData, globKoord, {&n_lowerXi}, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, frameSize, frameOffsets[2], padding/3);

    } else {

        m_isoMesh.display(m_isoMesh.getCellData(), displayedData, globKoord, nodeSetsForDisplay, colors, 0, displayOnDeformedMesh, displayOnQuadraturePoints, winSize, frameOffset, padding);
    }
}

void FemModel::sampling(){

    // Monte Carlo für pdf
    const IsoMeshMaterial& mat = m_isoMesh.getMaterial();

    //
    if(!mat.hasPdf){

        n_upperXi.clear();
        n_lowerXi.clear();

        return;
    }

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