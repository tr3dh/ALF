# Aufbau
- MinGW64 Subsystem
- mögliche Installationen über pacman oder mingw64/cmake (->reproduzierbar)
- das alles kann dann über eine makebefehl eingerichtet werden

# Tools
- Buildsystem : Makefiles
- Compiler : gcc/g++
- Sprachstandard : C++23

# Setup
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