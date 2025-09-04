#include "Curl.h"

void OpenLink(const char* url) {
    
#if defined(_WIN32)
    std::string cmd = "start " + std::string(url);
#elif defined(__APPLE__)
    std::string cmd = "open " + std::string(url);
#else // Linux
    std::string cmd = "xdg-open " + std::string(url);
#endif

    cmd = "cmd.exe /C \"" + cmd + "\"";

    runWinCommand(cmd);
}