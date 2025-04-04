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

// 3. Zu double konvertieren
// double result;
// if (eval_double(*substituted, result)) {
//     std::cout << "Numerischer Wert: " << result << std::endl;
//     Optional: Explizite Konvertierung zu float
//     float float_result = static_cast<float>(result);
// } else {
//     std::cerr << "Konvertierung fehlgeschlagen" << std::endl;
// }

void subMatrix(const SymEngine::DenseMatrix& matrix, Eigen::MatrixXd& resultMatrix,  const SymEngine::map_basic_basic& subMap, bool addUp){
    
    for (unsigned i = 0; i < matrix.nrows(); ++i) {
        for (unsigned j = 0; j < matrix.ncols(); ++j) {

            if(addUp){
                resultMatrix(i,j) += SymEngine::eval_double(*matrix.get(i, j)->subs(subMap));
            } else {
                resultMatrix(i,j) = SymEngine::eval_double(*matrix.get(i, j)->subs(subMap));
            }
        }
    }
}