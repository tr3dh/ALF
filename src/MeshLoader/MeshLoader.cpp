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