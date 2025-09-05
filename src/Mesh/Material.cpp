#include "Material.h"

const std::string IsoMeshMaterial::fileSuffix = ".Material";

SymEngine::DenseMatrix fromJson(const nlohmann::json& j) {

    unsigned rows = j.at("rows");
    unsigned cols = j.at("cols");
    const auto& data = j.at("data");

    std::vector<Expression> elements;

    for (const auto& row : data) {
        for (const auto& entry : row) {
            elements.push_back(toExpression(entry.get<std::string>()));
        }
    }
    return SymEngine::DenseMatrix(rows, cols, elements);
}

bool IsoMeshMaterial::loadFromFile(const std::string& path){

    RETURNING_ASSERT(string::endsWith(path, fileSuffix), "Invalide Dateiendung beim Laden eines Materials aus " + path, false);
    RETURNING_ASSERT(fs::exists(path), path + "existiert nicht", false);

    LOG << LOG_BLUE << "-- Reading Material from file : " << path << ENDL;

    // Check ob fstream den file ohne Fehler geladen bekommt
    std::ifstream matFile(path);
    RETURNING_ASSERT(matFile, "Fehler beim Laden von '" + path + "'", false);

    // Check ob nlohmann json den file geparst bekommt
    nlohmann::json matData; matData = nlohmann::json::parse(matFile, nullptr, true, true);
    
    RETURNING_ASSERT(matData.contains("stdParams"),
        "Materialparameter E,v,t des Materialfiles " + path + " unvollständig", false);

    E = matData["stdParams"]["E"].get<float>();
    v = matData["stdParams"]["v"].get<float>();
    t = matData["stdParams"]["t"].get<float>();

    RETURNING_ASSERT(E != 0 && v != 0 && t != 0, "Relevante Materiaparamter E,t,v sind Null, kein valides Material Modell",false);

    LOG << LOG_GREEN << "** Material aus " << path << " geladen" << ENDL;
    LOG << LOG_GREEN << "   Parameter : [E|" << E << "]; [v|" << v << "]; [t|" << t << "]" << ENDL; 

    if(matData.contains("pdf")){
       
        //
        hasPdf = true;

        // wenn key nicht in jsondata, bleibt pdf auf NULL_EXPR
        ASSERT(matData["pdf"].contains("function"), "pdf Funktion wurde nicht deklariert");

        pdf = SymEngine::parse(matData["pdf"]["function"].get<std::string>());
        LOG << LOG_GREEN << "   pdf(xi) = " << pdf << ENDL;

        if(matData["pdf"].contains("borders")){

            xi_min = matData["pdf"]["borders"]["xi_min"].get<float>();
            xi_max = matData["pdf"]["borders"]["xi_max"].get<float>();

            LOG << LOG_GREEN << "   Xi Borders geladen : [" << xi_min << "|" << xi_max << "]" << ENDL;
        }

        ASSERT(matData["pdf"].contains("pdfPreprocessing"), "pdf preprocessing nicht definiert");

        segmentation = matData["pdf"]["pdfPreprocessing"]["segmentation"].get<float>();
        tolerance = matData["pdf"]["pdfPreprocessing"]["tolerance"].get<float>();

        ASSERT(matData["pdf"].contains("pdfSampling"), "pdf sampling nicht definiert");

        nSamples = matData["pdf"]["pdfSampling"]["samples"].get<unsigned int>();

        ASSERT(matData["pdf"].contains("params"), "Parameter für pdf wurden nicht deklariert");
        
        for (const auto& [param, value] : matData["pdf"]["params"].items()) {
            subs.try_emplace(SymEngine::symbol(param), toExpression(value.get<float>()));
            LOG << LOG_GREEN << "   substitution [" << param << "|" << value << "]" << ENDL; 
        }
        LOG << ENDL; 
    } else {
        hasPdf = false;
    }

    // Ausgelagert aus Laden für bessere Übersichtlichkeit
    if(hasPdf){
        substitutePdf();
    }

    if(matData.contains("isLinear") && !matData["isLinear"].get<bool>()){

        isLinear = false;
        LOG << "** geladenes Material ist nicht linear" << ENDL;

        // Lademechanik für nichtlineare Materialien
        // nichtlineare Materialien müssen definiert sein über
        // . klare Angabe innere Variable z.b           -> epsilon^v, Vektor, größen Angabe nötig ?? für viskoelastische Dehnung
        // . Spannungsansatz                            -> sigma = E * (epsilon - epsilon^v) für viskoelastische Materialien (siehe Markdown)
        // . Evolutionsgleichung für innere Variable    -> epsilon^v _i+1 = f(B,E,epsilon, epsilon^v)  
        // . Übersicht über eingeführte Symbole, Zusammenhänge etc.
        // . genaue beschreibung des Verfahrens (implizit,explizit, ... ??)

        // Kategorie
        ASSERT(matData.contains("nonLinearApproach"), "Ansatz für nicht lineares Material fehlt");

        // Array für Innere Variable z.b "innerVariable": ["epsilon_v",1],
        ASSERT(matData["nonLinearApproach"].contains("innerVariable"), "Angabe innere Variable für nicht lineares Material fehlt");
        innerVariable = matData["nonLinearApproach"]["innerVariable"][0];
        innerVariableDimension = matData["nonLinearApproach"]["innerVariable"][1];

        ASSERT(matData["nonLinearApproach"].contains("innerVariableSize"), "Angabe innere Variable für nicht lineares Material fehlt");

        innerVariableSize.clear();
        innerVariableSize.reserve(innerVariableDimension);

        for(size_t dim = 0; dim < innerVariableDimension; dim++){
            innerVariableSize.emplace_back(matData["nonLinearApproach"]["innerVariableSize"][dim]);
        }

        LOG << "** innere Variable " << innerVariable << " der Dimension " << innerVariableDimension << " geladen" << ENDL;

        // Spannungsansatz
        ASSERT(matData["nonLinearApproach"].contains("sigma"), "Spannungs Ansatz für nicht lineares Material fehlt");
        stressApproach = matData["nonLinearApproach"]["sigma"];

        ASSERT(string::contains(stressApproach, innerVariable), "Ansatz für Spannung ist nicht von angegebener innererVariable abhängig");

        string::fullStrip(stressApproach);
        LOG << "** sigma = " << stressApproach << " = f(";
        
        // args ...

        LOG << ")" << ENDL;

        // Evolution
        ASSERT(matData["nonLinearApproach"].contains("evolutionEquation"), "EvolutionsGleichung für innere Variable fehlt");
        evolutionEquation = matData["nonLinearApproach"]["evolutionEquation"];
        string::fullStrip(evolutionEquation);

        //
        LOG << "** " << innerVariable << "_n_plus_1 = " << evolutionEquation << ENDL;

        if(!matData["nonLinearApproach"].contains("params")){
            return true;
        }

        for (const auto& [param, value] : matData["nonLinearApproach"]["params"].items()) {

            nonlinearModellParams.try_emplace(param, fromJson(value));
            LOG << "** substitution " << param << " : \n" << nonlinearModellParams[param] << ENDL;
        }

        if(matData["nonLinearApproach"].contains("normTolerance")){
            normTolerance = matData["nonLinearApproach"]["normTolerance"].get<decltype(normTolerance)>();
        }

        if(matData["nonLinearApproach"].contains("precissionDigits")){
            precisionDigits = matData["nonLinearApproach"]["precissionDigits"].get<decltype(precisionDigits)>();
        }

        if(matData["nonLinearApproach"].contains("simulationDuration")){
            simulationDuration = matData["nonLinearApproach"]["simulationDuration"].get<decltype(simulationDuration)>();
        }

        if(matData["nonLinearApproach"].contains("simulationTimeStep")){
            deltaTime = matData["nonLinearApproach"]["simulationTimeStep"].get<decltype(deltaTime)>();
        }

        if(matData["nonLinearApproach"].contains("breakNRAfterNumIterations")){
            breakNRAfterNumIterations = matData["nonLinearApproach"]["breakNRAfterNumIterations"].get<decltype(breakNRAfterNumIterations)>();
        }

        // Default für rundung der Expressions setzen
        g_decimalPlaces = precisionDigits;
        LOG << "-- Nachkommastellen für Rundung eingestellt auf " << g_decimalPlaces << ENDL;

        //
        simulationSteps = (size_t)(simulationDuration/deltaTime);
    }

    LOG << ENDL;

    return true;
}

void IsoMeshMaterial::save(const std::string& path){

    RETURNING_ASSERT(string::endsWith(path, fileSuffix), "Invalide Dateiendung beim Speichern eines Materials nach " + path,);

    LOG << LOG_BLUE << "-- Writing Material to file : " << path << ENDL;

    // Check ob nlohmann json den file geparst bekommt
    nlohmann::json matData = nlohmann::json::parse(std::ifstream(path), nullptr, true, true);

    std::ofstream matFile(path);
    RETURNING_ASSERT(matFile, "Fehler beim Öffnen von '" + path + "' zum Schreiben",);

    // Basisparameter speichern
    matData["stdParams"]["E"] = E;
    matData["stdParams"]["v"] = v;
    matData["stdParams"]["t"] = t;

    // PDF-Informationen speichern, falls vorhanden
    if (pdf->__str__() != NULL_EXPR->__str__()) {

        matData["pdf"]["function"] = pdf->__str__();

        if (xi_min != 0.0f || xi_max != 0.0f) {
            matData["pdf"]["borders"]["xi_min"] = xi_min;
            matData["pdf"]["borders"]["xi_max"] = xi_max;
        }

        matData["pdf"]["pdfPreprocessing"]["segmentation"] = segmentation;
        matData["pdf"]["pdfPreprocessing"]["tolerance"] = tolerance;

        matData["pdf"]["pdfSampling"]["samples"] = nSamples;

        // Parameter für PDF (subs)
        nlohmann::json params;
        for (auto& [sym, expr] : subs) {
            std::string paramName = sym->__str__();
            double value = SymEngine::eval_double(*expr);
            params[paramName] = value;
        }
        matData["pdf"]["params"] = params;
    } else {
        matData.erase("pdf");
    }

    matData["isLinear"] = isLinear;

    // JSON in Datei schreiben
    matFile << matData.dump(4);
    matFile.close();

    LOG << LOG_GREEN << "** Material nach " << path << " gespeichert" << ENDL;
}

void IsoMeshMaterial::substitutePdf(){

    sym::round_subs_map(subs,3);
    pdf_xi = sym::roundAllNumbers(pdf->subs(subs),3);

    LOG << LOG_GREEN << "   substituted pdf(xi) = " << pdf_xi << ENDL;

    try{
        float pdf_xi0 = SymEngine::eval_double(*pdf_xi->subs({{xi, toExpression(0)}}));
        LOG << LOG_GREEN << "   pdf(xi = 0) = " << pdf_xi0 << ENDL;
        ASSERT(pdf_xi0 > 0, "pdf Funktion muss an Stelle xi = 0 größer null sein");
    }
    catch(const std::exception& exc){
        ASSERT(TRIGGER_ASSERT, "Die pdf Funktion ist mit den angegebenen Substitutionen immer noch von mehr Symbolen als xi abhängig");
        pdf = NULL_EXPR;
    }
}

void IsoMeshMaterial::createElasticityTensor(SymEngine::DenseMatrix& target, const size_t& dimension){

    //    
    Expression _E = toExpression(E);
    Expression _t = toExpression(v);
    Expression _v = toExpression(v);

    if(dimension == 1){
        target = SymEngine::DenseMatrix(1,1,{_E});
    }
    else if(dimension == 2){

        target = SymEngine::DenseMatrix(3, 3,
                {ONE_EXPR, _v, NULL_EXPR,
                 _v, ONE_EXPR, NULL_EXPR,
                 NULL_EXPR, NULL_EXPR, (1-_v)/2});

        target.mul_scalar(_E/(1-_v*_v), target);
    }
    else if(dimension == 3){

        target = SymEngine::DenseMatrix(6, 6,
            {1-_v, _v, _v, NULL_EXPR, NULL_EXPR, NULL_EXPR,
            _v, 1-_v, _v, NULL_EXPR, NULL_EXPR, NULL_EXPR,
            _v, _v, 1-_v, NULL_EXPR, NULL_EXPR, NULL_EXPR,
            NULL_EXPR, NULL_EXPR, NULL_EXPR, (1-2*_v)/2, NULL_EXPR, NULL_EXPR,
            NULL_EXPR, NULL_EXPR, NULL_EXPR, NULL_EXPR, (1-2*_v)/2, NULL_EXPR,
            NULL_EXPR, NULL_EXPR, NULL_EXPR, NULL_EXPR, NULL_EXPR, (1-2*_v)/2
        });

        target.mul_scalar(_E/((1+_v)*(1-2*_v)), target);
    }
    else {
        RETURNING_ASSERT(TRIGGER_ASSERT, "Invalide übergebene Dimension " + std::to_string(dimension) + " für Steifigkeitsmatrix",);
    }
}