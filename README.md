# <h1> <span style="font-size: 1.3em;">A</span>daptive <span style="font-size: 1.3em;">L</span>ightweight <span style="font-size: 1.3em;">F</span>inite Element Tool (ALF) </h1>

<img align="left" style="width:140px" src="Recc/Compilation/iconStripped.png" width="288px">

ALF ist ein leichtgewichtiges, adaptives FEM Programm, das im Rahmen einer [Studienarbeit](Recc/Thetis/Studienarbeit.pdf) für das Institut für Kontinuumsmechanik (IKM) der Leibniz Universität Hannover (LUH) entwickelt worden ist. Dabei liegt der Fokus auf der Erprobung verschiedener Wahrscheinlichkeitsdichten für die Unsicherheitsquantifizierung der linearen FEM und von simplen, nichtlinearen Materialmodellen für die nichtlineare FEM.

<br>

# 🌐 Download
- Programm : Vorkompilierte Binarys über den letzten [Release](https://github.com/tr3dh/FEMProc/releases) herunterladen
- Bibliothek : Vorkompilierte Binarys über den letzten [Release](https://github.com/tr3dh/FEMProc/releases) herunterladen
- Source Code : Projekt mit git klonen, Umgebung mit `make prefab` einrichten und mit `make launch` kompilieren und ausführen 

# 🧬 Entwicklung
- Sprachstandard : C++23
- Kompiler : gcc/g++
- Plattform : Windows
- Subsystem : Msys MinGw64
- BuildSystem : Make

# 🔧 Funktionen
- Durchführen von linearen Finite Elemente Analysen (FEAs)
- Durchführen von linearen FEAs mit Unsicherheitsquantifizierung
- Durchführen von nichtlinearen FEAs für simple nicht lineare Materialmodelle
- Ergebnisvisualisierung über 2D/3D-Renderer
- Variation und Erprobung der Wahrscheinlichkeitsdichten über die Benutzeroberfläche
- Steuerung der Ergebnisvisualisierung über die Benutzeroberfläche

# 🛠️ Umsetzung
Das Programm arbeitet mit einer rein dateigetriebenen Modelldefinition (Netz, Materialmodell, Randbedingungen, etc.) und Informationsbereitstellung (isoparametrisches Element, Vorlagen für Wahrscheinlicheitsdichten, etc.). Dabei werden die Informationen über gutverständliche JSON-Dateien bereitgestellt. So können schnell und ohne langwierige Einarbeitung Modelle implementiert, Programmfunktionen erweitert und Simulationen durchgeführt werden.

# 🖥️ Benutzeroberfläche
Die Benutzeroberfläche bietet die Möglichkeit Modelle zu laden und zu verwalten und die Steuerung der Ergebnisvisualisierung.
Sie ist in eine Menüleiste (oberer Fensterrand) und eine aus-/einklappbare Registeransicht (rechte Fensterhälfte) unterteilt. Die Menüleiste bietet dabei Import-/ Einstellungs- und Verwaltungsoptionen. Über die Registeransicht kann die beschleunigte Erprobung der Wahrscheinlichkeitsdichten und die Steuerung der Visualisierung angestellt werden. In dem Hauptkompartment findet das Rendering statt. 

<div style="text-align: center;">
<img src="Recc/textures/UI.png" alt="Übersicht über Benutzeroberfläche" style="border: 2px solid black; border-radius: 4px;width: 800px;" />
</div>

# 🗂️ Modellimport

<!-- <img align="left" style="width:250px" src="Recc/textures/2DRendering.png" width="288px"> -->

<img align="left" style="width:300px; margin-right: 15px;" src="Recc/visuals/Animation.gif">

Über `File->Open->Model` wird ein Dateiauswahldialog gestartet, der die Auswahl des Modellordners ermöglicht. Beim Import des Modells werden die Dateien gelesen, geparst in programminterne Strukturen übersetzt und die Simulation durchgeführt.
Diese wird im mittigen Hauptkompartement des Fensters gerendert. Die Navigation innerhalb dieser Ansicht ist für zweidimensionale Systeme nicht möglich. Für dreidimensionale Systeme kann über mehrere Kamerafahrten die Ansicht verändert werden.
Eine Übersicht über die Kameraführungen und die Steurung ist in [ShortCuts](#⌨️-shortcuts) aufgeführt.
Über die Registeransicht in der rechten Fensterhilfe können unter `Rendering` das Netz auf dem die Visualisierung erfolgt, die visualisierte Größe, etc. eingestellt werden. Unter `Rendering->Animation` gibt es einen Abspieldialog für die Navigation in den Simulationsergebnissen der nichtlinearen FEA (Animation).

# 📊 Beispielmodell

Im Folgenden ist exemplarisch die Modelldefinition für eine lineare FEM gezeigt. Weitere Beispielmodelle, die direkt importiert werden können liegen im [Import](/Import/)-Ordner. Die Definition des isoparametrischen Elements ist für mehrere Standardfälle wie einfache Dreiecks-/Vierecks- und Würfelelemente bereits hinterlegt. Verwendet das Netz ein nicht implementiertes isoparametrisches Element, muss dieses in den [Recc/Cells]-Ordner implementiert werden. Imformationen dazu können aus den Implementierungsdateien der vorhandenen Elemente unter [Recc/Cells](/Recc/Cells/) und der [Studienarbeit](/Recc/Cells/Thetis/Studienarbeit.pdf) entnommen werden. 

## 🏗️ Aufbau
```txt
Arbeitsverzeichnis des Programms/
|__ build/
|__ Recc/
|   |__ Cells/
|   |   |__ CPS4R.ISOPARAM                    // JSON-Datei
|   |   |__ CPS3.ISOPARAM                     // JSON-Datei
|   |   |__ für Modell relevantes Element     // JSON-Datei
|   |   |__ ...
|   |__ ...
|
|__ Import/
|   |__ ModelName.model/
|   |   |__ .Mesh             // INP-Datei        über Abaqus oder manuell anlegen und bearbeiten
|   |   |__ .Material         // JSON-Datei       manuell anlegen und bearbeiten
|   |   |__ .Constraints      // JSON-Datei       manuell anlegen und bearbeiten
|   |   |__ .RESULTCACHE      // Bytecode-Datei   wird vom Programm erzeugt und verwaltet
|   |__ ...
|__ ...
```

## 🕸️ Netzdefinition
Datei `.Mesh`
```txt
*Heading
...
*Node                                   // Definition der Knoten über Index und Koordinaten
    1,           0.,           0.
    ...
    121,        0.12,        0.12

*Element, type=CPS4R                    // Definition der Elemente
    1,   1,   2,  13,  12               // hier auftauchende Bennenung des isoparametrischen Elements muss bei Definition
                                        // des isoparametrischen Elements angegeben werden
    ...                                 // >> Definition über CPS4R.ISOPARAM (liegt als Standardtyp bereits vor)
    100, 109, 110, 121, 120

*End Part
...
*End Assembly
```

## 📐 Definition Randbedingungen
Datei `.Constraints`
```js
{
    //
    "Constraints" : [
        {"1" : [0,1]},
        {"11" : [1]}
    ],
    "Loads" : [
        {"11" : [{"0": 1000}]}
    ]
}
```

## 🧱 Definition Material
Datei `.Material`
```js
{
    "isLinear": true,
    // Für Unsicherheitsquantifizierung oder nichtlineare Materialmodelle
    // "nonLinearApproach": {...},
    // "pdf" : {...}
    "stdParams": {
        "E": 20000.0,
        "t": 0.1,
        "v": 0.3
    }
}
```

Weitere Informationen zur Definition der Wahrscheinlicheitsdichten und des nichtlinearen Materialmodells werden über verschiedene Beispielmodelle in [Import](/Import/) und die beigelegte [Studienarbeit](Recc/Thetis/Studienarbeit.pdf) bereitgestellt. 

# ⌨️ ShortCuts
| Shortcut              | Funktion                          |
|-----------------------|-----------------------------------|
| l hold                | Normal Planar Kamera              |
| r hold                | Orbital Kamera                    |
| l+r hold              | FPS Kamera                        |
| w/a/s/d/shift/space   | Bewegung in fps Kamera            |
| F11                   | toggle Fullscreen                 |
| C                     | toggle Cursor                     |
| Space                 | Resampling der pdf                |

# 🤝 Danksagung

Mein besonderer Dank gilt Dr. Hendrik Geisler, der diese Studienarbeit sehr spontan und durch seine Unterstützung erst ermöglicht hat.

## 📚 Verwendete Bibliotheken
Bedanken möchte ich mich zudem bei den jeweiligen Entwicklern und Maintainern der im Rahmen des Projekts verwendeten Open-Source Bibliotheken.
Diese sind im folgenden aufgeführt. Die zugehörigen Lizenztexte sind im Ordner [thirdPartyLicenses](/thirdPartyLicenses/) hinterlegt.

| Bibliothek        | Lizenz                          |
|-------------------|----------------------------------|
| [raylib](https://www.raylib.com/)             | zlib/libpng                      |
| [Eigen](https://eigen.tuxfamily.org/)         | MPL2 (Mozilla Public License 2.0) |
| [SymEngine](https://github.com/symengine/symengine) | BSD 2-Clause                     |
| [magic_enum](https://github.com/Neargye/magic_enum) | MIT                              |
| [nlohmann/json](https://github.com/nlohmann/json)   | MIT                              |
| [Boost.PFR](https://github.com/boostorg/pfr)        | Boost Software License 1.0       |
| [Dear ImGui](https://github.com/ocornut/imgui)      | MIT                              |
| [rlImGui](https://github.com/raylib-extras/rlImGui) | MIT                              |
| [ImPlot](https://github.com/epezent/implot)         | MIT                              |
| [ImGuiFileBrowser](https://github.com/AirGuanZ/imgui-filebrowser) | MIT                   |
