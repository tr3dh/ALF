#include "defines.h"

#include "Mesh/Mesh.h"

int main(int argc, char** argv)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    IsoMesh isomesh;
    isomesh.loadFromFile("../Import/2DTriMesh.inp");

    if(isomesh.createStiffnessMatrix()){

        isomesh.readBoundaryConditions();

        isomesh.solve();
        isomesh.calculateStrainAndStress();

        isomesh.display(MeshData::VANMISES_STRESS, 0, false, false, {100,100});
    }

    return 0;
}