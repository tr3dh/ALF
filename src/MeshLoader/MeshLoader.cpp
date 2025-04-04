#include "Meshloader.h"

SYMBOL(x);
SYMBOL(y);
SYMBOL(z);
SYMBOL(r);
SYMBOL(s);
SYMBOL(t);

Expression Quad4Cell::s_shapeFunctions[4] = {
    0.25*(1-r)*(1-s),
    0.25*(1+r)*(1-s),
    0.25*(1+r)*(1+s),
    0.25*(1-r)*(1+s),
};

Expression Quad4Cell::s_shapeFunctionDerivatives[4][2] = {};

//
const double Quad4Cell::gauss_pt = 1.0/sqrt(3.0);

//
const std::vector<SymEngine::map_basic_basic> Quad4Cell::subs = {
    {{r, SymEngine::real_double(-gauss_pt)}, {s, SymEngine::real_double(-gauss_pt)}}, // Point 1
    {{r, SymEngine::real_double(-gauss_pt)}, {s, SymEngine::real_double(gauss_pt)}},  // Point 2
    {{r, SymEngine::real_double(gauss_pt)},  {s, SymEngine::real_double(-gauss_pt)}}, // Point 3
    {{r, SymEngine::real_double(gauss_pt)},  {s, SymEngine::real_double(gauss_pt)}}   // Point 4
};