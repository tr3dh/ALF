#include "Material.h"

const std::string IsoMeshMaterial::fileSuffix = ".mat";

bool IsoMeshMaterial::loadFromFile(const std::string& path){

    RETURNING_ASSERT(string::endsWith(path, fileSuffix), "Invalide Dateiendung beim Laden eines Materials aus " + path, false);
    RETURNING_ASSERT(fs::exists(path), path + "existiert nicht", false);

    LOG << LOG_BLUE << "-- Reading Material from file : " << path << endl;

    // Check ob fstream den file ohne Fehler geladen bekommt
    std::ifstream matFile(path);
    RETURNING_ASSERT(matFile, "Fehler beim Laden von '" + path + "'", false);

    // Check ob nlohmann json den file geparst bekommt
    nlohmann::json matData = nlohmann::json::parse(matFile, nullptr, true, true);
    
    RETURNING_ASSERT(matData.contains("E") && matData.contains("v") && matData.contains("t"),
        "Materialparameter E,v,t des Materialfiles " + path + " unvollständig", false);

    E = matData["E"].get<float>();
    v = matData["v"].get<float>();
    t = matData["t"].get<float>();

    LOG << LOG_GREEN << "** Material aus " << path << " geladen" << endl;
    LOG << LOG_GREEN << "   Parameter : [E|" << E << "]; [v|" << v << "]; [t|" << t << "]" << endl; 
    LOG << endl;

    return true;
}

void IsoMeshMaterial::createElasticityTensor(SymEngine::DenseMatrix& target, const size_t& dimension){

    RETURNING_ASSERT(dimension == 2, "Erstellung des Elastizitätstensors bislang nur für 2D implementiert",);

    //    
    Expression _E = toExpression(E);
    Expression _t = toExpression(v);
    Expression _v = toExpression(v);

    target = SymEngine::DenseMatrix(dimension + 1, dimension + 1,
                {ONE_EXPR, _v, NULL_EXPR,
                 _v, ONE_EXPR, NULL_EXPR,
                 NULL_EXPR, NULL_EXPR, (1-_v)/2});

    target.mul_scalar(_E/(1-_v*_v), target);
}