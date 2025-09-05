#include "symbolicExpressionLexer.h"

std::string getDominantOperator(const std::string& expr){

    // default Werte
    std::string functionName = "";
    size_t hierarchie = 0;

    // Position des Operators im operator string
    // wird hier zum cachen der Wertigkeit des bisher gefundenen, dominantesten Operators genutzt
    size_t dominantOperator = operators.size(); 
                                                
    // Loop durch args
    for(size_t i = 0; i < expr.size(); i++) {

        // Positionen der eingrenzenden Klammern für Hierarchie 1
        // zum hearausstellen des funktionsnamens/funktionsargumente für die dominante Operation
        // für den Fall das kein Operator gefunden werden kann
        static size_t parenOpen, parenClose;

        // beschriebenes Hierarchiemanegement mit Hearusstellen Funktionsnamen
        if(expr[i] == '('){

            hierarchie++;
            if(hierarchie == 1){
                parenOpen = i;
            }
            continue;

        } else if(expr[i] == ')'){

            if(hierarchie == 1){

                parenClose = i;
                functionName = expr.substr(0, parenOpen);
            }

            hierarchie--;
            continue;
        }

        // Lexer ignoriert Tokens, die nicht auf höchster Hierarchie sind
        if (hierarchie != 0) {
            continue;
        }

        // Bestimmung der Wertigkeit des aktuellen Tokens als operator
        size_t pos = operators.find(expr[i]);

        // wenn token im operator string gefunden wird und höher wertig ist als bislang gefundener
        // dominantester Operator wird token als neuer dominantester Operator gecached
        if (pos != std::string::npos && pos < dominantOperator){

            //
            dominantOperator = pos;
        }

        // wenn dominanter Operator höchste Wertigkeit hat ist dominanter Operation
        // bereits bekannt
        if(dominantOperator == 0){
            break;
        }
    }

    // check ob dominante Operation gefunden werden konnte oder noch auf default steht 
    if(dominantOperator < operators.size()){

        // rückgabe Operator
        return std::string(1,operators[dominantOperator]);
    } else {

        // Rückgabe Funktionsname
        // wenn kein Funktionsname gefunden wurde ist zurückgegebener Wert ""
        return functionName;
    }
}

std::vector<std::string> lexExpression(const std::string& expr, const std::string& primOperator) {

    // bekommt Ausdruck übergeben und Operator nach dem dieser gesplittet werden soll

    // ...
    std::vector<std::string> args;
    size_t hierarchie = 0;              
    size_t lastSplit = 0;               // Cachen der letzten Ausdrucksteilung nach operator 

    // check ob operator Funktionsname ist
    bool operatorIsFunctionSymbol = operators.find(primOperator) == std::string::npos;

    // Loop durch char
    for (size_t i = 0; i < expr.size(); ++i) {

        // ...
        static size_t parenOpen, parenClose;

        // ...
        if(expr[i] == '('){

            hierarchie++;
            if(hierarchie == 1 && operatorIsFunctionSymbol){

                parenOpen = i;
            }
            continue;

        } else if(expr[i] == ')'){

            if(hierarchie == 1 && operatorIsFunctionSymbol){

                parenClose = i;
                for(const auto& split : string::split(expr.substr(parenOpen + 1, parenClose - parenOpen - 1), ',')){
                    args.push_back(split);
                }
            }

            hierarchie--;
            continue;
        }

        // Nur auf Hierarchie 0 splitten und für nicht Functionsoperator
        if (hierarchie == 0 && expr[i] == primOperator[0] && !operatorIsFunctionSymbol) {

            // Token vor dem Operator
            if (i > lastSplit)
                args.push_back(expr.substr(lastSplit, i - lastSplit));

            // cachen letzte Teilung mit i+1 damit Operator selbst in keinem Argument steht
            lastSplit = i + 1;
        }
    }

    // Rest nach dem letzten Operator
    if (lastSplit < expr.size() && !operatorIsFunctionSymbol){
        args.push_back(expr.substr(lastSplit));
    }

    return args;
}

void printLexer(const std::string& expr, int level) {

    //
    LOG << std::string(level, '|') << expr;

    const std::string dominantOperator = getDominantOperator(expr);
    LOG << " operator " << dominantOperator << ENDL;

    auto tokens = lexExpression(expr, dominantOperator);

    if(tokens.size() <= 1 && dominantOperator == ""){

        // hier würde im parser eingesetzt werden
        // if expr in symbolMap return symbolMap(expr)
        // else try return const string::convert<float>(expr)
        //      catch ASSERT(TRIGGER_ASSERT, ...)
        
        return;
    }

    // rekursiver Lexer Aufruf
    for (auto& token : tokens) {

        if (token.front() == '(' && token.back() == ')'){
            token = token.substr(1, token.size() - 2);
        }

        LOG << std::string(level, '|') << token << ENDL;

        printLexer(token, level + 1);  
    }
}