#pragma once

// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden

// std includes
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <array>
#include <tuple>
#include <random>
#include <stdio.h>
#include <utility>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace fs = std::filesystem;
typedef fs::file_time_type fileTime;

#include <string>
#include <optional>

// Numeric includes
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/SparseCholesky>
#include <eigen3/Eigen/SparseLU>

//
#include <SFML/Graphics.hpp>

//
#include <raylib.h>
#include <raymath.h>
#include <r3d.h>

#include <rlImGui/rlImGui.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h> 

// aufgrund von Konflikten mit der WinAPI
#define NOGDI
#define NOUSER
#include <imgui-filebrowser/imfilebrowser.h>
#include <implot.h>

#include <GL/gl.h>

//
#if defined(LOG) | defined(_ERROR)
#error "Logging Direktiven können nicht definiert werden, Makros bereits deklariert"
#endif

#define LOG_RED     "\033[31m"
#define LOG_GREEN   "\033[32m"
#define LOG_YELLOW  "\033[93m"
#define LOG_BLUE    "\033[34m"
#define LOG_ORANGE  "\033[38;2;255;165;0m"
#define LOG_RESET   "\033[0m"

#define LOG std::cout << LOG_ORANGE
#define _ERROR std::cerr << LOG_RED << "!! <ERROR> !! -> " 

#define endl LOG_RESET << "\n";

#define mbug(message) LOG << LOG_YELLOW << "___Passed : " << #message << endl;
#define ibug(objekt) LOG << LOG_YELLOW << "___Objekt : " << #objekt << " " << objekt << endl;

// Drivers
#include "Drivers/ReccHandling/__Asserts.h"
#include "Drivers/ReccHandling/__StringProcessing.h"
#include "Drivers/ReccHandling/__jsonSerialize.h"
#include "Drivers/ReccHandling/__lineCounter.h"

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

#include "Drivers/Prompts/__Curl.h"

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

static std::string githubRepositoryUrl = "https://github.com/tr3dh/FEMProc";

static std::string g_encoderKey = "ALF";