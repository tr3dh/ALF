#pragma once 

#include <chrono>

using chrono = std::chrono::high_resolution_clock;
using timePoint = chrono::time_point;
using duration = std::chrono::duration<double>;

#define START_TIMER \
    auto start = std::chrono::high_resolution_clock::now(); \
    auto end = std::chrono::high_resolution_clock::now(); \
    std::chrono::duration<double> elapsed = start - end;

#define RESET_TIMER\
    start = std::chrono::high_resolution_clock::now(); \

#define LOG_TIMER \
    \
    end = std::chrono::high_resolution_clock::now(); \
    elapsed = end - start; \
    LOG << "Ausführungszeit: " << elapsed.count() << " Sekunden\n"; \
    start = std::chrono::high_resolution_clock::now();

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