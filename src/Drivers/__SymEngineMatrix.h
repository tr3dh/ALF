#pragma once

#include "__SymbolicExpressions.h"
#include <symengine/eval_double.h>
#include <eigen3/Eigen/Dense>

void clearMatrix(SymEngine::DenseMatrix& matrix);

void subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::MatrixXd& resultMatrix,  const SymEngine::map_basic_basic& subMap, bool addUp = false);

template<typename T>
inline void removeSparseRowAndCol(Eigen::SparseMatrix<float>& mat, const std::vector<T>& indices) {
    std::vector<Eigen::Triplet<float>> triplets;
    triplets.reserve(mat.nonZeros());

    //
    for (int k = 0; k < mat.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(mat, k); it; ++it) {
            int row = it.row();
            int col = it.col();

            //
            bool keep = true;
            for (const auto& index : indices) {
                if (row == index || col == index) {
                    keep = false;
                    break;
                }
            }

            //
            if (keep) {
                int newRow = row;
                int newCol = col;
                for (const auto& index : indices) {
                    if (index < row) newRow--;
                    if (index < col) newCol--;
                }
                triplets.emplace_back(newRow, newCol, it.value());
            }
        }
    }

    // Neue Größe berechnen
    int newSize = mat.rows() - indices.size();
    mat.resize(newSize, newSize);
    mat.setFromTriplets(triplets.begin(), triplets.end());
}

template<typename T>
inline void removeSparseRow(Eigen::SparseMatrix<float>& mat, const std::vector<T>& rowsToRemove) {
    assert(mat.cols() == 1);
    std::vector<Eigen::Triplet<float>> triplets;
    triplets.reserve(mat.nonZeros());

    for (int k = 0; k < mat.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(mat, k); it; ++it) {
            int row = it.row();
            bool keep = true;
            
            //
            for (const auto& index : rowsToRemove) {
                if (row == index) {
                    keep = false;
                    break;
                }
            }

            //
            if (keep) {
                int newRow = row;
                for (const auto& index : rowsToRemove) {
                    if (index < row) newRow--;              // Indexverschiebung für absteigende Sortierung
                }
                triplets.emplace_back(newRow, 0, it.value());
            }
        }
    }

    int newSize = mat.rows() - rowsToRemove.size();
    mat.resize(newSize, 1);
    mat.setFromTriplets(triplets.begin(), triplets.end());
}