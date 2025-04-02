#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{   
    Mesh mesh;
    mesh.loadFromFile("Job-1.inp");

    return 0;
}

