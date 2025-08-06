#include "ClearCaches.h"

void clearCache(const std::string& modelPath){

    if(!fs::exists(modelPath)){
        return;
    }

    std::string cachePath = modelPath + "/.RESULTCACHE";

    if(!fs::exists(cachePath)){
        return;
    }

    std::string command = "powershell -Command \"Remove-Item -Recurse -Force '" + cachePath + "'\"";

    int result = system(command.c_str());
}

void clearCaches(){

    system("powershell -Command \"Get-ChildItem -Path ../Import -Filter *.RESULTCACHE -Recurse | Remove-Item\"");
    system("powershell -Command \"Get-ChildItem -Path ../Import -Filter *.RESULTS -Recurse | Remove-Item\"");
}

void clearBin(){
    
    system("powershell -Command \"rm -r ../bin\"");
}