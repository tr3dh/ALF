// Includes die für Nutzung von LLVM gebraucht werden
//
// Wichtig: >>  LLVM kollidiert mit von Windows, Raylib, etc gesetzten Makros
//              aus diesem Grund muss llvm immer VOR diesen Makro definitionen includiert werden
//              deshalb sind auch die '-include PCH' aus dem Kompiler aufruf gelöscht worden damit 
//              diese die Definitionen nicht automatisch am Skript anfang erledigen
//
//          >>  zudem ist der llvm include sehr groß, template reich und verzögert/crasht jeden build wenn die header
//              in tatsächlich jeder cpp eingebunden sind
//              Aus diesem Grund sollten die llvm Includes wirklich nur in den cpp files stattfinden, die 
//              Funktionen deklarieren, die llvm Funktionen benötigen
//              Auf KEINEN Fall die llvm Header in übergeordnete Projekt Header einbinden, das friert die Kompilierung
//              komplett ein
//
// Awendung:>>  exemplarischer Aufbau
//              
//              expLLVM.h
//              {
//                  #include "defines.h"
//                  void helloWorldLLVM();
//              }
//
//              expLLVM.cpp
//              {
//                  #include "llvm/includes"    |   <<>>   LLVM_INLUDE("expLLVM.h")
//                  #include "expLLVM.h"        |       
//
//                  void helloWorldLLVM(){
//                      
//                      llvm::...
//                      return;
//                  }
//              }

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#ifdef USE_LLVM

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Constants.h>
#include <llvm/Bitcode/BitcodeWriter.h>

#endif

// optional
// #include "defines.h"