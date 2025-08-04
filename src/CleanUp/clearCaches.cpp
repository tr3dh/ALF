#include "ClearCaches.h"

void clearCaches(){

    system("powershell -Command \"Get-ChildItem -Path ../Import -Filter *.RESULTCACHE -Recurse | Remove-Item\"");
    system("powershell -Command \"Get-ChildItem -Path ../Import -Filter *.RESULTS -Recurse | Remove-Item\"");
}

void clearBin(){
    
    system("powershell -Command \"rm -r ../bin\"");
}