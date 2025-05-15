#include "__rlLogging.h"

// Für Vector2
std::ostream& operator<<(std::ostream& os, const Vector2& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

// Eigene Ausgabe für Raylib Log
void RaylibLogCallback(int logType, const char* text, va_list args) {

    if(!g_rlLoggingEnabled){
        return;
    }

    char formatted[512];
    vsnprintf(formatted, sizeof(formatted), text, args);

    //
    LOG << g_rlLogPrefix << formatted << "\n";  // Weiterleitung in LOG Makro
}

//
void enableRLLogging(){
    g_rlLoggingEnabled = true;
}

//
void disableRLLogging(){
    g_rlLoggingEnabled = false;
}