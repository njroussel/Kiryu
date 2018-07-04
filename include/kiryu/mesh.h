# pragma once

#include <tiny_obj_loader.h>

#include <kiryu/common.h>

class Mesh {
    public:
        Mesh(uint32_t faceCount, tinyobj::index_t *indices, Float *vertices,
                Float *normals, Float *texCoords);

    private:
        //tinyobj::mesh_t &m_mesh;
};
