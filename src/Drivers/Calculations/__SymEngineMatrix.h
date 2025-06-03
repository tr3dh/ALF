#pragma once

#include "__SymbolicExpressions.h"
#include <symengine/eval_double.h>
#include <eigen3/Eigen/Dense>

void clearMatrix(SymEngine::DenseMatrix& matrix);

bool subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::MatrixXd& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff = 1.0, bool addUp = false);

bool subMatrix(const SymEngine::DenseMatrix& matrix, SymEngine::DenseMatrix& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff = 1.0, bool addUp = false);
    
void expandMatrix(SymEngine::DenseMatrix& matrix);