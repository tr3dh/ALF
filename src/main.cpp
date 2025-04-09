#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    Quad4Cell::deriveShapeFunctions();
    
    // dieses Anwendungsbeispiel ist identisch mit dem aus der Übung 5 der Fem1 Vorlesung 21/22 (Skript Hu5.m)
    // es dient zur Validierung des Programms

    //  
    Mesh mesh;
    mesh.loadFromFile("HU5_FEM1Example.inp");

    mesh.createStiffnesMatrix();

    mesh.applyForces({{6,{{1,-1000}}}});
    mesh.fixNodes({{1,{0,1}}, {3,{0,1}}, {4,{0,1}}});

    mesh.solve();
    mesh.display(-300,50);

    return 0;
}

