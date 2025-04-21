#include "defines.h"

#include "MeshLoader/Meshloader.h"
#include "Mesh/Mesh.h"

int main(int argc, char** argv)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    IsoMesh isomesh;
    isomesh.loadFromFile("../Meshes/Job-1.inp");

    isomesh.createStiffnessMatrix();
    isomesh.readBoundaryConditions();

    isomesh.solve();

    isomesh.calculateStrainAndStress();
    isomesh.display(MeshData::VANMISES_STRESS, 0, true, true);

    return 0;
}



