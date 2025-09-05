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

#ifndef ENDL
#define ENDL "\n"
#endif // _ERROR

#define TRIGGER_ASSERT  0
#define PASS_ASSERT     1

#define RESOLVE_CONDITION(condition)\
    bool resolvedCondition = false;\
    if(condition){\
        resolvedCondition = true;\
    } 

#define ASSERT(condition, message)\
    do{\
        if (!(condition)) {\
            std::ostringstream oss;\
            oss << "Assertion failed:\n"\
                << "\t\\ Debug Instruction : " << message << "\n"\
                << "\t\\ File: " << __FILE__ << "\n"\
                << "\t\\ Line: " << __LINE__ << "\n"\
                << "\t\\ Function: " << __FUNCTION__;\
            _ERROR << oss.str() << "\n" << ENDL;\
        }}\
    while(0);

#define CRITICAL_ASSERT(condition, message)\
    do{\
        RESOLVE_CONDITION(condition);\
        ASSERT(resolvedCondition, message);\
        if (!(resolvedCondition)) {\
            std::terminate();\
        }\
    }\
    while(0);

// für
#define RETURNING_ASSERT(condition, message, returnVal)\
    do{\
        RESOLVE_CONDITION(condition);\
        ASSERT(resolvedCondition, message);\
        if (!(resolvedCondition)) {\
            return returnVal;\
        }\
    }\
    while(0)\

#define RETURNING_VOID_ASSERT(condition, message, returnVal)\
    RETURNING_ASSERT(condition,message,)

// define dient eigentlich nur dazu, dass der file nicht auf eine durch '\' angehängte Zeile oder '//' endet
// das führt zu einer Warnung im GCC da Skripte so nicht enden dürfen
#define ASSERTS_DEFINED