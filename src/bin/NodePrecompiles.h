#pragma once

#include "defines.h"

extern template struct NodeXd<float, 1>;
extern template struct NodeXd<float, 2>;
extern template struct NodeXd<float, 3>;

extern template struct NodeXd<double, 1>;
extern template struct NodeXd<double, 2>;
extern template struct NodeXd<double, 3>;

extern template struct dynNodeXd<double>;
extern template struct dynNodeXd<float>;
extern template struct dynNodeXd<int>;