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

    mesh.applyForces({{22,{{0,502.7}, {1,252.7}}}, {33,{{0,500},{1,250}}}});
    mesh.fixNodes({{1,{0,1}}, {11,{1}}});

    return 0;
}

