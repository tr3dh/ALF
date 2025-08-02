#include "../Mesh/Node.h"

template struct NodeXd<float, 1>;
template struct NodeXd<float, 2>;
template struct NodeXd<float, 3>;

template struct NodeXd<double, 1>;
template struct NodeXd<double, 2>;
template struct NodeXd<double, 3>;

template struct dynNodeXd<double>;
template struct dynNodeXd<float>;
template struct dynNodeXd<int>;