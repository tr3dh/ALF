#include "__SymbolicExpressions.h"

//
bool contains(const Expression& expr, const Expression& sym){

    return SymEngine::free_symbols(*expr).contains(sym);
}

const std::vector<Expression> dependencies(const Expression& expr){

    static std::vector<Expression> deps;
    deps.clear();

    auto set = SymEngine::free_symbols(*expr);
    deps.reserve(set.size());

    for(const auto& dep : set){
        deps.push_back(dep);
    }

    return deps;
}