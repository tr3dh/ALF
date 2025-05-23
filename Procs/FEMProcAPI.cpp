#include "defines.h"

#define LOG std::cout
#define _ERROR std::cerr

std::string getExecutablePath() {
    
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    return fullPath;
}

std::string getExecutableDir() {
    std::string fullPath = getExecutablePath();
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return fullPath.substr(0, pos);
    }
    return "";
}

int main(int argc, char* argv[]){

    // argc ist anzahl übergebener Argumente + 1
    // argv dann das array der größe argc die die übergebene Argumente aus char Ptrs enthält
    // um auf n-tes übergebenes argument zu kommen argv[n] abfragen -> Indizierung ab 1 nicht ab Null

    // Programm ohne args gestartet
    if(argc < 2){

        LOG << "!! API wurde ohne Argumente angesprochen, invalider Aufruf" << endl;
        LOG << "   zum Fortfahren beliebige Taste drücken..." << endl;

        std::cin.get();
        return 1;
    }

    std::string exeDir = getExecutableDir();

    if(std::string(argv[1]) == "--version"){
        LOG << "Version v0.0.1" << endl;
    }

    if(std::string(argv[1]) == "--license"){
        
        std::ifstream file(exeDir + "/../LICENSE.txt");
        if (!file) {
            _ERROR << "keine gültige Lizenz gefunden an " << exeDir + "/../LICENSE.txt" << endl;
            return 1;
        }

        LOG << endl;

        std::string line;
        while (std::getline(file, line)) {
            LOG << "\t" << line << endl;
        }
    }

    return 0;
}