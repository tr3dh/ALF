#pragma once

#include "ByteSequence.h"
#include "Drivers/Calculations/__SymbolicExpressions.h"
#include "Drivers/Calculations/__MatrixConversions.h"

#ifndef LOG 
#define LOG std::cout
#endif

// Dense Matrix Eigen MatrixXf
void toByteSequence(const Eigen::MatrixXf& member, ByteSequence& seq);
void fromByteSequence(Eigen::MatrixXf& member, ByteSequence& seq);

// Sparse Matrix Eigen Sparse<float>
void toByteSequence(const Eigen::SparseMatrix<float>& member, ByteSequence& seq);
void fromByteSequence(Eigen::SparseMatrix<float>& member, ByteSequence& seq);

// Dense Matrix SymEngine Dense
void toByteSequence(const SymEngine::DenseMatrix& member, ByteSequence& seq);
void fromByteSequence(SymEngine::DenseMatrix& member, ByteSequence& seq);

// Expression (string serialization wrapper)
void toByteSequence(const Expression& member, ByteSequence& seq);
void fromByteSequence(Expression& member, ByteSequence& seq);