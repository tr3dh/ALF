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