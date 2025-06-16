# ToDo

# Änderungen
. if abfrage multiline input text einfügen

# neu
. inputExpression für strings implementieren

# Prim
. Toleranz und entsprechende Dezimalstellen aus file Laden ??
. Simulation ins Mesh verlegen
. innere Variable in celldata abspeichern
. unsicherheitsverechnung in nicht linearer Simulation ??
. Rendering der Simulation über frame switch mit frame conunter der hochgezählt wird wenn Zeit ab frmaeswitch verstrichen ist
  über frame switch dauer liegt
. Auswahl der zu printenden Größe auf Netz

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