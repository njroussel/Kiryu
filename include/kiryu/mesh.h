# pragma once

#include <tiny_obj_loader.h>

#include <kiryu/ray.h>
#include <kiryu/common.h>

class Mesh {
    public:
        Mesh(std::vector<tinyobj::index_t> &indices,
                std::vector<Float> &vertices, std::vector<Float> &normals,
                std::vector<Float> &texCoords, tinyobj::mesh_t &mesh);

        inline int getFaceCount() { return m_faceCount; }

        void getNormal(Vector3f &normal, size_t faceIndex, float u, float v);

        bool findIntersection(Ray3f &ray, size_t faceIndex,
                Vector3f &outIntersectionPoint, Float &u, Float &v);
    private:
        size_t m_faceCount;
        std::vector<tinyobj::index_t> &m_indices;
        std::vector<Float> &m_vertices;
        std::vector<Float> &m_normals;
        std::vector<Float> &m_texCoords;
};
