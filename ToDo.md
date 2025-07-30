# ToDo

# Änderungen
. if abfrage multiline input text einfügen

# neu
. inputExpression für strings implementieren

# Prim
. !! bei sampling der pdf im nicht lin model display wird assertion ausgelöst

!! <ERROR> !! -> Assertion failed:
        \ Debug Instruction : ├╝bergebene Nodes und Displacement sind inkompatibel
        \ File: src/Mesh/Mesh_Solve.cpp
        \ Line: 9
        \ Function: displaceNodes

. API schreiben
        . Befehle :
        . do fem
        . wich node -> pos node
        . wich elem -> elem info
        . info über spannungen im element
        . init window ohne display und auf render texture rendern
        -> png und als asci konvert im terminal printen

. Precompiles verbessern includes in includes templs in defines

. getprefabs nur an relevanter stelle

. segfault beim mehrfachen reload (-> wenn cached B Mats im isomesh einkommentiert sind und serialisiert werden)
. nur wenn innerhalb der ui der reload geschieht

. serialisierungen komplett auf to/from bs umstellen und in cpp auslagern, sodas weniger kompilieraufwand durch vorkompilierung in o file

. weiteres 3d format testen und checken ob rendering switch klappt

. hintergrund farbe anpassungs dialog

. Falsche schreibweise ::: INDIDES in .ISOPARAM C3D8R und Ladelogik

. Parser kopiert expression string ???
. ref in parser übergabe

. pwd auch im programm anpassen sodass nicht immer vollständige Navigatrion nötig ist
--> management in main sodass der sich die pfade der jeweiligen Anwendungen speichert und setzt
. bin für cwd des explorers

. Menubalken und notebookck checken >> wird alles gebracuht ?? alle schaltflächen ??

. plus minus xi ist irgendwie vertauscht

. smoothed fps

. mit hendrik reden / klären was strain xx strain yy und strain yz (ebenfalls für 3d ist)

. bei file new kann man inp aus wählen und der wird kpiert und mit nem .model initialisiert ???

# Releases
. Release statt Debug build initialisieren
. in Software release beide anbiete
. bei Release schreibt LOG in file (.> atomic Logger)

# CleanUp
. Compiler Warnungen beheben
. aufteilung in incl/src ?? -> angenehmer für Nutzungs als lib
. if mat has pdf einfügen
. size == 0 checks ins rendering
. redirect rayliblog

# Releases
. Release (native)
. Debug (console)
. libary
. headlesss api

# alg. Rendering
. angabe maximal/minimalwerte
. werte mittlung aus frame auslagern --> nur bei jedem update
. orbit Camera führung um camera.target einfügen
. shader Pipeline

# Ergebnis ausgabe
. in Ui
. größe und koord über UI steuern
. Ergebnis file als bin body und json

# Ideen
. Unsicherheitsberechnung mit mehr Taylor Reihengliedern für Ansätze
. 3d Rendering der deformierten Netze in Raylib
. cachen von Kmatrix, fem Ergebnisse in modell ordner und nur laden wenn änderungsdatum sich verändert hat
. compute shader mit raylib
. automatische bytesequenz serialisierung über pfr (boost und standalone)

# Markdowns
. vec x_n+1 fixen (pfeil mittig über gesamte Benennung gerendert)
. lin Fem
. nicht lin fem -> entsprechende Verfahren
. Übersicht Treiberstruktur/Projektstruktur
. shader
. comp shader

# FormelRendering
. MicroTeX

# .LINKS
https://catlikecoding.com/unity/tutorials/pseudorandom-noise/simplex-noise/
https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/simplexnoise.pdf
https://www.youtube.com/watch?v=zI41iCSwEFs&t=2s
https://github.com/EFV0804/vulkan_compute_shader/blob/main/simple_compute_exemple/simple_compute_exemple.cpp
https://github.com/FacundoGFlores/NSL
https://github.com/TendTo/Linear-System-Solver