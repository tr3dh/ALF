#include "GetEnv.h"

void getEnv(){

    const char* msys = std::getenv("MSYSTEM");
    if (msys) g_env = "MSYS"; return;

    const char* ps = std::getenv("PSModulePath");
    if (ps) g_env = "POWERSHELL";
}