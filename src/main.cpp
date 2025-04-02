#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{   
    Quad4Cell::deriveShapeFunctions();
    
    Mesh mesh;
    mesh.loadFromFile("Job-1.inp");

    mesh.createStiffnesMatrix();

    return 0;
}

