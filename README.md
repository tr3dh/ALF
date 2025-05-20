# Aufbau
- MinGW64 Subsystem
- mögliche Installationen über pacman oder mingw64/cmake (->reproduzierbar)
- das alles kann dann über eine makebefehl eingerichtet werden

# Tools
- Buildsystem : Makefiles
- Compiler : gcc/g++
- Sprachstandard : C++23

# Features
| Feature                                                                           | Ab Version |
|-----------------------------------------------------------------------------------|:----------:|
| Lösen und Visualisieren von linearen 2D und 3D FEM Problemen                      |  v0.0.1    |
| Linearisierung der Lösungen für zufallsverteilte Materialien um Arbeitspunkt      |  v0.0.1    |
| Bearbeiten der Materialien innerhalb der UI                                       |  v0.0.1    |
| Rejection Sampling für Material pdfs                                              |  v0.0.1    |
| Dynamisches Laden von isoparametrischen Elementen, pdfs, Prefabs, Meshtypen       |  v0.0.1    |
| Erweiterung durch eigene Prefabs (Pipelines)                                      |  v0.0.1    |


# Setup für Kompilierung
- Installation von msys64
- clonen vom github repo
- batch skript ausführen, das vscode workspace öffnet
- optional : keybindings für runtask einfpgen
- ctrl + shift + P -> globale keybindings öffnen
```json
// folgendes in globale keybindings json einfügen
[
    {
    "key": "ctrl+shift+r",
    "command": "workbench.action.tasks.runTask",
    "when": "editorTextFocus"
    }
]
```
- task "open msys mingw64 shell" ausführen
- bei permission error muss benutzer schreibzugriff auf den installtionsordner von msys (standardmäßig c:/msys64) freigeschaltet werden
- benutzung nicht einwandfrei möglich wenn shell im /tmp ordner läuft
- make mit pacman installieren : pacman -S --noconfirm make
- make update > PacketDatenbank vom pacman updaten
- make prefab > Einrichtung von Subsystem und Projekt (kann bis zu 15/20 Minuten dauern)
- make > Kompilierung
- make exec > Ausführung der erzeugten exe

# Kompilierung
- minimierter Overhead durch :
    - inkrementelle Kompilierung
    - precompiled header : defines.h (externe includes und Treiber includes)
    - precompiled templates : src/bin/*.cpp (effektiv durch inkrementelle Kompilierung)

# Logging
- wird über Preprozessor Direktiven LOG und _ERROR angestellt
- Vorteil, dass diese dann je nach Build (Debug/Release, native/console) über den Preprozessor angepasst werden können
- z.B kann Log für den Debug Mode in die Konsole Schreiben und im Release in einen Logfile
- Verwendung von endl antstatt std::endl, da für endl (Makro definiert mit \n) nicht jedes mal der Ausgabe buffer geflusht wird 

# Debugging
- Debug Message für Pass Test über mbug(MessageString_Without_QuatationMarks)
- Debug Instance mit Werte und Variablennamen ausgabe per ibug(objekt) !! Objekt Logging muss also definiert sein 

# Pfadangaben
- der makefile setzt vor ausführung innerhalb des 'exec' Befehls das cwd auf das Elternverzeichnis der executable
- das liegt daran das bei Ausführung über 'make exec' ohne den cwd Wechsel sich dieses in einem anderen Verzeichnis befindeen würde (Projektverzeichnis)
- Damit würden die Pfadangaben, die gemacht werden müssten inkonsistet sein (bspl Recc/... für 'make exec' ../Recc für ececutable)
- Um diese Probleme der abweichenden Pfadangaben zu umgehen befindet sich das cwd auch für exec im Elternverzeichnis der executable (build/*)
- Pfadangaben die relativ zum Projektverzeichnis sind (z.b src/main.cpp) müssen also angepasst werden (folglich ../src/main.cpp)

# AsciGen
- Schriftart ANSI shadow

# shortCuts
| Shortcut              | Funktion                          |
|-----------------------|-----------------------------------|
| l hold                | Normal Planar Kamera              |
| r hold                | Orbital Kamera                    |
| l+r hold              | FPS Kamera                        |
| w/a/s/d/shift/space   | Bewegung in fps Kamera            |
| F11                   | toggle Fullscreen                 |
| C                     | toggle Cursor                     |
| Space                 | Resampling der pdf                |

# für Markdowns
für korrektes Rendering im github
- kein sternchen zwischen einzelnen inline Math blocks anstelle dessen \cdot
- zwischen outline matblocks und einer Zeile einen Absatz
- besser als block: math codeblock

# .LINKS
- [LUHCluster](https://login.cluster.uni-hannover.de/pun/sys/dashboard)
- [PeridicTableOfElements](https://www-users.cse.umn.edu/~arnold/femtable)
- [DataSets](https://people.sc.fsu.edu/~jburkardt/DataSets)
- [AsciGen](https://www.asciiart.eu/text-to-ascii-art)
