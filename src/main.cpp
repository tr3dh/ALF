#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    Quad4Cell::deriveShapeFunctions();

    Mesh::genInpFile("TMP.inp");

    //  
    Mesh mesh;
    mesh.loadFromFile("Job-1.inp");

    mesh.createStiffnesMatrix();

    mesh.applyForces({{121,{{0,2}}}});
    mesh.fixNodes({{1,{0,1}}, // {12,{0,1}}, {23,{0,1}}, {34,{0,1}}, {45,{0,1}}, {56,{0,1}}, {67,{0,1}}, {78,{0,1}}, {89,{0,1}}, {100,{0,1}}, {111,{0,1}},
                    {11,{1}}});

    mesh.solve();
    mesh.calculateStrainAndStress();
    
    mesh.display(Mesh::MeshData::VANMISES_STRESS, 0, false, -200,3500);

    return 0;
}

