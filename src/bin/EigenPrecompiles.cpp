#include "defines.h"

template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<int>&);
template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<int>&);
template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<int>&);

template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<uint8_t>&);
template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint8_t>&);
template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint8_t>&);

template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<uint16_t>&);
template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint16_t>&);
template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint16_t>&);

template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<uint32_t>&);
template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint32_t>&);
template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint32_t>&);

template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<size_t>&);
template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<size_t>&);
template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<size_t>&);

// addRow
template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const int&);
template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const uint8_t&);
template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const uint16_t&);
template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const uint32_t&);
template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const size_t&);

// removeRow
template void removeRow(Eigen::MatrixXf&, const int&);
template void removeRow(Eigen::MatrixXf&, const uint8_t&);
template void removeRow(Eigen::MatrixXf&, const uint16_t&);
template void removeRow(Eigen::MatrixXf&, const uint32_t&);
template void removeRow(Eigen::MatrixXf&, const size_t&);

// removeDenseRows
template void removeDenseRows(Eigen::MatrixXf&, const std::vector<int>&);
template void removeDenseRows(Eigen::MatrixXf&, const std::vector<uint8_t>&);
template void removeDenseRows(Eigen::MatrixXf&, const std::vector<uint16_t>&);
template void removeDenseRows(Eigen::MatrixXf&, const std::vector<uint32_t>&);
template void removeDenseRows(Eigen::MatrixXf&, const std::vector<size_t>&);

// addDenseRows
template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<int>&);
template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<uint8_t>&);
template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<uint16_t>&);
template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<uint32_t>&);
template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<size_t>&);