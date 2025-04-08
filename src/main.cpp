#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{   
    std::cout << std::fixed << std::setprecision(4);
    LOG << std::endl;

    Quad4Cell::deriveShapeFunctions();
    
    Mesh mesh;
    mesh.loadFromFile("Job-1.inp");

    mesh.createStiffnesMatrix();

    mesh.applyForces({{11,{{0,2}}}});
    mesh.fixNodes({{1,{0,1}}, {11,{1}}});

    mesh.solve();

    return 0;
}

