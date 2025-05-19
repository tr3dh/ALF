#include "Mesh.h"

void IsoMesh::setSelfPointer(IsoMesh* ptr){

    g_self = ptr;
}

IsoMesh* IsoMesh::getSelfPointer(){
    
    return g_self;
}