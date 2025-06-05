#include "__SymEngineMatrix.h"
#include <symengine/matrices/immutable_dense_matrix.h>
#include <symengine/matrix.h>
#include "Drivers/ReccHandling/__Asserts.h"
#include <eigen3/Eigen/Sparse>

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

bool subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::SparseMatrix<float>& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff, bool addUp){
    
    ASSERT(koeff != 0, "Invalider Koeffizient übergeben");

    static std::vector<Eigen::Triplet<float>> triplets = {};
    triplets.clear();

    if(!addUp){
        resultMatrix.setZero();
    }

    float value = 0;

    for (unsigned i = 0; i < matrix.nrows(); ++i) {
        for (unsigned j = 0; j < matrix.ncols(); ++j) {

            const auto& expr = matrix.get(i, j);

            if(SymEngine::eq(*expr, *SymEngine::zero)){
                continue;
            }

            value = koeff * SymEngine::eval_double(*expr->subs(subMap));

            if(std::abs(value) < 1e-12){
                continue;
            }

            if(std::isnan(value)){

                ASSERT(TRIGGER_ASSERT, "NAN Value bei Substitution aufgetaucht");
                _ERROR << "Substitution : Result " << value << " koeff " << koeff << " expr " << matrix.get(i, j) << endl;

                for(const auto& [symbol, sub] : subMap){
                    _ERROR << "Symbol " << symbol << " Substutution " << sub << endl;
                }
                return false;
            }

            if(addUp){
                resultMatrix.coeffRef(i,j) += value;
            } else {
                triplets.emplace_back(i,j,value);
            }
        }
    }

    if(!addUp){
        resultMatrix.setFromTriplets(triplets.begin(), triplets.end());
    }

    return true;
}

bool subMatrix(const SymEngine::DenseMatrix& matrix, SymEngine::DenseMatrix& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff, bool addUp){
    
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
                resultMatrix.set(i,j, resultMatrix.get(i,j)+toExpression(value));   //(i,j) += value;
            } else {
                resultMatrix.set(i,j, toExpression(value));
            }
        }
    }

    return true;
}

bool subTriplets(const std::vector<SymTriplet>& matrix, Eigen::SparseMatrix<float>& resultMatrix,  const SymEngine::map_basic_basic& subMap, float koeff, bool addUp){

    //
    ASSERT(koeff != 0, "Invalider Koeffizient übergeben");

    static std::vector<Eigen::Triplet<float>> triplets = {};
    triplets.clear();

    if(!addUp){
        resultMatrix.setZero();
    }

    float value = 0;

    for(const auto& [i,j,expr] : matrix){

        if(SymEngine::eq(*expr, *SymEngine::zero)){
            continue;
        }

        value = koeff * SymEngine::eval_double(*expr->subs(subMap));

        if(std::abs(value) < 1e-12){
            continue;
        }

        if(std::isnan(value)){

            ASSERT(TRIGGER_ASSERT, "NAN Value bei Substitution aufgetaucht");
            _ERROR << "Substitution : Result " << value << " koeff " << koeff << " expr " << expr << endl;

            for(const auto& [symbol, sub] : subMap){
                _ERROR << "Symbol " << symbol << " Substutution " << sub << endl;
            }
            return false;
        }

        if(addUp){
            resultMatrix.coeffRef(i,j) += value;
        } else {
            triplets.emplace_back(i,j,value);
        }
    }

    if(!addUp){
        resultMatrix.setFromTriplets(triplets.begin(), triplets.end());
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