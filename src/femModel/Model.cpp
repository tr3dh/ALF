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

    //
    DataSet cellDataSet;
    Eigen::MatrixXf displacement;
    // CellSet und undeformed Nodes liegen im isoMesh
    NodeSet deformedNodes;
};

void fromEigenDense(SymEngine::DenseMatrix& target, Eigen::MatrixXf& source){

    RETURNING_ASSERT(target.nrows() == source.rows() && target.ncols() == source.cols(), "Unültige Dimensionen",);

    for (int i = 0; i < target.nrows(); ++i) {
        for (int j = 0; j < target.ncols(); ++j) {
            target.set(i, j, SymEngine::real_double(source(i, j)));
        }
    }
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

        m_isoMesh.calculateStrainAndStress(m_isoMesh.m_cellData, m_isoMesh.m_uSystem);

        // if mat has pdf einfügen
        if(m_isoMesh.getMaterial().hasPdf){
            sampling();
        }

    } else {

        // KMatrix wird hier erstellt um BMatrix und Jacoby Matrix caches zu füllen, da diese Matritzen in jedem weiteren
        // Schritt der Zeitintegration benötigt werden und um die KMatrix und das Ergebnis der linearen Fem zum vergleich
        // heranzuziehen
        RETURNING_ASSERT(m_isoMesh.createStiffnessMatrix(), "KMatrix Erstellung fehlgeschlagen",);
        RETURNING_ASSERT(m_isoMesh.readBoundaryConditions(), "BoundaryConditions konnten nicht gelesen werden",);
        // fixNodes für Jacoby machen

        // Zeitschrittintegration
        const float simulationTime = 5.0f;
        const float deltaTime = 1.0f;
        const size_t simulationSteps = (size_t)(simulationTime/deltaTime);

        // refs
        const IsoMeshMaterial& mat = m_isoMesh.getMaterial();
        const auto& r_pref = m_isoMesh.getCells().begin()->second.getPrefab();

        //
        std::string innerVariablePreviousFrame = mat.innerVariable + "_n";
        std::string innerVariableCurrentFrame = mat.innerVariable + "_n_plus_1";
        
        //
        std::unordered_map<std::string, SymEngine::DenseMatrix> symbolTable = {};

        // Substitutionsparameter die sich über Iteration nicht ändern
        symbolTable["ElastTensor"] = m_isoMesh.SymCMatrix;
        
        symbolTable["I"] = SymEngine::DenseMatrix(m_isoMesh.SymCMatrix);
        SymEngine::eye(symbolTable["I"]);

        symbolTable["t"] = SymEngine::DenseMatrix(1,1,{toExpression((m_isoMesh.nDimensions == 2 ? m_isoMesh.getMaterial().t : 1))});

        // Deviatoren
        if(m_isoMesh.nDimensions == 2) {
            // 2D Deviator-Matrix
            symbolTable["S"] = SymEngine::DenseMatrix(3,3, {
                toExpression("2/3"), toExpression("-1/3"), toExpression("0"),
                toExpression("-1/3"), toExpression("2/3"), toExpression("0"),
                toExpression("0"),   toExpression("0"),   toExpression("1")
            });
        } else if(m_isoMesh.nDimensions == 3) {
            // 3D Deviator-Matrix
            symbolTable["S"] = SymEngine::DenseMatrix(6,6, {
                toExpression("2/3"),  toExpression("-1/3"), toExpression("-1/3"), toExpression("0"), toExpression("0"), toExpression("0"),
                toExpression("-1/3"), toExpression("2/3"),  toExpression("-1/3"), toExpression("0"), toExpression("0"), toExpression("0"),
                toExpression("-1/3"), toExpression("-1/3"), toExpression("2/3"),  toExpression("0"), toExpression("0"), toExpression("0"),
                toExpression("0"),    toExpression("0"),    toExpression("0"),    toExpression("1"), toExpression("0"), toExpression("0"),
                toExpression("0"),    toExpression("0"),    toExpression("0"),    toExpression("0"), toExpression("1"), toExpression("0"),
                toExpression("0"),    toExpression("0"),    toExpression("0"),    toExpression("0"), toExpression("0"), toExpression("1")
            });
        }

        //
        symbolTable["deltaT"] = SymEngine::DenseMatrix(1,1,{toExpression(deltaTime)});

        // params aus dem nicht linearen Materialmodell
        for(const auto& [param, val] : mat.nonlinearModellParams){
            symbolTable[param] = val;
        }

        // Container für sich ändernde Paramter

        // Dehnung
        symbolTable["epsilon_n"] = SymEngine::DenseMatrix(m_isoMesh.m_cachedBMats.begin()->second.nrows(),1);
        symbolTable["epsilon_n_plus_1"] = SymEngine::DenseMatrix(m_isoMesh.m_cachedBMats.begin()->second.nrows(),1);

        // Aufstellen Residuum einzelnes Element
        symbolTable["B"] = m_isoMesh.m_cachedBMats.begin()->second;
        symbolTable["jDet"] = SymEngine::DenseMatrix(1,1);
        symbolTable["w"] = SymEngine::DenseMatrix(1,1);
        symbolTable["uCell"] = SymEngine::DenseMatrix(r_pref.nNodes * r_pref.nDimensions,1);

        // innere Variable
        symbolTable[innerVariablePreviousFrame] = SymEngine::DenseMatrix(mat.innerVariableSize.size() > 0 ? mat.innerVariableSize[0] : 1, mat.innerVariableSize.size() > 1 ? mat.innerVariableSize[1] : 1);
        symbolTable[innerVariableCurrentFrame] = symbolTable[innerVariablePreviousFrame];
        
        // Startwerte
        SymEngine::zeros(symbolTable["epsilon_n"]);
        SymEngine::zeros(symbolTable[innerVariablePreviousFrame]);

        // Container für Werte der inneren Variable
        // Einordnung über vec[cellIdx * r_pref.nNodes + quadPoint]
        std::vector<SymEngine::DenseMatrix> innerVariableContainer;
        innerVariableContainer.reserve(r_pref.nNodes * m_isoMesh.m_Cells.size());
        innerVariableContainer.resize(r_pref.nNodes * m_isoMesh.m_Cells.size());

        //
        SymEngine::DenseMatrix nullVariable = symbolTable[innerVariablePreviousFrame];
        for(auto& var : innerVariableContainer){
            var = nullVariable;
        }

        // Residuum entspricht K*u und ist dementsprechend ein Vektor
        SymEngine::DenseMatrix cellResidual(r_pref.nDimensions * r_pref.nNodes,1);
        SymEngine::DenseMatrix globalResidual(r_pref.nDimensions * m_isoMesh.getUndeformedNodes().size(),1);
        SymEngine::zeros(globalResidual);

        // allgemeine Instanz zum Runden
        sym::RoundingVisitor visitor;

        // Startwerte festlegen
        // ...

        // Variablen Vector festlegen
        // das Residuum im Newton Raphson Verfahren wird nach diesem Vektor abgeleitet 
        SymEngine::DenseMatrix symbolVector(m_isoMesh.m_nodes.size() * m_isoMesh.nDimensions, 1);
        
        // Einsetzen der Symbolnamen in den Vektor
        for(size_t row = 0; row < globalResidual.nrows(); row++){

            symbolVector.set(row, 0, toExpression("u" + std::to_string(row)));
        }

        // Kürzen des symbol Vektors um fixed dofs
        for(const auto& idx : m_isoMesh.m_indicesToRemove){

            removeRow(symbolVector, idx);
        }

        // Ergebnis Container und Arbeitsgröße für den Newton Raphson Solver
        // ist bereits mit m_uSystem

        // speichern der Daten in SimulationFrame struct mit {displacement, defNodes, DataSet, innerVariable}
        // Option : innere Variable in Frame einlagern oder slot im CellData einrichten
        // der displacement vektor dient dann immer als Lösungsvektor und es wird ins frame Dataset geschrieben
        // und auch die Daten daraus für die sustitution entnommen

        // Container der Simulationswerte
        std::vector<SimulationFrame> simulationFrames = {};

        //
        simulationFrames.resize(simulationSteps);

        //
        SymEngine::map_basic_basic substituteDisplacement = {};

        //
        const auto cellNumOffset = m_isoMesh.getCells().begin()->first;

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

            // auf fixed dofs achten
            // . displacement vektor und symbol vektor aufstellen und kürzen (für m_uSystem bereits geschehen)
            // . Residuums erstellung wie gehabt und kürzen
            // . gekürzten Kraftvektor von Residuum abziehen
            // Durch die übergabe des gekürzten Residuums und symbol und displacement Vektors an den NR Solver
            // wird auch nur eine um die fixierten dofs erstelle Jacoby Matrix erstellt und das delta für das
            // reduzierte System berechnet

            //
            bool firstFrame = stepIdx == 0;

            //
            SimulationFrame& r_currentFrame = simulationFrames[stepIdx];
            SimulationFrame& r_previousFrame = firstFrame ? simulationFrames[stepIdx] : simulationFrames[stepIdx-1];
            r_currentFrame.setupDisplacement(m_isoMesh.m_nodes.size() * m_isoMesh.nDimensions - m_isoMesh.m_indicesToRemove.size());

            //
            LOG << "** erstelle Residuum für Iterationsschritt " << stepIdx << endl;

            //
            globalResidual.resize(m_isoMesh.m_nodes.size() * m_isoMesh.nDimensions,1);
            SymEngine::zeros(globalResidual);

            //
            for(const auto& [cellIdx, cell] : m_isoMesh.getCells()){

                // Position in caches : cellIdx

                // KMatrix aufstellen nach spannungsansatz

                // K Matrix bzw. Residuumsformulierung mit Spannungsansatz sigma
                // K_cell * u = sum(B^T*sigma*jdet*weight)

                // ...

                //
                const auto cellStorePositon = cellIdx - cellNumOffset; 

                // Matrix nullen
                SymEngine::zeros(cellResidual);

                // Verschiebungsvektor aufstellen
                for(size_t localNodeNum = 0; localNodeNum < r_pref.nNodes; localNodeNum++){

                    //
                    for(const auto& koord : m_isoMesh.globKoords){

                        const NodeIndex& globalDofIndex = (cell[localNodeNum]-m_isoMesh.nodeNumOffset)*r_pref.nDimensions+koord;
                        const NodeIndex& localDofIndex = localNodeNum * r_pref.nDimensions + koord;

                        // wenn in fixed dofs symbol mit 0 substituieren
                        if(vectorContains(m_isoMesh.m_indicesToRemove, globalDofIndex)){

                            symbolTable["uCell"].set(localDofIndex, 0, NULL_EXPR);
                        }
                        else {
                            
                            symbolTable["uCell"].set(localDofIndex, 0,
                                toExpression("u" + std::to_string(globalDofIndex)));
                        }
                    }
                }
                
                // Loop durch quadrature Points
                for(size_t quadPoint = 0; quadPoint < r_pref.nNodes; quadPoint++){

                    //
                    subMatrix(m_isoMesh.m_cachedBMats[cellIdx], symbolTable["B"], r_pref.quadraturePoints[quadPoint]);
                    symbolTable["jDet"].set(0,0,m_isoMesh.m_cachedJDets[cellIdx]->subs(r_pref.quadraturePoints[quadPoint]));
                    symbolTable["w"].set(0,0,toExpression(r_pref.weights[quadPoint]));
                    
                    if(!firstFrame){
                        fromEigenDense(symbolTable["epsilon_n"], r_previousFrame.cellDataSet.at(cellIdx).strain);
                    }

                    // Dehnung aus aktuellem Schritt mit B*u substutuieren
                    symbolTable["epsilon_n_plus_1"] = evalSymbolicMatrixExpr("B*uCell", symbolTable);

                    // Wert innere Variable aus letztem Schritt in innerVariableContainer
                    symbolTable[innerVariablePreviousFrame] = innerVariableContainer[cellStorePositon * r_pref.nNodes + quadPoint];

                    // beim fortpflanzen mit der Evolutionsgleichung bleibt innere Variable in beim Euler implizit
                    // imnmer noch von u abhängig, um sich die Werte dann später abspeichern zu können muss
                    // die Evolutionsgleichung noch einmal mit bekanntem Verschiebungsvektor u subtituiert werden 

                    symbolTable[innerVariableCurrentFrame] = firstFrame ? symbolTable[innerVariablePreviousFrame] :
                        evalSymbolicMatrixExpr(
                            mat.evolutionEquation,
                                symbolTable);

                    symbolTable["sigma"] = evalSymbolicMatrixExpr(
                        mat.stressApproach, symbolTable);

                    // cachen der von u abhängigen Formel im innerVariable Container
                    innerVariableContainer[cellStorePositon * r_pref.nNodes + quadPoint] = symbolTable[innerVariableCurrentFrame];

                    //
                    cellResidual += evalSymbolicMatrixExpr(
                        "B^T*sigma*jDet*w*t",symbolTable);
                }

                // Assemblierung in globale K Matrix bzw Residuumsvektor
                for(size_t localNodeNum = 0; localNodeNum < r_pref.nNodes; localNodeNum++){

                    //
                    for(const auto& koord : m_isoMesh.globKoords){

                        //
                        globalResidual.set((cell[localNodeNum] - m_isoMesh.nodeNumOffset) * r_pref.nDimensions + koord, 0,
                        globalResidual.get((cell[localNodeNum] - m_isoMesh.nodeNumOffset) * r_pref.nDimensions + koord, 0) +
                        visitor.apply(SymEngine::expand(cellResidual.get(localNodeNum * r_pref.nDimensions + koord,0))));
                    }
                }
            }

            // nötig und zeitsparend ???
            expandMatrix(globalResidual);
            sym::roundMatrix(globalResidual);

            // benötigt wird ein gekürztes Residuum ein gekürzter symbolVector und der Lösungsvektor

            for(const auto& idx : m_isoMesh.m_indicesToRemove){

                removeRow(globalResidual, idx);
            }

            // Kräfte abziehen
            for(size_t row = 0; row < globalResidual.nrows(); row++){

                const auto& load = m_isoMesh.m_fSystem.coeffRef(row,0);

                if(std::abs(load) < 1e-12){
                    continue;
                }

                globalResidual.set(row,0, globalResidual.get(row,0) - toExpression(load)); 
            }

            // globalResidual - f = 0 mit NR lösen
            solveNewtonRaphson(globalResidual, symbolVector, r_currentFrame.displacement, 5);

            // Ermitteln von Spannug/Dehnung etc. für Substitution und cachen
            // der Spannungs/Dehnungswerte und Verschiebungen

            // für finale substitution in Evogl
            // . nach oben in Schleife durch Elemente legen ??
            assembleSubstitutionMap(substituteDisplacement, symbolVector, r_currentFrame.displacement);

            // Für ermitteln Spannung und Dehnung muss zunächst der Spannungsvektor so aufgefüllt werden dass
            // er wieder von der Länge mit allen dofs übereinstimmt
            r_currentFrame.refillDisplacement(m_isoMesh.m_indicesToAdd);

            // Nodes deformieren
            r_currentFrame.deformedNodes = m_isoMesh.m_nodes;
            m_isoMesh.displaceNodes(r_currentFrame.deformedNodes, r_currentFrame.displacement, m_isoMesh.nodeNumOffset);

            // nach dem der spannungs Vektor wieder vollständig ist können nun spannungen und Dehnungen ermittelt werden
            m_isoMesh.calculateStrainAndStress(r_currentFrame.cellDataSet, r_currentFrame.displacement);

            // über einfache SymEngine Substitutionen bzw subsMatrix kann jetzt der Ausdruck für die innere
            // Variable, der mittels der Matrixsubstitution ermittelt wurde mit den Einträgen des Verschiebungsvektors substituiert und so
            // vollständig aufgelöst werden und in den Container geschrieben sodass er für die Berechnungen im nächsten Schritt
            // für die Substitution herangezogen werden kann

            for(auto& var : innerVariableContainer){

                subMatrix(var,var,substituteDisplacement);
            }

            // mitteln und abspeichern der Substitutionsergebnisse für die einzelnen Zellen
            for(const auto& [cellIdx, cell] : m_isoMesh.getCells()){

                //
                const auto cellStorePositon = cellIdx - cellNumOffset; 

                r_currentFrame.cellDataSet.at(cellIdx).innerVariable = Eigen::MatrixXf(innerVariableContainer.begin()->nrows(), 
                                innerVariableContainer.begin()->ncols());
                r_currentFrame.cellDataSet.at(cellIdx).innerVariable.setZero();
                
                // Loop durch quadrature Points
                for(size_t quadPoint = 0; quadPoint < r_pref.nNodes; quadPoint++){

                    // cachen der von u abhängigen Formel im innerVariable Container
                    subMatrix(innerVariableContainer[cellStorePositon * r_pref.nNodes + quadPoint], r_currentFrame.cellDataSet.at(cellIdx).innerVariable,{},
                        1, true);

                    r_currentFrame.cellDataSet.at(cellIdx).innerVariable *= (1/r_currentFrame.cellDataSet.at(cellIdx).cellVolume);
                }
            }

            // >> nächster Iterationsschritt

            LOG << "Iterationsschritt " << stepIdx << endl;
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
    u_upperXi = m_isoMesh.getDisplacement() * (1-upperXi);
    u_lowerXi = m_isoMesh.getDisplacement() * (1-lowerXi);

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