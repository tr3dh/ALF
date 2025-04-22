#include "defines.h"

#include "MeshLoader/Meshloader.h"
#include "Mesh/Mesh.h"

int main(int argc, char** argv)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    IsoMesh isomesh;
    isomesh.loadFromFile("../Meshes/2DQuadMesh.inp");

    isomesh.createStiffnessMatrix();

    isomesh.readBoundaryConditions();

    isomesh.solve();

    isomesh.calculateStrainAndStress();
    isomesh.display(MeshData::VANMISES_STRESS, 0, false, false, {-1200,-800},3500);

    return 0;
}



