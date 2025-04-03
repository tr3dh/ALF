#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{   
    std::cout << std::fixed << std::setprecision(4);

    Quad4Cell::deriveShapeFunctions();
    
    Mesh mesh;
    mesh.loadFromFile("Job-1.inp");

    mesh.createStiffnesMatrix();

    return 0;
}

