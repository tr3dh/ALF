#pragma once

#include "defines.h"

inline SYMBOL(xi);

enum class StateVariable{

    STRAIN,
    STRESS,
    DISPLACEMENT,
    NONE
};

inline void to_json(nlohmann::json& j, const StateVariable& type) {
    if (auto name = magic_enum::enum_name(type); !name.empty()) {
        j = std::string{name};
    } else {
        j = nullptr; // oder j = "unknown";
    }
}

inline void from_json(const nlohmann::json& j, StateVariable& type) {
    if (auto opt = magic_enum::enum_cast<StateVariable>(j.get<std::string>())) {
        type = *opt;
    } else {
        type = StateVariable::NONE;
    }
}

struct IsoMeshMaterial{

    const static std::string fileSuffix;

    //
    IsoMeshMaterial() = default;

    bool loadFromFile(const std::string& path = NULLSTR);

    void createElasticityTensor(SymEngine::DenseMatrix& target, const size_t& dimension);

    void substitutePdf();

    void save(const std::string& path);

    float E = 0,v = 0,t = 0;

    Expression pdf = NULL_EXPR, pdf_xi = NULL_EXPR;
    float xi_min = 0, xi_max = 0;
    SymEngine::map_basic_basic subs = {};
    float segmentation = 0, tolerance = 0;
    unsigned int nSamples = 0;
    bool isLinear = true;
    bool hasPdf = false;

    //
    std::string stressApproach, innerVariable, evolutionEquation;
    std::map<std::string, SymEngine::DenseMatrix> nonlinearModellParams = {};

    // entspricht dann numerisches Maximum von size_t -1
    size_t innerVariableDimension = -1;
    std::vector<size_t> innerVariableSize = {};
};