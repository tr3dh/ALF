#pragma once

// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden
// Dieser Header wird in einen precompiled header (*.h.gch) umgewandelt
// Dementsprechend sollte er nur thirdParty oder Skripte einbinden die nicht mehr verändert werden

#include <memory>
#include <ranges>
#include <string>
#include <vector>
#include <iostream>

// Lib inkludes die Std Lib Makros nutzen die raylib bzw. die für raylib geschaffene Umgebung überschreibt müssen vorher
// eingebunden werden

// #include <llvm/IR/IRBuilder.h>
// #include <llvm/IR/LLVMContext.h>
// #include <llvm/IR/Module.h>
// #include <llvm/IR/Verifier.h>
// #include <llvm/Support/TargetSelect.h>
// #include <llvm/MC/TargetRegistry.h>
// #include <llvm/Target/TargetMachine.h>
// #include <llvm/Support/FileSystem.h>
// #include <llvm/Support/raw_ostream.h>
// #include <llvm/IR/LegacyPassManager.h>
// #include <llvm/IR/Constants.h>
// #include <llvm/Bitcode/BitcodeWriter.h>

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
#include <raylib.h>
#include <raymath.h>

#include <rlImGui/rlImGui.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#if defined(_WIN32)

// To avoid conflicting windows.h symbols with raylib, some flags are defined
// WARNING: Those flags avoid inclusion of some Win32 headers that could be required
// by user at some point and won't be included...
//-------------------------------------------------------------------------------------

// If defined, the following flags inhibit definition of the indicated items.
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
//#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
//#define NOMINMAX          // Macros min(a,b) and max(a,b)

#ifndef NOMINMAX
#define NOMINMAX          // Verhindert Konflikte mit min/max Makros von Windows
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Reduziert Windows-Header, schnelleres Kompilieren
#endif

#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions

// Type required before windows.h inclusion
typedef struct tagMSG *LPMSG;

#include <windows.h>

// Type required by some unused function...
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#include <objbase.h>
#include <mmreg.h>
#include <mmsystem.h>

// Some required types defined for MSVC/TinyC compiler
#if defined(_MSC_VER) || defined(__TINYC__)
    #include "propidl.h"
#endif
#endif

#define IMGUI_FILEBROWSER_NO_EXCEPTIONS
#include <imgui-filebrowser/imfilebrowser.h>

#include <implot.h>

#include <rlgl.h>
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

// Debugging über message ausgabe und instance debugging, dass direkt überladung für string stream mit << aufruft
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