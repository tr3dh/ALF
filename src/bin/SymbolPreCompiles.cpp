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
extern template class SymEngine::RCP<const SymEngine::Symbol>;
extern template class SymEngine::RCP<const SymEngine::Basic>;