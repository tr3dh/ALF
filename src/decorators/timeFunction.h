#pragma once 

#include "defines.h"

// Decorator nimmt Funktion, gewünschte Ausführungsanzahl und Funktionsargumente entgegen 
// führt Funktion entsprechend häufig aus und misst die entsprechende Zeit um die sich der thread
// in Folge verzögert
template<typename Func, typename... Args>
void timeFunction(Func func, int numExecutions, Args&&... args) {

    // start Zeitpunkt
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numExecutions; ++i) {
        func(std::forward<Args>(args)...);
    }

    // Endzeit
    auto end = std::chrono::high_resolution_clock::now();

    // Berechnen und Ausgeben der Dauern
    std::chrono::duration<double> elapsed = end - start;
    LOG << "Ausführungszeit: " << elapsed.count() << " Sekunden\n";
    LOG << "Ausführungszeit pro Aufruf " << elapsed.count()/numExecutions << " Sekunden\n";
}