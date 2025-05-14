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

namespace fs = std::filesystem;

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

#include "Drivers/SFML/__sfLine.h"
#include "Drivers/SFML/__sfPolygon.h"
#include "Drivers/SFML/__sfLog.h"

#include "Drivers/Raylib/__rlLogging.h"

#include "Drivers/Visualisation/__Coloration.h"

#include "Drivers/UI/__fileBrowser.h"

inline SYMBOL(x);
inline SYMBOL(y);
inline SYMBOL(z);

inline SYMBOL(r);
inline SYMBOL(s);
inline SYMBOL(t);

const static std::vector<Symbol> g_globalKoords = {x,y,z};
const static std::vector<Symbol> g_isometricKoords = {r,s,t};

typedef uint16_t NodeIndex;
typedef NodeIndex CellIndex;