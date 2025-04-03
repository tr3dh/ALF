#include "__SymEngineMatrix.h"
#include <symengine/matrices/immutable_dense_matrix.h>
#include <symengine/matrix.h>

void clearMatrix(SymEngine::DenseMatrix& matrix){
    for (unsigned i = 0; i < matrix.nrows(); ++i) {
        for (unsigned j = 0; j < matrix.ncols(); ++j) {
            matrix.set(i, j, SymEngine::integer(0));
        }
    }
}