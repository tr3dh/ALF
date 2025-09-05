#include "llvm/testScript.h"
#include "templateDecls.h"

int main(){

    //
    mkdir("../bin");

    openLogFile();

    llvmHelloWorld();

    closeLogFile();

    return 0;
}