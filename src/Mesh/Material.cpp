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

    if(matData.contains("pdf")){
       
        // wenn key nicht in jsondata, bleibt pdf auf NULL_EXPR
        ASSERT(matData["pdf"].contains("function"), "pdf Funktion wurde nicht deklariert");

        pdf = SymEngine::parse(matData["pdf"]["function"].get<std::string>());
        LOG << LOG_GREEN << "   pdf(xi) = " << pdf << endl;

        ASSERT(matData["pdf"].contains("params"), "Parameter für pdf wurden nicht deklariert");
        SymEngine::map_basic_basic subs = {};

        for (const auto& [param, value] : matData["pdf"]["params"].items()) {
            subs.try_emplace(SymEngine::symbol(param), toExpression(value.get<float>()));
            LOG << LOG_GREEN << "   substitution [" << param << "|" << value << "]" << endl; 
        }
        LOG << endl;

        pdf = pdf->subs(subs);
        LOG << LOG_GREEN << "   substituted pdf(xi) = " << pdf << endl;
    }

    try{
        float pdf_xi0 = SymEngine::eval_double(*pdf->subs({{xi, toExpression(0)}}));
        LOG << LOG_GREEN << "   pdf(xi = 0) = " << pdf_xi0 << endl;
    }
    catch(const std::exception& exc){
        ASSERT(TRIGGER_ASSERT, "Die pdf Funktion ist mit den angegebenen Substitutionen immer noch von mehr Symbolen als xi abhängig");
        pdf = NULL_EXPR;
    }

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