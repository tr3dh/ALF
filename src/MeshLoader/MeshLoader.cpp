#include "Meshloader.h"

SYMBOL(x);
SYMBOL(y);
SYMBOL(z);
SYMBOL(r);
SYMBOL(s);
SYMBOL(t);

Expression Quad4Cell::s_shapeFunctions[4] = {
    0.25*(1+r)*(1+s),
    0.25*(1-r)*(1+s),
    0.25*(1-r)*(1-s),
    0.25*(1+r)*(1-s),
};

Expression Quad4Cell::s_shapeFunctionDerivatives[4][2] = {};

//
const double Quad4Cell::gauss_pt = 1.0/sqrt(3.0);

//
std::vector<SymEngine::map_basic_basic> Quad4Cell::subs = {
    {{r, SymEngine::real_double(gauss_pt)}, {s, SymEngine::real_double(gauss_pt)}},
    {{r, SymEngine::real_double(-gauss_pt)}, {s, SymEngine::real_double(gauss_pt)}},
    {{r, SymEngine::real_double(-gauss_pt)},  {s, SymEngine::real_double(-gauss_pt)}},
    {{r, SymEngine::real_double(gauss_pt)},  {s, SymEngine::real_double(-gauss_pt)}}
};

cMesh::CellData cMesh::CellData::nullRef = cMesh::CellData();