#include "__EigenMatrix.h"

// Entfernt Zeile rowToRemove aus MatrixXf mat
void removeRow(Eigen::MatrixXf& mat, int rowToRemove) {

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
void addRow(Eigen::MatrixXf& mat, const Eigen::RowVectorXf& row, int insertAt) {

    int numRows = mat.rows();
    int numCols = mat.cols();

    mat.conservativeResize(numRows + 1, numCols);

    // Insert the new row
    Eigen::MatrixXf block = mat.block(insertAt, 0, numRows - insertAt, numCols);
    mat.block(insertAt + 1, 0, numRows - insertAt, numCols) = block;

    mat.row(insertAt) = row;
}