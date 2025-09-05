// Header sorgt dafür das bestimmte template Instanzierungen unterdrückt werden,
// das sie bereits in anderen cpp bzw. obj files liegen

#pragma once

#include "defines.h"

#include "femModel/Model.h"
#include "GUI/ImGuiStyleDecls.h"
#include "GUI/ImGuiCustomElements.h"
#include "Rendering/CameraMovement.h"
#include "Rendering/CellRenderer.h"
#include "Rendering/RenderingStandardShapes.h"
#include "WinHandle/GetEnv.h"
#include "WinHandle/WinCmd.h"
#include "Prompts/Curl.h"
#include "Logging/Logging.h"

#include "CleanUp/ClearCaches.h"

#include "bin/EigenPrecompiles.h"
#include "bin/StringProcessingPreCompiles.h"
#include "bin/SymbolPrecompiles.h"
#include "bin/NodePrecompiles.h"
#include "bin/UIPrecompiles.h"