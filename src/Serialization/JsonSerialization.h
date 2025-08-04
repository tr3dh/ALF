#pragma once

#include "Mesh/Mesh.h"
#include "UncertaintyCalculation/UncertantyCalculation.h"
#include "symbolic/symbolicExpressionParser.h"
#include "Solver/NewtonRaphsonSolver.h"

#include <nlohmann/json.hpp>

// Template für Eigen::MatrixXf
template <typename JSON>
void to_json(JSON& j, const Eigen::MatrixXf& mat) {
    j["rows"] = mat.rows();
    j["cols"] = mat.cols();

    j["data"] = JSON::array();
    for (int i = 0; i < mat.rows(); ++i) {
        JSON row = JSON::array();
        for (int j_ = 0; j_ < mat.cols(); ++j_) {
            row.push_back(mat(i, j_));
        }
        j["data"].push_back(row);
    }
}

template <typename JSON>
void from_json(const JSON& j, Eigen::MatrixXf& mat) {
    int rows = j.at("rows").template get<int>();
    int cols = j.at("cols").template get<int>();

    mat.resize(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j_ = 0; j_ < cols; ++j_) {
            mat(i, j_) = j.at("data")[i][j_].template get<float>();
        }
    }
}

// Template für Eigen::SparseMatrix<float>
template <typename JSON>
void to_json(JSON& j, const Eigen::SparseMatrix<float>& mat) {
    j["rows"] = mat.rows();
    j["cols"] = mat.cols();
    j["data"] = JSON::array();

    for (int k = 0; k < mat.outerSize(); ++k) {
        for (typename Eigen::SparseMatrix<float>::InnerIterator it(mat, k); it; ++it) {
            j["data"].push_back(JSON::array({it.row(), it.col(), it.value()}));
        }
    }
}

template <typename JSON>
void from_json(const JSON& j, Eigen::SparseMatrix<float>& mat) {
    int rows = j.at("rows").template get<int>();
    int cols = j.at("cols").template get<int>();
    mat.resize(rows, cols);

    std::vector<Eigen::Triplet<float>> triplets;
    for (const auto& entry : j.at("data")) {
        int row = entry[0].template get<int>();
        int col = entry[1].template get<int>();
        float value = entry[2].template get<float>();
        triplets.emplace_back(row, col, value);
    }

    mat.setFromTriplets(triplets.begin(), triplets.end());
}

// Template für Eigen::SparseMatrix<float>
template <typename JSON>
void to_json(JSON& j, const CellData& data) {
    
    to_json(j["displacement"], data.cellDisplacement);
    to_json(j["volume"], data.cellVolume);
    to_json(j["strain"], data.strain);
    to_json(j["stress"], data.stress);
    to_json(j["vanMisesStress"], data.vanMisesStress);
    to_json(j["innerVariable"], data.innerVariable);
}

template<typename JSON>
void to_json(JSON& j, const dynNodeXd<float>& node) {
    
    to_json(j, node.getCoordinates());
}