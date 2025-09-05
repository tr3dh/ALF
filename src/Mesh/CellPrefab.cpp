#include "CellPrefab.h"

void from_json(const nlohmann::json& j, Vector2& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
}

CellPrefab CellPrefab::nullRef = CellPrefab();

CellPrefab::CellPrefab() = default;

CellPrefab::CellPrefab(const std::string& path) : CellPrefab(){

    // Check ob Pfad existiert
    CRITICAL_ASSERT(fs::exists(path), "Angegebener Pfad : '" + path + "' existiert nicht");

    // Check ob fstream den file ohne Fehler geladen bekommt
    std::ifstream isoParamFile(path);
    CRITICAL_ASSERT(isoParamFile, "Fehler beim Laden von '" + path + "'");

    // Check ob nlohmann json den file geparst bekommt
    nlohmann::json isoParamData = nlohmann::json::parse(isoParamFile, nullptr, true, true);

    //
    if(!isoParamData.contains("NDIMENSIONS") || !isoParamData.contains("NNODES")){

        ASSERT(TRIGGER_ASSERT, "Angabe über Dimension oder NodeAnzahl in file " + path + " unzulässig oder nicht vorhanden");
        return;
    }

    nDimensions = isoParamData["NDIMENSIONS"].get<uint8_t>();
    nNodes = isoParamData["NNODES"].get<uint8_t>();

    if(nDimensions < 1 || nNodes < nDimensions){

        ASSERT(TRIGGER_ASSERT, "Ungültige Angabe für Dimension und/oder Nodeanzahl");
        return;
    }

    ASSERT(isoParamData.contains("SHAPEFUNCTIONS") && isoParamData["SHAPEFUNCTIONS"].get<std::vector<std::string>>().size() == nNodes,
        "Ansatzfunktionen fehlen oder sind nur unvollständig vorhanden");

    //
    shapeFunctions.reserve(nNodes);
    shapeFunctionDerivatives.reserve(nNodes);

    const auto shapefVec = isoParamData["SHAPEFUNCTIONS"].get<std::vector<std::string>>();

    for(size_t nodeNum = 0; nodeNum < shapefVec.size(); nodeNum++){

        // Funktion einfügen die checkt ob die Expression nur bekannte symbole enthält (-> x,y,z, ..., r,s,t, ...)

        const auto& shapeFunction = shapefVec[nodeNum]; 

        shapeFunctions.emplace_back(SymEngine::parse(shapeFunction));
        shapeFunctionDerivatives.emplace_back(1,nDimensions);

        for(int koord = 0; koord < nDimensions; koord++){

            shapeFunctionDerivatives[nodeNum].set(0, koord, shapeFunctions[nodeNum]->diff(g_isometricKoords[koord]));
        }
    }

    ASSERT(isoParamData.contains("QUADRATUREPOINTS") && isoParamData["QUADRATUREPOINTS"].size() == nNodes,
        "Quadraturpunkte fehlen oder sind nur unvollständig vorhanden");

    quadraturePoints.reserve(nNodes);
    for (const auto& subsMap : isoParamData["QUADRATUREPOINTS"]) {

        SymEngine::map_basic_basic exprMap;

        ASSERT(subsMap.size() == nDimensions, "ungültige Anzahl an übergebenen Koordinaten für QuadraturPunkt");
        for (const auto& subsPair : subsMap) {

            for (const auto& [symbol, substitution] : subsPair.items()) {

                exprMap[SymEngine::parse(symbol)] = SymEngine::parse(substitution.get<std::string>());
            }
        }
        quadraturePoints.emplace_back(exprMap);
    }

    ASSERT(isoParamData.contains("WEIGHTS") && isoParamData["WEIGHTS"].get<std::vector<float>>().size() == nNodes,
            "Geiwchte fehlen oder sind nur unvollständig vorhanden");

    weights.reserve(nNodes);
    weights = isoParamData["WEIGHTS"].get<std::vector<float>>();

    //
    label = *string::split(string::split(path,'/').back(), '.').begin();

    // Einlesen der Daten wie wireFrame und faceIndices fürs 3d Rendering
    if(nDimensions != 3){
        return;
    }

    //
    RETURNING_ASSERT(isoParamData.contains("FACEINDICES"), "Rendering Information FACEINDICES für 3D Element " + label + " fehlt, Rendering des Elements nicht möglich",);

    faceIndices = isoParamData["FACEINDICES"].get<std::vector<unsigned short>>();
    numFaces = faceIndices.size()/3;
    RETURNING_ASSERT(faceIndices.size()%3 == 0, "Ungültige Anzahl an FACEINDICES für 3D Element " + label + " übergeben, für eindeutige Zuordnung muss jede fläche durch 3 VertexIndices definiert sein",);

    LOG << LOG_GREEN << "** " << "Element " << label << " mit " << numFaces << " faces geladen" << ENDL;

    ASSERT(isoParamData.contains("WIREFRAMEINDICES"),
        "Rendering Information WIREFRAMEINDICES für 3D Element " + label + " fehlt, Rendering des Elements wird mit default Wireframe, basierend auf angegebenen Faces gerendert");

    if(isoParamData.contains("WIREFRAMEINDICES")){
        wireFrameIndices = isoParamData["WIREFRAMEINDICES"].get<decltype(wireFrameIndices)>();
    }

    LOG << LOG_GREEN << "** " << "Element " << label << " erfolgreich aus file " << path << " geladen" << ENDL;
}