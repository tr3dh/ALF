#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{
    //
    LOG << std::fixed << std::setprecision(10);
    LOG << endl;

    //
    Quad4Cell::deriveShapeFunctions();

    //
    Mesh mesh;
    mesh.loadFromFile("Meshes/Job-1.inp");

    mesh.createStiffnesMatrix();

    mesh.applyForces({{121,{{0,2}}}});
    mesh.fixNodes({{1,{0,1}},
                    {11,{1}}});

    mesh.solve();

    mesh.calculateStrainAndStress();

    mesh.display(Mesh::MeshData::VANMISES_STRESS, 0, true, false, -200,3500);
    mesh.display(Mesh::MeshData::VANMISES_STRESS, 0, true, true, -200,3500);

    return 0;
}

