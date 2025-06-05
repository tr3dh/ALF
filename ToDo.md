# ToDo

# Prim
. Jacoby und Residuum nicht in SymEngine::DenseMatrix sondern eingene SymTriplet listen schreiben
  mit Symtriplet = tuple(x,y,Expr)

# CleanUp
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
. legende für linienfarben/einfärbungen
. angabe maximal/minimalwerte
. autofit (camera/scale + offset) aus frame auslagern --> nur bei jedem update
. werte mittlung aus frame auslagern --> nur bei jedem update
. orbit Camera führung um camera.target einfügen

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
