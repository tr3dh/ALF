#pragma once

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>

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

template<typename T>
inline void addSparseRow(Eigen::SparseMatrix<float>& mat, const std::vector<T>& rowsToAdd) {
    
    assert(mat.cols() == 1);
    if (rowsToAdd.empty()) return;
    
    assert(std::is_sorted(rowsToAdd.begin(), rowsToAdd.end()));
    
    std::vector<Eigen::Triplet<float>> triplets;
    triplets.reserve(mat.nonZeros() + rowsToAdd.size());
    
    int shift = 0;
    auto insertIt = rowsToAdd.begin();
    
    for (int k = 0; k < mat.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(mat, k); it; ++it) {
            int originalRow = it.row();
            
            // Advance insert positions that are before current row
            while (insertIt != rowsToAdd.end() && *insertIt <= originalRow + shift) {
                shift++;
                insertIt++;
            }
            
            triplets.emplace_back(originalRow + shift, 0, it.value());
        }
    }
    
    while (insertIt != rowsToAdd.end()) {
        triplets.emplace_back(*insertIt, 0, 0.0f);
        insertIt++;
    }
    
    // Update matrix
    int newSize = mat.rows() + rowsToAdd.size();
    mat.resize(newSize, 1);
    mat.setFromTriplets(triplets.begin(), triplets.end());
}

// Entfernt Zeile rowToRemove aus MatrixXf mat
void removeRow(Eigen::MatrixXf& mat, int rowToRemove);

// Fügt Zeile row an Position insertAt in MatrixXf mat ein
void addRow(Eigen::MatrixXf& mat, const Eigen::RowVectorXf& row, int insertAt);