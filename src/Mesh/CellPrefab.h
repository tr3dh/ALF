#pragma once

#include "Node.h"

class CellPrefab{

public:

    CellPrefab() = default;
    CellPrefab(const std::string& path) : CellPrefab(){

        // Check ob Pfad existiert
        CRITICAL_ASSERT(fs::exists(path), "Angegebener Pfad : '" + path + "' existiert nicht");

        // Check ob fstream den file ohne Fehler geladen bekommt
        std::ifstream isoParamFile(path);
        CRITICAL_ASSERT(isoParamFile, "Fehler beim Laden von '" + path + "'");

        // Check ob nlohmann json den file geparst bekommt
        nlohmann::json isoParamData = nlohmann::json::parse(isoParamFile, nullptr, true, true);
        CRITICAL_ASSERT(isoParamFile, "Fehler beim Parsen von '" + path + "'");

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
        for(const auto& [nodeNum, shapeFunction] : std::views::enumerate(isoParamData["SHAPEFUNCTIONS"].get<std::vector<std::string>>())){

            // Funktion einfügen die checkt ob die Expression nur bekannte symbole enthält (-> x,y,z, ..., r,s,t, ...)

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

        LOG << GREEN << "** " << "Element " << label << " erfolgreich aus file " << path << " geladen **" << endl;
    }

    std::string label = "__INVALID__";
    uint8_t nDimensions = 0, nNodes = 0;

    std::vector<Expression> shapeFunctions = {};
    std::vector<SymEngine::DenseMatrix> shapeFunctionDerivatives = {};

    std::vector<SymEngine::map_basic_basic> quadraturePoints = {};
    std::vector<float> weights = {};
};