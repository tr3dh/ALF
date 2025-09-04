#include "ClearCaches.h"
#include "WinHandle/WinCmd.h"

// Wrap damit windows befehle auch unter msys laufen >> Powershell aufmachen und darin aufrufen
// powershell -Command \" command \"

void clearCache(const std::string& modelPath) {

    if (!fs::exists(modelPath)) return;

    std::string cacheFile = modelPath + "\\.RESULTCACHE";
    if (!fs::exists(cacheFile)) return;

    // Lösche .RESULTCACHE Datei
    runWinCommand(("cmd.exe /C del /F /Q \"" + cacheFile + "\"").c_str());
}

void clearCaches(){

    // Löscht alle .RESULTCACHE Dateien rekursiv aus Import
    runWinCommand("cmd.exe /C for /R ..\\Import %f in (*.RESULTCACHE) do @del /F /Q \"%f\"");
}

void clearBin() {

    // Löscht alle Dateien in ../bin außer .LOG
    runWinCommand("cmd.exe /C \"for %f in (..\\bin\\*) do @if /I not \"%~nxf\"==\".LOG\" del /Q \"%f\"\"");

    // Löscht alle Unterordner in ../bin
    runWinCommand("cmd.exe /C \"for /D %d in (..\\bin\\*) do @rmdir /S /Q \"%d\"\"");
}