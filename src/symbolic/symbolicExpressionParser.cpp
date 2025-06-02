#include "symbolicExpressionParser.h"


void operator *= (SymEngine::DenseMatrix& source, const SymEngine::DenseMatrix& multiplier){

    static bool sourceScalar, multiplierScalar;

    // source ist scalar
    sourceScalar = (source.ncols() == 1 && source.nrows() == 1) ? true : false;
    multiplierScalar = (multiplier.ncols() == 1 && multiplier.nrows() == 1) ? true : false;

    // entweder zwei Matritzen oder zwei Skalare
    if(sourceScalar == multiplierScalar){

        //
        source.mul_matrix(multiplier, source);

    } else {

        //
        if(multiplierScalar){
            source.mul_scalar(multiplier.get(0,0), source);
        }
        else{
            Expression scalar = source.get(0,0);
            source = multiplier;
            return source *= SymEngine::DenseMatrix(1,1,{scalar});
        }
    }
}

void operator += (SymEngine::DenseMatrix& source, const SymEngine::DenseMatrix& sumup){

    static bool sourceScalar, sumupScalar;

    ASSERT(source.ncols() == sumup.ncols() && source.nrows() == sumup.nrows(),
        "Spalten und Zeilenanzahlen der zu addierenden Matritzen stimmen nicht überein");

    // source ist scalar
    sourceScalar = (source.ncols() == 1 && source.nrows() == 1) ? true : false;
    sumupScalar = (sumup.ncols() == 1 && sumup.nrows() == 1) ? true : false;

    // entweder zwei Matritzen oder zwei Skalare
    if(sourceScalar == sumupScalar){

        //
        source.add_matrix(sumup, source);

    } else {
        ASSERT(TRIGGER_ASSERT, "Matrix Add für Skalar und Matrix wird versucht");
    }
}

SymEngine::DenseMatrix evalExpression(const Expression& expr, const std::unordered_map<std::string, SymEngine::DenseMatrix>& symbolTable) {

    ASSERT(TRIGGER_ASSERT,
        "Funktion nur eingeschränkt nutzbar, da Symengine Expressions Matrixarithmetik verletzen, Nutzung von evalSymbolicMatrixExpr anstelle dessen empfohlen")

    // args kriegen
    // operation ist durch operations art und argumente definiert
    // z.b add [A*4,B-C,B]
    // dementsprechend wird result = args[0] + args[1] + args[2] aufgerufen wobei args[i] noch rekursiv evaluiert werden
    // müssen
    SymEngine::vec_basic args = expr->get_args();

    // Operation abfragen
    if (SymEngine::is_a<SymEngine::Add>(*expr)) {

        // init
        SymEngine::DenseMatrix result = evalExpression(args[0], symbolTable);

        //
        for(size_t argIdx = 0; argIdx < args.size(); argIdx++){

            // wurde zu init für result benutzt
            if(argIdx == 0){
                continue;
            }

            result += evalExpression(args[argIdx],symbolTable);
        }

        return result;

    } else if (SymEngine::is_a<SymEngine::Mul>(*expr)) {
        
        // init
        SymEngine::DenseMatrix result = evalExpression(args[0], symbolTable);

        //
        for(size_t argIdx = 0; argIdx < args.size(); argIdx++){

            // wurde zu init für result benutzt
            if(argIdx == 0){
                continue;
            }

            result *= evalExpression(args[argIdx],symbolTable);
        }

        return result;

    } else if (SymEngine::is_a<SymEngine::Pow>(*expr)) {

        SymEngine::DenseMatrix exponent = evalExpression(args[1], symbolTable);
        ASSERT(exponent.ncols() && exponent.nrows(), "Exponent muss skalar sein");

        std::string exponentStr = exponent.get(0,0)->__str__();

        SymEngine::DenseMatrix base = evalExpression(args[0], symbolTable);
        
        if(exponentStr == "T" || exponentStr == "t"){

            SymEngine::DenseMatrix result(base.ncols(), base.nrows());
            base.transpose(result);
            return result;

        } else if(exponentStr == "-T" || exponentStr == "-t"){

            SymEngine::DenseMatrix result(base.ncols(), base.nrows());
            base.inv(result);
            result.transpose(result);
            return result;
        }

        SymEngine::DenseMatrix result = base;
        int exp = string::convert<int>(exponentStr);

        if(exp == 0){

            SymEngine::eye(result);
            return result;

        } else if(exp == -1){
            base.inv(result);
            return result;
        } 
        else if(exp < 0){
            base.inv(result);
            result*=base;
        }
        
        for(size_t numMults = 1; numMults < std::abs(exp); numMults++){
            result *= base;
        }

        return result;

    } else if (SymEngine::is_a<SymEngine::Symbol>(*expr)) {

        if(symbolTable.contains(expr->__str__())){
            return symbolTable.at(expr->__str__());
        } else {
            return SymEngine::DenseMatrix(1,1,{expr});
        }

    } else if (SymEngine::is_a_Number(*expr)) {

        return SymEngine::DenseMatrix(1,1,{expr});

    } else if (SymEngine::is_a<SymEngine::FunctionSymbol>(*expr)) {

        const SymEngine::FunctionSymbol& func = static_cast<const SymEngine::FunctionSymbol&>(*expr);
        std::string name = func.get_name();
        
        ASSERT(TRIGGER_ASSERT, "Matrix eval für Expressions mit Funktionsnamen noch nicht implementiert");

    } else {

        ASSERT(TRIGGER_ASSERT, "Matrix eval für gegebene Operation in " + expr->__str__() + " mit Funktionsnamen noch nicht implementiert");
    }

    return SymEngine::DenseMatrix();
}

SymEngine::DenseMatrix evalSymbolicMatrixExpr(std::string expr, const std::unordered_map<std::string, SymEngine::DenseMatrix>& symbolTable){

    ASSERT(!expr.contains(' '), "an Lexer übergebene Expression enthält Leerzeichen");

    if(expr.size() == 0){

        return SymEngine::DenseMatrix(1,1,{toExpression(0)});
    }

    if(expr.at(0) == '(' && expr.at(expr.size()-1) == ')'){
    
        //
        expr = expr.substr(1,expr.size()-2);
    }

    const std::string dominantOperator = getDominantOperator(expr);
    std::vector<std::string> args = lexExpression(expr, dominantOperator);

    if(dominantOperator == "+"){

        // init
        SymEngine::DenseMatrix result = evalSymbolicMatrixExpr(args[0], symbolTable);

        //
        for(size_t argIdx = 0; argIdx < args.size(); argIdx++){

            // wurde zu init für result benutzt
            if(argIdx == 0){
                continue;
            }

            result += evalSymbolicMatrixExpr(args[argIdx],symbolTable);
        }

        return result;
    }
    else if(dominantOperator == "*"){

        // init
        SymEngine::DenseMatrix result = evalSymbolicMatrixExpr(args[0], symbolTable);

        //
        for(size_t argIdx = 0; argIdx < args.size(); argIdx++){

            // wurde zu init für result benutzt
            if(argIdx == 0){
                continue;
            }

            result *= evalSymbolicMatrixExpr(args[argIdx],symbolTable);
        }

        return result;
    }
    else if(dominantOperator == "^"){

        SymEngine::DenseMatrix exponent = evalSymbolicMatrixExpr(args[1], symbolTable);
        ASSERT(exponent.ncols() && exponent.nrows(), "Exponent muss skalar sein");

        std::string exponentStr = exponent.get(0,0)->__str__();
        SymEngine::DenseMatrix base = evalSymbolicMatrixExpr(args[0], symbolTable);
        
        if(exponentStr == "T" || exponentStr == "t"){

            SymEngine::DenseMatrix result(base.ncols(), base.nrows());
            base.transpose(result);
            return result;
        }
        else if(exponentStr == "-T" || exponentStr == "-t"){

            ASSERT(base.det()->__str__() != "0", "Matrix nicht invertierbar");

            SymEngine::DenseMatrix result(base.ncols(), base.nrows());
            base.inv(result);
            result.transpose(result);
            return result;
        }

        const float fExponent = string::convert<float>(exponentStr);

        SymEngine::DenseMatrix result(base.ncols(), base.nrows());
        SymEngine::eye(result);

        if(fExponent < 0){
            
            ASSERT(base.det()->__str__() != "0", "Matrix nicht invertierbar");
            base.inv(result); 
        }

        for(float i = 0; i < std::abs(fExponent) - 1; i++){
         
            result *= base;            
        }

        return result;
    }
    // Funktionsname oder nicht deklarierter Operator
    else if(dominantOperator != ""){

    }
    else if(symbolTable.contains(expr)) {

        return symbolTable.at(expr);
    }
    else if(expr.at(0) == '-' && symbolTable.contains(expr.substr(1,expr.size()-1))){
        
        //
        SymEngine::DenseMatrix result(symbolTable.at(expr.substr(1,expr.size()-1)));
        result.mul_scalar(toExpression(-1), result);

        return result;
    }
    else {

        return SymEngine::DenseMatrix(1,1, {toExpression(expr)});
    }

    return SymEngine::DenseMatrix(1,1,{toExpression(0)});
}