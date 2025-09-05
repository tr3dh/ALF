// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden
// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden

#pragma once

#include "Env.h"

// Drivers
#include "Drivers/ReccHandling/__Asserts.h"
#include "Drivers/ReccHandling/__StringProcessing.h"
#include "Drivers/ReccHandling/__jsonSerialize.h"
#include "Drivers/ReccHandling/__lineCounter.h"
#include "Drivers/ReccHandling/__timeStamp.h"

#include "Drivers/Calculations/__SymbolicExpressions.h"
#include "Drivers/Calculations/__SymEngineMatrix.h"
#include "Drivers/Calculations/__SymbolicExpressionRound.h"
#include "Drivers/Calculations/__EigenMatrix.h"
#include "Drivers/Calculations/__MatrixConversions.h"

#include "Drivers/Raylib/__rlLogging.h"
#include "Drivers/Raylib/__rlProgressDisplay.h"

#include "Drivers/Visualisation/__Coloration.h"

#include "Drivers/UI/__fileBrowser.h"

#include "Drivers/Vec/__Vec.h"

// Decorators
#include "decorators/timeFunction.h"

//
#include "Serialization/ByteSequence.h"
#include "Serialization/SequenceSerializations.h"

// typedefs
typedef uint16_t NodeIndex;
typedef NodeIndex CellIndex;

// globals
inline SYMBOL(x);
inline SYMBOL(y);
inline SYMBOL(z);

inline SYMBOL(r);
inline SYMBOL(s);
inline SYMBOL(t);

const static std::vector<Symbol> g_globalKoords = {x,y,z};
const static std::vector<Symbol> g_isometricKoords = {r,s,t};

static bool g_ComputeShaderBackendEnabled = false;
static float g_glVersion = 0;

static std::string g_vendorCorp = NULLSTR;
static bool g_CudaBackendEnabled = false;

static Color g_backgroundColor = Color(30,30,30,255);

static std::string githubRepositoryUrl = "https://github.com/tr3dh/ALF";

static std::string g_encoderKey = "ALF";

static std::string g_env = "UNKNOWN"; // std::getenv("MSYSTEM");