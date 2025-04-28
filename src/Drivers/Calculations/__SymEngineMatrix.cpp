#include "__SymEngineMatrix.h"
#include <symengine/matrices/immutable_dense_matrix.h>
#include <symengine/matrix.h>
#include "Drivers/ReccHandling/__Asserts.h"

void clearMatrix(SymEngine::DenseMatrix& matrix){
    for (unsigned i = 0; i < matrix.nrows(); ++i) {
        for (unsigned j = 0; j < matrix.ncols(); ++j) {
            matrix.set(i, j, SymEngine::integer(0));
        }
    }
}

bool subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::MatrixXd& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff, bool addUp){
    
    ASSERT(koeff != 0, "Invalider Koeffizient übergeben");

    float value = 0;

    for (unsigned i = 0; i < matrix.nrows(); ++i) {
        for (unsigned j = 0; j < matrix.ncols(); ++j) {

            value = koeff * SymEngine::eval_double(*matrix.get(i, j)->subs(subMap));

            if(std::isnan(value)){

                ASSERT(TRIGGER_ASSERT, "NAN Value bei Substitution aufgetaucht");
                _ERROR << "Substitution : Result " << value << " koeff " << koeff << " expr " << matrix.get(i, j) << endl;

                for(const auto& [symbol, sub] : subMap){
                    _ERROR << "Symbol " << symbol << " Substutution " << sub << endl;
                }
                return false;
            }

            if(addUp){
                resultMatrix(i,j) += value;
            } else {
                resultMatrix(i,j) = value;
            }
        }
    }

    return true;
}

void expandMatrix(SymEngine::DenseMatrix& matrix){
    for (unsigned i = 0; i < matrix.nrows(); ++i) {
        for (unsigned j = 0; j < matrix.ncols(); ++j) {
            matrix.set(i, j, SymEngine::expand(matrix.get(i,j)));
        }
    }
}