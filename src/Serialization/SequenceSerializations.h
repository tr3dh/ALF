#pragma once

#include "ByteSequence.h"
#include "Drivers/Calculations/__SymbolicExpressions.h"
#include "Drivers/Calculations/__MatrixConversions.h"

#ifndef LOG 
#define LOG std::cout
#endif

// so und in precompiles dann ohne extern
// extern template void removeRow(Eigen::MatrixXf&, const int&);
inline void toByteSequence(const Eigen::MatrixXf& member, ByteSequence& seq) {

    for(size_t row = 0; row < member.rows(); row++){
        for(size_t col = 0; col < member.cols(); col++){
            seq.insert(member(row,col));
        }
    }
    seq.insertMultiple(member.cols(), member.rows());
}

inline void fromByteSequence(Eigen::MatrixXf& member, ByteSequence& seq) {

    size_t rows, cols;
    seq.extractMultiple(rows, cols);

    if(member.rows() != rows || member.cols() != cols){
        member.resize(rows,cols);
    }

    for(size_t row = 0; row < member.rows(); row++){
        for(size_t col = 0; col < member.cols(); col++){
            seq.extract(member(rows-row-1,cols-col-1));
        }
    }
}

template<>
inline void ByteSequence::insert<Eigen::SparseMatrix<float>>(const Eigen::SparseMatrix<float>& member) {

    // einfacher insert ohne triplet ??
    for (int k = 0; k < member.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(member, k); it; ++it) {
            insert(Eigen::Triplet<float>(it.row(), it.col(), it.value()));
        }
    }

    insertMultiple(member.nonZeros(), member.cols(), member.rows());
}

template<>
inline void ByteSequence::extract<Eigen::SparseMatrix<float>>(Eigen::SparseMatrix<float>& member) {

    size_t rows, cols;
    extractMultiple(rows, cols);

    if(member.rows() != rows || member.cols() != cols){
        member.resize(rows,cols);
    }

    std::vector<Eigen::Triplet<float>> triplets;
    extract(triplets);

    member.setFromTriplets(triplets.begin(), triplets.end());
}

template<>
inline void ByteSequence::insert<SymEngine::DenseMatrix>(const SymEngine::DenseMatrix& member) {

    //
    for(size_t row = 0; row < member.nrows(); row++){
        for(size_t col = 0; col < member.ncols(); col++){
            insert(member.get(row,col)->__str__());
        }
    }

    insertMultiple((size_t)member.ncols(), (size_t)member.nrows());
}

template<>
inline void ByteSequence::extract<SymEngine::DenseMatrix>(SymEngine::DenseMatrix& member) {

    size_t rows, cols;
    extractMultiple(rows, cols);

    if(member.nrows() != rows || member.ncols() != cols){
        member.resize(rows,cols);
    }

    for(size_t row = 0; row < member.nrows(); row++){
        for(size_t col = 0; col < member.ncols(); col++){
            member.set(rows-row-1,cols-col-1, get<Expression>());
        }
    }
}

// so und in precompiles dann ohne extern
// extern template void removeRow(Eigen::MatrixXf&, const int&);
inline void toByteSequence(const Expression& member, ByteSequence& seq) {

    seq.insert(member->__str__());
}

inline void fromByteSequence(Expression& member, ByteSequence& seq) {

    member = toExpression(seq.get<std::string>());
}