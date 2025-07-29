#pragma once

#include "defines.h"

// schreibt alle Ausdrücke aus einem Vektor mit den zugehörigen Werten in die SubstitutionsMap
void assembleSubstitutionMap(SymEngine::map_basic_basic& substitutionMap, const SymEngine::DenseMatrix& symbolVector, Eigen::MatrixXf& result);

// ermittelt für einen gegebenen Residuumsvektor den Vektor bei dessen symbolischer Substitution mit symbolVector
// die Norm des Residuumsvektors unter die Toleranz fällt, der Resultvektor dient dabei als Container während der Rechnung
// sein Initialwert ist der Startwert ner Newton Raphson Iteration  
void solveNewtonRaphson(const SymEngine::DenseMatrix& residual, const SymEngine::DenseMatrix& symbolVector,
                        Eigen::MatrixXf& result, const float& tolerance = 0.01, unsigned int breakAfterIterations = 10);