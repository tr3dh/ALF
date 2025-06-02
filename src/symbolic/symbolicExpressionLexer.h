#pragma once

#include "defines.h"

// Bei dem folgenden Lexer handelt es sich um einen rekursiv aufrufbaren sehr rudimentaeren, symbolischen Lexer
// der als operatoren + * ^ und Funktionsnamen erkennt
// das - kann als Vorzeichen für Skalare und für Symbole eingesetzt werden
// Die Funktionsnamen und entsprechenden Operationen können frei definiert werden
// dementsprechend können bei fehlender Funktionalität einfach Funktionen genutzt werden
// z.B kreuzprodukt cross(x,y), integral int(a,b,x,term), Summe sum(i,k,term)
// Aufgrund der Matrixfunktionalität ist das / nicht implementiert und auch für skalare kann es nicht verwendet werden
// deshalb müssen einige umformulierungen angestellt werden um ausdrücke mit dem Lexer kompatibel zu machen
// Eine weitere Umformulierung betrifft potenzen
// aufgrund der rudimentaeren Funktionsweise ist der hoch Operator mit ^ anstatt von ** gewählt worden
// das Lexen wird so deutlich einfacher
// folgende Ausdrücke sollten/müssen also vor der Übergabe an den Lexer umformuliert werden
// . 1/x muss geschrieben werden als x^-1
// . x**-T muss geschrieben werden als x^-T
// 
// zudem müssen vor Nutzung alle Leerzeichen entfernt werden
// z.B über die std::erase Funktion expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end());
// -> noch eigene Funktion ins Stringprocessing einführen
//
// Der Lexer kann für die vollständige Erfassung der nötigen Operationen zur substitution rekursiv aufgerufen werden
// er stellt die Operation eines symbolischen ausdrucks heraus die zuerst ausgeführt werden würde und die Argumente dieser Operation
// also ebenfalls symbolische ausdrücke, dabei geht er nach einem simplen Muster vor
//
// Solange länge expression args > 1 und ein dominanter Operator gefunden werden kann (+*^funcName)
// else : Wert Symbol oder Wert Konstate zurückgeben
// 1. findet den dominanten Operator auf dem niedrigsten Hierarchie Level der mit geringster Priorität ausgeführt
//    wird -> übergeordnete Operation, Punkt vor Strich, Potenz vor Punkt, ...
// 2. dann wird der Ausdruck nach dem dominanten Operator auf Hierarchie 0 gesplittet
// 4. die Ergebnisse des rekursiven lexens jedes Teils werden über den dominanten Operator verknüpft
//
// Beim rekursiven Aufruf wird jedes Argument erneut an den Lexer übergeben und das so lange bis das argument nur noch
// eine konstate oder ein symbol ist und keine operation mehr gefunden werden kann
// Das geschieht im symbolischen Parser
// 
// Dementsprechend wird der Ausdruck in immer kleinere Teile aufgespalten die durch ihre höher wertigen Operatoren
// den Termen entsprechen in die vorherigen eingesetzt werden
// wenn dann ein symbol oder eine Constante auftauchen wird die Rekursion gestoppt und die entsprechenden Werte zurückgegeben
// So kann der Parser über den rekursiven Lexeraufruf den symbolischen Ausdruck vollständig über sybstitution auflösen

// Operator Hierarchie in einem string dargestellt
// >> nur Operatoren die aus einem char bestehen möglich
// Die Reihenfolge in der die Operatoren im string angegeben sind
// sind relevant für den Lexer
// die weiter vorne stehenden Symbole werden für die Findung des primären Operators
// priorisiert
// Dementsprechend nach den umgekehrten Rechenregeln für Operatoren aufstellen
// . punkt vor strich -> + steht vor *
// . potenz vor punkt -> * steht vor °
// Dies ist am Beispiel A*4+3 gut erkennbar.
// Dadurch das Multiplikation in Ausdrücken zuerst aufgelöst wird und das vor der Addition ist die übergeordnete Operation die
// Addition und das Ergebnis der Multipliktion ein Argument der Addition
// Es gibt noch eine weitere mögliche Operation und das sind eigene Funktionen
// diese kommen in der Wertigkeit automatisch zuletzt, da sie sich als argument einfügen und ihre Aufrufargumente
// innerhalb ihrer eigenen Klammern aufführen
const static std::string operators = "+*^";

// stellt den übergeordneten Operator auf Hierarchie 0 im Ausdruck heraus
// die Hierarchie bestimmung funktioniert folgender maßen
// . zu Beginn auf Level 0
// . für '(' : Hierarchie++
// . für ')' : Hierarchie--
// alle Operationen die nicht auf Hierarchie 0 stattfinden werden ignoriert
std::string getDominantOperator(const std::string& expr);

// splittet Ausdruck nach einem bekannten Dominanten Operator auf Hierarchie 0
std::vector<std::string> lexExpression(const std::string& expr, const std::string& primOperator);

// printet Ausdrucksstruktur mittels rekursivem Lexeraufruf
void printLexer(const std::string& expr, int level = 0);