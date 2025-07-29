#include "NewtonRaphsonSolver.h"

//
void assembleSubstitutionMap(SymEngine::map_basic_basic& substitutionMap, const SymEngine::DenseMatrix& symbolVector, Eigen::MatrixXf& result){

    ASSERT(symbolVector.nrows() == result.rows(), "Symbol Vector und Werte Vektor für substitution nicht gleich groß");

    substitutionMap.clear();
    for(size_t rowInSymbolVector = 0; rowInSymbolVector < symbolVector.nrows(); rowInSymbolVector++){

        substitutionMap.try_emplace(symbolVector.get(rowInSymbolVector, 0), toExpression(result(rowInSymbolVector,0)));
    }
}

//
void solveNewtonRaphson(const SymEngine::DenseMatrix& residual, const SymEngine::DenseMatrix& symbolVector,
    Eigen::MatrixXf& result, const float& tolerance, unsigned int breakAfterIterations){

    // jacobian Matrix erstellen als triplet liste
    // äquivalent zu einer sparse Matrix mit SymEngine Expressions als Einträgen
    static std::vector<SymTriplet> jacobianMatrix = {};
    jacobianMatrix.clear();
    
    // durchlaufe jede Reihe des Residuums
    for(size_t rowInResidual = 0; rowInResidual < residual.nrows(); rowInResidual++){

        // durchlaufe jeden Symbolischen Eintrag
        for(size_t rowInSymbolVector = 0; rowInSymbolVector < symbolVector.nrows(); rowInSymbolVector++){
            
            // ableitung jese Residuums Eintrags nach jedem Symbol
            const auto& expr = residual.get(rowInResidual,0)->diff(
                SymEngine::rcp_dynamic_cast<const SymEngine::Symbol>(symbolVector.get(rowInSymbolVector,0)));

            //
            if(SymEngine::eq(*expr, *SymEngine::zero)){
                continue;
            }

            // Eintrag der Ableitung in die Jacoby Matrix
            jacobianMatrix.emplace_back(rowInResidual, rowInSymbolVector, expr);
        }
    }

    // SubstitionsContainer für Residuum und JacobyMatrix
    Eigen::MatrixXf substitutedResidual(residual.nrows(), residual.ncols());
    substitutedResidual.setZero();

    Eigen::SparseMatrix<float> substitutedJacobyMatrix(residual.nrows(), symbolVector.nrows());
    substitutedJacobyMatrix.setZero();

    //
    static SymEngine::map_basic_basic substitutionMap = {};
    assembleSubstitutionMap(substitutionMap, symbolVector, result);
    
    //
    subMatrix(residual, substitutedResidual, substitutionMap);
    subTriplets(jacobianMatrix, substitutedJacobyMatrix, substitutionMap);

    //
    float firstResidualNorm = substitutedResidual.norm();

    //
    Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
    solver.analyzePattern(substitutedJacobyMatrix);
    
    //
    int numIterations = 0;

    while(std::abs(substitutedResidual.norm()) > tolerance){

        //
        solver.factorize(substitutedJacobyMatrix);

        //
        result -= solver.solve(substitutedResidual);

        //
        assembleSubstitutionMap(substitutionMap, symbolVector, result);

        //
        subMatrix(residual, substitutedResidual, substitutionMap);
        subTriplets(jacobianMatrix, substitutedJacobyMatrix, substitutionMap);

        //
        numIterations++;

        //
        if(numIterations >= breakAfterIterations){
            
            break;
        }
    }

    //
    LOG << numIterations << " NR Iterationen, Norm " << 
            firstResidualNorm << " --> " << substitutedResidual.norm();

    //
    if(numIterations == 0){

        LOG << LOG_RED << "\n!! Newton Raphson Verfahren nicht durchgeführt - Residuumsnorm erfüllt bereits die Torleranz" << endl;
    }
    else if(numIterations >= breakAfterIterations){

        LOG << LOG_RED << "\n!! Newton Raphson Verfahren konvergiert nach maximaler Anzahl zulässiger Durchläufe von " <<
            breakAfterIterations << " Iterationen nicht (für Toleranz von " << tolerance << ")" << endl;
    }


}