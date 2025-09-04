// Minimale Umgebung die Terminal öffnet, die AlfUI in der Debug Config aufruft
// den Rückgabestatus entgegennimmt und das Terminal offenhält bis der Nutzer es schließt
// die Debug Version schließt das Terminal beim Absturz, die Umgebung hält ein Terminal auch bei Abstürzen offen
// sodass Debug Nachrichten angezeigt werden können
// kann auch händisch gemacht werden aber so geht es etwas scheller

#include <cstdlib>
#include <iostream>
#include "defines.h"
#include "templateDecls.h"

#ifdef LOG
#undef LOG
#endif

#ifdef _ERROR
#undef _ERROR
#endif

#ifdef endl
#undef endl
#endif

#define LOG std::cout
#define _ERROR std::cerr
#define endl "\n"

int main(){

    mkdir("../bin");

    #ifdef NDEBUG

    //
    g_logFile = std::ofstream("../bin/.LOG");
    std::cout.rdbuf(g_logFile.rdbuf());

    #endif

    LOG << "-------------------------------------------------------------------\n";
    LOG << "\tStarte Debug Env für die ALF UI\n";
    LOG << "-------------------------------------------------------------------\n";

    int result = system("powershell -Command \"./ALF_d\"");

    LOG << endl;
    LOG << "-------------------------------------------------------------------\n";
    LOG << "\tALF UI mit Status " << result << " beendet" << endl; 

    LOG << "\tZum fortfahren beliebige Taste drücken\n";
    LOG << "-------------------------------------------------------------------\n" << endl;

    #ifdef DEBUG
    std::cin.get();
    #endif

    #ifdef NDEBUG

    // Release build
    g_logFile.close();

    #endif

    return result;
}