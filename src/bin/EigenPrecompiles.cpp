#include "defines.h"

extern template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<int>&);
extern template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<int>&);
extern template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<int>&);

extern template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<uint8_t>&);
extern template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint8_t>&);
extern template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint8_t>&);

extern template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<uint16_t>&);
extern template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint16_t>&);
extern template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint16_t>&);

extern template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<uint32_t>&);
extern template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint32_t>&);
extern template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<uint32_t>&);

extern template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<size_t>&);
extern template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<size_t>&);
extern template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<size_t>&);

extern template void removeSparseRowAndCol(Eigen::SparseMatrix<float>&, const std::vector<unsigned int>&);
extern template void removeSparseRow(Eigen::SparseMatrix<float>&, const std::vector<unsigned int>&);
extern template void addSparseRow(Eigen::SparseMatrix<float>&, const std::vector<unsigned int>&);

// addRow
extern template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const int&);
extern template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const unsigned int&);
extern template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const uint8_t&);
extern template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const uint16_t&);
extern template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const uint32_t&);
extern template void addRow(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const size_t&);

// removeRow
extern template void removeRow(Eigen::MatrixXf&, const int&);
extern template void removeRow(Eigen::MatrixXf&, const unsigned int&);
extern template void removeRow(Eigen::MatrixXf&, const uint8_t&);
extern template void removeRow(Eigen::MatrixXf&, const uint16_t&);
extern template void removeRow(Eigen::MatrixXf&, const uint32_t&);
extern template void removeRow(Eigen::MatrixXf&, const size_t&);

// removeDenseRows
extern template void removeDenseRows(Eigen::MatrixXf&, const std::vector<int>&);
extern template void removeDenseRows(Eigen::MatrixXf&, const std::vector<unsigned int>&);
extern template void removeDenseRows(Eigen::MatrixXf&, const std::vector<uint8_t>&);
extern template void removeDenseRows(Eigen::MatrixXf&, const std::vector<uint16_t>&);
extern template void removeDenseRows(Eigen::MatrixXf&, const std::vector<uint32_t>&);
extern template void removeDenseRows(Eigen::MatrixXf&, const std::vector<size_t>&);

// addDenseRows
extern template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<int>&);
extern template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<unsigned int>&);
extern template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<uint8_t>&);
extern template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<uint16_t>&);
extern template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<uint32_t>&);
extern template void addDenseRows(Eigen::MatrixXf&, const Eigen::RowVectorXf&, const std::vector<size_t>&);