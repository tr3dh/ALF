#include "Logging.h"

void openLogFile(){

    #ifdef NDEBUG

    g_logFile = std::ofstream("../bin/.LOG");
    std::cout.rdbuf(g_logFile.rdbuf());

    #endif
}

void closeLogFile(){

    #ifdef NDEBUG

    // Release build
    g_logFile.close();

    #endif
}