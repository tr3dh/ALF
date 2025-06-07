#pragma once

#include "__SymbolicExpressions.h"
#include <symengine/eval_double.h>
#include <eigen3/Eigen/Dense>

typedef std::tuple<size_t, size_t, Expression> SymTriplet;

void clearMatrix(SymEngine::DenseMatrix& matrix);

bool subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::MatrixXf& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff = 1.0, bool addUp = false);

bool subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::SparseMatrix<float>& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff = 1.0, bool addUp = false);

bool subMatrix(const SymEngine::DenseMatrix& matrix, SymEngine::DenseMatrix& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff = 1.0, bool addUp = false);

bool subTriplets(const std::vector<SymTriplet>& matrix, Eigen::SparseMatrix<float>& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff = 1.0, bool addUp = false);

void expandMatrix(SymEngine::DenseMatrix& matrix);

void removeRow(SymEngine::DenseMatrix &mat, int rowToRemove);