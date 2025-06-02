#pragma once

#include "symbolicExpressionLexer.h"

// Die gesamte Lexer/Parser Struktur dieses Projekts ist relevant für die nichtlinearen Materialmodelle
// da hier die Steifigkeitsmatritzen/Residuuen von dem Verschiebungsvektor u_n+1 (Euler implizit) abhängig bleiben
// der unbekannt ist und somit als symbolisch behandelt wird
// Die für die symbolischen Ausdrücke bisher verwendete Bibliothek SymEngine bietet keine
// Sybstitutionsfunktionalität für Tensoren an und stellt Expressions beim Einlesen/Parsen nach den Gesetzen der Skalararithmetik um
// Diese ist für Tensoren jedoch nicht gültig -> A*B ungleich B*A
// Deshalb ist eine Behandlung des Ausdrucks erforderlich, die den Ausdruck nicht nach der Skalararithmetik umstellt und
// Matrizen substituieren kann. Um diese Funktionen zu implementieren ist die Zwischenspeicherung des Ausdrucks als String erforderlich>
// und die Substitution über eine eigene Lexer/Parser Logik die den Ausdruck über einen primären Operator und Operationsargumente
// rekursiv in einen Hierarchiebaum zerlegen kann, über den dann reskursiv und über das Einsetzen in alle Const/Symbol Enden/Argumente
// der Ausdruck vollständig gelöst werden kann
//
// Der symbolische Parser nutzt wie bereits im Lexer Beschrieben den Lexer rekursiv, indem er über den Lexer
// die primäre stattfindende Rechenoperation im Ausdruck und deren Argumente herausstellt
// z.B lexer(a*4+3) : Operation "+" ; Argumente [a*4,3]
//
// dann wird die Operation mit den Rückgabewerten der Lexer Aufrufe der Argumente durchgeführt
// also für dieses Beispiel
//     return lexer(a*4) + lexer(3)
//
// diese rekursiven Lexer aufrufe passieren so lange bis kein übergeordneter Operator mehr gefunden werden kann und
// der Wert der Konstante oder des Symbols zurückgegeben werden kann
// also hier
//     lexer(3): Operartion ""; Argumente[]
//     -> gibt zu symbolTable(Ausdruck) oder zu konvertierten Ausdruck zurück,
//        je nachdem ob symbolTable Ausdruck als key enthält
//
// so kann der Ausdruck über den rekursiven Lexer aufruf vollständig substituiert und gelöst werden
//
// Dieser Parser benutzt dazu symengine Matritzen mit symengine Expressions als Matrixeinträge
// Damit kann auch mit Matritzen gerechnet werden deren Einträge symbolische Ausdrücke sind
// Skalare werden als 1x1 Matrix übergeben

// Operatoren für SymEngine DenseMatritzen
void operator *= (SymEngine::DenseMatrix& source, const SymEngine::DenseMatrix& multiplier);
void operator += (SymEngine::DenseMatrix& source, const SymEngine::DenseMatrix& sumup);

// parser für Matrix Substitution von SymEngine Expressions
// !! nur eingeschränkt Nutzbar da SymEngine Ausdrücke mittels Skalararithmetik umstellt
SymEngine::DenseMatrix evalExpression(const Expression& expr, const std::unordered_map<std::string, SymEngine::DenseMatrix>& symbolTable);

SymEngine::DenseMatrix evalSymbolicMatrixExpr(std::string expr, const std::unordered_map<std::string, SymEngine::DenseMatrix>& symbolTable);