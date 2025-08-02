#include "defines.h"

// Die Precompiles nehmen hier die Deklaration von bestimmten template Ausführungen vor
// dadurch werden diese bereits in ein *.o (und damit auch ein *.d file) kompiliert
// dadurch das die precompiles bei Code Bearbeitung des Projekts nicht mehr geändert werden müssen
// bleiben die die entsprechenden *.o/*.d files erhalten und müssen nicht bei jeder Änderung neu kompiliert
// werden.
// Damit werden die template Ausführungen in kompilierter Form (hier Objekt und Dependency Files)
// gespeichert und können bei Änderungen am übrigen Quellcode einfach ohne weiteres für die Erzeugung der 
// Executable herangezogen werden

// Symengine Precompiles
template class SymEngine::RCP<const SymEngine::Symbol>;
template class SymEngine::RCP<const SymEngine::Basic>;

template Expression toExpression<int>(const int&);
template Expression toExpression<float>(const float&);
template Expression toExpression<double>(const double&);

template Expression operator+<int>(const int&, const Expression&);
template Expression operator+<float>(const float&, const Expression&);
template Expression operator+<double>(const double&, const Expression&);

template Expression operator+<int>(const Expression&, const int&);
template Expression operator+<float>(const Expression&, const float&);
template Expression operator+<double>(const Expression&, const double&);

template Expression operator-<int>(const int&, const Expression&);
template Expression operator-<float>(const float&, const Expression&);
template Expression operator-<double>(const double&, const Expression&);

template Expression operator-<int>(const Expression&, const int&);
template Expression operator-<float>(const Expression&, const float&);
template Expression operator-<double>(const Expression&, const double&);

template Expression operator*<int>(const int&, const Expression&);
template Expression operator*<float>(const float&, const Expression&);
template Expression operator*<double>(const double&, const Expression&);

template Expression operator*<int>(const Expression&, const int&);
template Expression operator*<float>(const Expression&, const float&);
template Expression operator*<double>(const Expression&, const double&);

template Expression operator/<int>(const int&, const Expression&);
template Expression operator/<float>(const float&, const Expression&);
template Expression operator/<double>(const double&, const Expression&);

template Expression operator/<int>(const Expression&, const int&);
template Expression operator/<float>(const Expression&, const float&);
template Expression operator/<double>(const Expression&, const double&);

template Expression pow<int>(const int&, const Expression&);
template Expression pow<float>(const float&, const Expression&);
template Expression pow<double>(const double&, const Expression&);

template Expression pow<int>(const Expression&, const int&);
template Expression pow<float>(const Expression&, const float&);
template Expression pow<double>(const Expression&, const double&);