#pragma once

#include "../ReccHandling/__Asserts.h"
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

    RETURNING_ASSERT(mat.cols() == 1, "Funktion nicht für Martrizen mit Spaltenanzahl ungleich 1 ausgelegt",);
    if (rowsToRemove.empty()) return;

    RETURNING_ASSERT(std::is_sorted(rowsToRemove.begin(), rowsToRemove.end(), std::greater<>()), "indices nicht absteigend sortiert",);

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
    
    RETURNING_ASSERT(mat.cols() == 1, "Funktion nicht für Martrizen mit Spaltenanzahl ungleich 1 ausgelegt",);
    if (rowsToAdd.empty()) return;
    
    RETURNING_ASSERT(std::is_sorted(rowsToAdd.begin(), rowsToAdd.end()), "Reihenindices sind nicht aufsteigend sortiert",);
    
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
template<typename T>
inline void removeRow(Eigen::MatrixXf& mat, const T& rowToRemove){

    int numRows = mat.rows();
    int numCols = mat.cols();

    if (rowToRemove < 0 || rowToRemove >= numRows) return; // Ungültiger Index

    if (rowToRemove < numRows - 1) {
        mat.block(rowToRemove, 0, numRows - rowToRemove - 1, numCols) =
            mat.block(rowToRemove + 1, 0, numRows - rowToRemove - 1, numCols);
    }

    mat.conservativeResize(numRows - 1, numCols);
}

// Fügt Zeile row an Position insertAt in MatrixXf mat ein
template<typename T>
inline void addRow(Eigen::MatrixXf& mat, const Eigen::RowVectorXf& row, const T& insertAt) {

    int numRows = mat.rows();
    int numCols = mat.cols();

    mat.conservativeResize(numRows + 1, numCols);

    // Insert the new row
    Eigen::MatrixXf block = mat.block(insertAt, 0, numRows - insertAt, numCols);
    mat.block(insertAt + 1, 0, numRows - insertAt, numCols) = block;

    mat.row(insertAt) = row;
}

//
template<typename T>
inline void removeDenseRows(Eigen::MatrixXf& mat, const std::vector<T>& rowsToRemove){

    if (rowsToRemove.empty()) return;

    RETURNING_ASSERT(std::is_sorted(rowsToRemove.begin(), rowsToRemove.end(), std::greater<>()), "indices nicht absteigend sortiert",);

    for(const auto& idx : rowsToRemove){

        //
        removeRow(mat,idx);
    }
}

//
template<typename T>
inline void addDenseRows(Eigen::MatrixXf& mat, const Eigen::RowVectorXf& row, const std::vector<T>& rowsToAdd){

    if (rowsToAdd.empty()) return;
    
    RETURNING_ASSERT(std::is_sorted(rowsToAdd.begin(), rowsToAdd.end()), "Reihenindices sind nicht aufsteigend sortiert",);

    for(const auto& idx : rowsToAdd){

        //
        addRow(mat,row,idx);
    }
}