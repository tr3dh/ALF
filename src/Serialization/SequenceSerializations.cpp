#include "SequenceSerializations.h"

void toByteSequence(const Eigen::MatrixXf& member, ByteSequence& seq) {

    for(size_t row = 0; row < member.rows(); row++){
        for(size_t col = 0; col < member.cols(); col++){
            seq.insert(member(row,col));
        }
    }
    seq.insertMultiple(member.cols(), member.rows());
}

void fromByteSequence(Eigen::MatrixXf& member, ByteSequence& seq) {

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

void toByteSequence(const Eigen::SparseMatrix<float>& member, ByteSequence& seq) {

    // einfacher insert ohne triplet ??
    for (int k = 0; k < member.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(member, k); it; ++it) {
            seq.insert(Eigen::Triplet<float>(it.row(), it.col(), it.value()));
        }
    }

    seq.insertMultiple(member.nonZeros(), member.cols(), member.rows());
}


void fromByteSequence(Eigen::SparseMatrix<float>& member, ByteSequence& seq) {

    size_t rows, cols;
    seq.extractMultiple(rows, cols);

    if(member.rows() != rows || member.cols() != cols){
        member.resize(rows,cols);
    }

    std::vector<Eigen::Triplet<float>> triplets;
    seq.extract(triplets);

    member.setFromTriplets(triplets.begin(), triplets.end());
}

void toByteSequence(const SymEngine::DenseMatrix& member, ByteSequence& seq) {

    //
    for(size_t row = 0; row < member.nrows(); row++){
        for(size_t col = 0; col < member.ncols(); col++){
            seq.insert(member.get(row,col)->__str__());
        }
    }

    seq.insertMultiple((size_t)member.ncols(), (size_t)member.nrows());

}

void fromByteSequence(SymEngine::DenseMatrix& member, ByteSequence& seq) {

    size_t rows, cols;
    seq.extractMultiple(rows, cols);

    if(member.nrows() != rows || member.ncols() != cols){
        member.resize(rows,cols);
    }

    for(size_t row = 0; row < member.nrows(); row++){
        for(size_t col = 0; col < member.ncols(); col++){
            member.set(rows-row-1,cols-col-1, seq.get<Expression>());
        }
    }
}

void toByteSequence(const Expression& member, ByteSequence& seq) {

    seq.insert(member->__str__());
}

void fromByteSequence(Expression& member, ByteSequence& seq) {

    member = toExpression(seq.get<std::string>());
}