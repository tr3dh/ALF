#pragma once

#include "defines.h"

extern template class SymEngine::RCP<const SymEngine::Symbol>;
extern template class SymEngine::RCP<const SymEngine::Basic>;

extern template Expression toExpression<int>(const int&);
extern template Expression toExpression<float>(const float&);
extern template Expression toExpression<double>(const double&);

extern template Expression operator+<int>(const int&, const Expression&);
extern template Expression operator+<float>(const float&, const Expression&);
extern template Expression operator+<double>(const double&, const Expression&);

extern template Expression operator+<int>(const Expression&, const int&);
extern template Expression operator+<float>(const Expression&, const float&);
extern template Expression operator+<double>(const Expression&, const double&);

extern template Expression operator-<int>(const int&, const Expression&);
extern template Expression operator-<float>(const float&, const Expression&);
extern template Expression operator-<double>(const double&, const Expression&);

extern template Expression operator-<int>(const Expression&, const int&);
extern template Expression operator-<float>(const Expression&, const float&);
extern template Expression operator-<double>(const Expression&, const double&);

extern template Expression operator*<int>(const int&, const Expression&);
extern template Expression operator*<float>(const float&, const Expression&);
extern template Expression operator*<double>(const double&, const Expression&);

extern template Expression operator*<int>(const Expression&, const int&);
extern template Expression operator*<float>(const Expression&, const float&);
extern template Expression operator*<double>(const Expression&, const double&);

extern template Expression operator/<int>(const int&, const Expression&);
extern template Expression operator/<float>(const float&, const Expression&);
extern template Expression operator/<double>(const double&, const Expression&);

extern template Expression operator/<int>(const Expression&, const int&);
extern template Expression operator/<float>(const Expression&, const float&);
extern template Expression operator/<double>(const Expression&, const double&);

extern template Expression pow<int>(const int&, const Expression&);
extern template Expression pow<float>(const float&, const Expression&);
extern template Expression pow<double>(const double&, const Expression&);

extern template Expression pow<int>(const Expression&, const int&);
extern template Expression pow<float>(const Expression&, const float&);
extern template Expression pow<double>(const Expression&, const double&);