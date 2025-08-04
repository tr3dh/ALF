#include "templateDecls.h"

#undef LOG
#undef _ERROR

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
        LOG << "   um mehr zu Erfahren die API mit dem Argument '--help' aufrufen" << endl;
        LOG << "   zum Fortfahren beliebige Taste drücken..." << endl;

        std::cin.get();
        return 1;
    }

    std::string exeDir = getExecutableDir();
    std::string cwd = std::filesystem::current_path().string();

    std::unordered_map<std::string, std::string> options = {
        {"--license", "view license >> 0 args requiered"},
        {"--version", "view version >> 0 args requiered"},
        {"simulate", "simulate FEM-Model >> 1 args requiered {Path/to/Model},\n\t\t\t\toptional third argument --absolut if the path is absolut"}
    };

    if(std::string(argv[1]) == "--help"){

        LOG << "Valid Commandline options are :" << endl;
        LOG << endl;

        for(const auto& [cm, desc] : options){
            LOG << "\t" << cm << "\t\t" << desc << endl;
        }
    }

    else if(std::string(argv[1]) == "--version"){

        std::ifstream file(exeDir + "/../.VERSION");
        if (!file) {
            _ERROR << "keine gültige Version gefunden an " << exeDir + "/../.VERSION" << endl;
            return 1;
        }

        LOG << endl;

        std::string line;
        while (std::getline(file, line)) {
            LOG << "\t" << line << endl;
        }
    }

    else if(std::string(argv[1]) == "--license"){
        
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

    else if(std::string(argv[1]) == "simulate"){

        RETURNING_ASSERT(argc > 2, "bitte Pfad des Modells für die Simulation übergeben", 1);

        std::string path;

        if(argc > 3 && std::string(argv[3]) == "--absolut"){
            path = argv[2];
        }
        else{
            path = cwd + "/" + argv[2];
        }

        std::filesystem::current_path(exeDir);
        LOG << "Arbeitsverzeichnis auf " << exeDir << " gesetzt" << endl;

        LOG << "Start Unpacking and Simulating " << path << endl;

        FemModel model(path);
        model.storeResults();
    }

        // Programm ohne args gestartet
    else {

        LOG << "!! API wurde ohne valide Argumente angesprochen, invalider Aufruf" << endl;
        LOG << "   um mehr zu Erfahren die API mit dem Argument '--help' aufrufen" << endl;
        LOG << "   zum Fortfahren beliebige Taste drücken..." << endl;

        std::cin.get();
        return 1;
    }

    return 0;
}