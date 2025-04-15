#include "defines.h"

#include "MeshLoader/Meshloader.h"
#include "Mesh/Mesh.h"

int main(int argc, char** argv)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    Cell quad4 = Cell("Recc/Cells/CPS4R.ISOPARAM");

    //
    return 0;

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

    mesh.display(Mesh::MeshData::STRESS, 0, false, false, -200,3500);
    mesh.display(Mesh::MeshData::STRESS, 0, false, true, -200,3500);
    mesh.display(Mesh::MeshData::STRESS, 1, false, false, -200,3500);
    mesh.display(Mesh::MeshData::STRESS, 1, false, true, -200,3500);
    mesh.display(Mesh::MeshData::STRESS, 2, false, false, -200,3500);
    mesh.display(Mesh::MeshData::STRESS, 2, false, true, -200,3500);

    mesh.display(Mesh::MeshData::STRAIN, 0, false, false, -200,3500);
    mesh.display(Mesh::MeshData::STRAIN, 0, false, true, -200,3500);
    mesh.display(Mesh::MeshData::STRAIN, 1, false, false, -200,3500);
    mesh.display(Mesh::MeshData::STRAIN, 1, false, true, -200,3500);
    mesh.display(Mesh::MeshData::STRAIN, 2, false, false, -200,3500);
    mesh.display(Mesh::MeshData::STRAIN, 2, false, true, -200,3500);

    mesh.display(Mesh::MeshData::VANMISES_STRESS, 0, false, false, -200,3500);
    mesh.display(Mesh::MeshData::VANMISES_STRESS, 0, false, true, -200,3500);

    return 0;
}

