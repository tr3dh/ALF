#pragma once

// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden

// std includes
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <ranges>

namespace fs = std::filesystem;

#include <string>
#include <optional>

// Numeric includes
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>

//
#if defined(LOG) | defined(_ERROR)
#error "Logging Direktiven können nicht definiert werden, Makros bereits deklariert"
#endif

#define LOG std::cout
#define _ERROR std::cerr

// Drivers
#include "Drivers/__Asserts.h"
#include "Drivers/__StringProcessing.h"
#include "Drivers/__SymbolicExpressions.h"
#include "Drivers/__SymEngineMatrix.h"

extern Symbol x,y,z;            // globale Koordinaten  
extern Symbol r,s,t;            // Koordinaten im isoparametrischen Element