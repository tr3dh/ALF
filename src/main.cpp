#include "defines.h"

#include "Mesh/Mesh.h"
#include "UncertaintyCalculation/UncertantyCalculation.h"

int main(int argc, char** argv)
{
    //
    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    IsoMesh isomesh;
    isomesh.loadFromFile("../Import/2DQuadMesh/2DQuadMesh.inp");
    isomesh.loadMaterial();

    if(isomesh.createStiffnessMatrix()){

        isomesh.readBoundaryConditions();

        isomesh.solve();
        isomesh.calculateStrainAndStress();

        isomesh.display(MeshData::VANMISES_STRESS, 0, false, false, {100,100});
    }

    // roadmap unsicherheitsanalyse

    // für zufallsverteilte variable xi
    int Xi = 0.1;

    DataSet advancedData;
    acvanceDataSet(isomesh.getCellData(), advancedData, {1.0f-Xi,1.0f-Xi,1.0f-Xi*Xi});

    NodeSet n0 = isomesh.getUndeformedNodes();
    Eigen::SparseMatrix<float> u_xi = isomesh.getDisplacement() * (1-Xi);

    IsoMesh::displaceNodes(n0, u_xi, n0.begin()->first);

    // Monte Carlo für pdf
    Expression pdf = xi*xi-xi+1;
    rejectionSampling(-pow(xi,2)-xi+1, 0.01, 0.01);

    return 0;
}