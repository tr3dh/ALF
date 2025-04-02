#pragma once

// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden

// std includes
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <ranges>

namespace fs = std::filesystem;

#include <string>
#include <optional>

// Sym includes
#include <symengine/expression.h>
#include <symengine/symbol.h>

// Numeric includes
#include <eigen3/Eigen/Dense>

//
#if defined(LOG) | defined(_ERROR)
#error "Logging Direktiven können nicht definiert werden, Makros bereits deklariert"
#endif

#define LOG std::cout
#define _ERROR std::cerr

// Drivers
#include "Drivers/__Asserts.h"
#include "Drivers/__StringProcessing.h"

typedef SymEngine::RCP<const SymEngine::Symbol> Symbol;
typedef SymEngine::RCP<const SymEngine::Basic> Expression;

//
inline std::ostream& operator<<(std::ostream& os, const Symbol& symbol) {
    os << symbol->get_name();
    return os;
}

//
inline std::ostream& operator<<(std::ostream& os, const Expression& expression) {
    os << expression->__str__();
    return os;
}