#pragma once

// includes für den asserst Teil
#include <assert.h>
#include <iostream>
#include <sstream>
#include <cstdlib>

#ifndef LOG
#define LOG std::cout
#endif // LOG

#ifndef _ERROR
#define _ERROR std::cerr
#endif // _ERROR

#define TRIGGER_ASSERT  0
#define PASS_ASSERT     1

#define ASSERT(condition, message) \
    if (!(condition)) { \
        std::ostringstream oss; \
        oss << "Assertion failed:\n" \
            << "\t\\ Debug Instruction : " << message << "\n" \
            << "\t\\ File: " << __FILE__ << "\n" \
            << "\t\\ Line: " << __LINE__ << "\n" \
            << "\t\\ Function: " << __FUNCTION__; \
        _ERROR << oss.str() << "\n" << endl; \
    }

#define CRITICAL_ASSERT(condition, message) \
    ASSERT(condition, message); \
    if (!(condition)) { \
        std::terminate(); \
    }

// define dient eigentlich nur dazu, dass der file nicht auf eine durch '\' angehängte Zeile oder '//' endet
// das führt zu einer Warnung im GCC da Skripte so nicht enden dürfen
#define ASSERTS_DEFINED