# ToDo

# Prim

. Precompiles verbessern includes in includes templs in defines

. getprefabs nur an relevanter stelle

. segfault beim mehrfachen reload (-> wenn cached B Mats im isomesh einkommentiert sind und serialisiert werden)
. nur wenn innerhalb der ui der reload geschieht

. serialisierungen komplett auf to/from bs umstellen und in cpp auslagern, sodas weniger kompilieraufwand durch vorkompilierung in o file

. hintergrund farbe anpassungs dialog

. smoothed fps

. weiteres 3d format testen und checken ob rendering switch klappt

. Parser kopiert expression string ???
. ref in parser übergabe

. pwd auch im programm anpassen sodass nicht immer vollständige Navigatrion nötig ist
--> management in main sodass der sich die pfade der jeweiligen Anwendungen speichert und setzt
. bin für cwd des explorers

. Menubalken und notebookck checken >> wird alles gebracuht ?? alle schaltflächen ??

. plus minus xi ist irgendwie vertauscht

. mit hendrik reden / klären was strain xx strain yy und strain yz (ebenfalls für 3d ist)

. bei file new kann man inp aus wählen und der wird kpiert und mit nem .model initialisiert ???

. space für fps und resampling ???

\(  \scalebox{2}{$\rlap{\rule[0.65ex]{1.4em}{0.1pt}}{\Omega \iota \kern-0.4em \int}$} \)

\(  \rlap{\rule[0.65ex]{1.4em}{0.1pt}}{\Omega \iota \kern-0.4em \raisebox{0.2ex}{\(\scriptstyle\int\)}} \)

\(  \rlap{\rule[0.65ex]{1.4em}{0.1pt}}{\Omega \iota \kern-0.1em \raisebox{0
.4ex}{\(\scriptstyle\int\)}} \)

\scalebox{2}{$\rlap{\rule[0.65ex]{1.2em}{0.1pt}}{\Omega \kern-0.08em \iota \kern-
0.45em \int}$}

\scalebox{2}{$\rlap{\rule[0.65ex]{1.2em}{0.1pt}}{\bm \Omega \kern-0.08em \iota \kern-
0.45em \int}$}

abspeichern label im modell und dann abfrage bei idx ???

koord system 

wireframe dicke einstellen

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