#include "defines.h"

#include "MeshLoader/Meshloader.h"

int main(int argc, char** argv)
{   
    LOG.precision(10);

    std::vector<NodeIndex> av = {0,1,2,3};
    Quad4Cell a(av);

    Mesh mesh;
    mesh.loadFromFile("Job-1.inp");

    return 0;
}

