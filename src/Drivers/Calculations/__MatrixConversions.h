#pragma once

#include <eigen3/Eigen/Sparse>

#include <symengine/real_double.h>
#include <symengine/matrix.h>

#include "__SymbolicExpressions.h"

inline SymEngine::DenseMatrix eigenSparseToSymEngineDense(const Eigen::SparseMatrix<float> &eigenSparse) {

    std::size_t rows = static_cast<std::size_t>(eigenSparse.rows());
    std::size_t cols = static_cast<std::size_t>(eigenSparse.cols());

    SymEngine::DenseMatrix result(rows, cols);

    // Initialisiere mit Nullen
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            result.set(i, j, SymEngine::real_double(0));
        }
    }

    // Setze die nicht-Null-Einträge
    for (int k = 0; k < eigenSparse.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(eigenSparse, k); it; ++it) {
            result.set(static_cast<std::size_t>(it.row()),
                       static_cast<std::size_t>(it.col()),

                       toExpression(it.value()));
        }
    }
    return result;
}