// Header sorgt dafür das bestimmte template Instanzierungen unterdrückt werden,
// das sie bereits in anderen cpp bzw. obj files liegen

#pragma once

#include "defines.h"

#include "femModel/Model.h"
#include "GUI/ImGuiStyleDecls.h"
#include "GUI/ImGuiCustomElements.h"
#include "Rendering/CameraMovement.h"
#include "Rendering/CellRenderer.h"

#include "CleanUp/ClearCaches.h"

#include "bin/EigenPrecompiles.h"
#include "bin/StringProcessingPreCompiles.h"
#include "bin/SymbolPrecompiles.h"
#include "bin/NodePrecompiles.h"
#include "bin/UIPrecompiles.h"