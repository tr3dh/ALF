// Minimale Umgebung die Terminal öffnet, die AlfUI in der Debug Config aufruft
// den Rückgabestatus entgegennimmt und das Terminal offenhält bis der Nutzer es schließt
// die Debug Version schließt das Terminal beim Absturz, die Umgebung hält ein Terminal auch bei Abstürzen offen
// sodass Debug Nachrichten angezeigt werden können
// kann auch händisch gemacht werden aber so geht es etwas scheller

#include <cstdlib>
#include <iostream>
#include "templateDecls.h"

#ifdef LOG
#undef LOG
#endif

#ifdef _ERROR
#undef _ERROR
#endif

#ifdef ENDL
#undef ENDL
#endif

#define LOG std::cout
#define _ERROR std::cerr
#define ENDL "\n"

int main(){

    mkdir("../bin");

    openLogFile();

    LOG << "-------------------------------------------------------------------\n";
    LOG << "\tStarte Debug Env für die ALF UI\n";
    LOG << "-------------------------------------------------------------------\n";

    int result = system("powershell -Command \"./ALF_d\"");

    LOG << ENDL;
    LOG << "-------------------------------------------------------------------\n";
    LOG << "\tALF UI mit Status " << result << " beendet" << ENDL; 

    LOG << "\tZum fortfahren beliebige Taste drücken\n";
    LOG << "-------------------------------------------------------------------\n" << ENDL;

    #ifdef DEBUG
    std::cin.get();
    #endif

    closeLogFile();

    return result;
}