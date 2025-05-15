#pragma once

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <iostream>

#ifndef LOG
#define LOG std::cout << "__INV_LOGGING_FUNC"
#endif 

//
static bool g_rlLoggingEnabled = false;
static std::string g_rlLogPrefix = "[redirected RaylibLog] "; 

// Für Vector2
std::ostream& operator<<(std::ostream& os, const Vector2& v);

// Eigene Ausgabe für Raylib Log
void RaylibLogCallback(int logType, const char* text, va_list args);

//
void enableRLLogging();

//
void disableRLLogging();