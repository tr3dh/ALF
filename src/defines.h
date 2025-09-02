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
#include <thread>

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

#ifdef NDEBUG

// Release build

extern std::ofstream g_logFile;

#define LOG_RED     ""
#define LOG_GREEN   ""
#define LOG_YELLOW  ""
#define LOG_BLUE    ""
#define LOG_ORANGE  ""
#define LOG_RESET   ""

#define LOG std::cout << "[" << getTimestamp() << "] : "
#define _ERROR std::cerr << "[" << getTimestamp() << "] : " << "!! <ERROR> !! -> "

#define endl LOG_RESET << "\n" << std::flush;

#else

// Debug build und alles andere

#define LOG_RED     "\033[31m"
#define LOG_GREEN   "\033[32m"
#define LOG_YELLOW  "\033[93m"
#define LOG_BLUE    "\033[34m"
#define LOG_ORANGE  "\033[38;2;255;165;0m"
#define LOG_RESET   "\033[0m"

#define LOG std::cout << LOG_ORANGE
#define _ERROR std::cerr << LOG_RED << "!! <ERROR> !! -> "

#define endl LOG_RESET << "\n";

#endif

#define mbug(message) LOG << LOG_YELLOW << "___Passed : " << #message << endl;
#define ibug(objekt) LOG << LOG_YELLOW << "___Objekt : " << #objekt << " " << objekt << endl;

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

static std::string githubRepositoryUrl = "https://github.com/tr3dh/ALF";

static std::string g_encoderKey = "ALF";

static std::string g_env = "UNKNOWN"; // std::getenv("MSYSTEM");

inline void mkdir(const std::string& path){
    
    // Prüfen, ob das Verzeichnis existiert
    if (fs::exists(path)) {
        
        // Löscht das Verzeichnis
        return;
    }

    // Verzeichnis neu erstellen
    if (fs::create_directory(path)) {
        
    } else {
        ASSERT(TRIGGER_ASSERT, "mkdir fehlgeschlagen");
    }
}