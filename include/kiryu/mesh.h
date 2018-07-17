# pragma once

#include <tiny_obj_loader.h>

#include <kiryu/vector.h>

class Mesh {
    public:
        Mesh(const std::vector<tinyobj::index_t> &indices,
                std::vector<Float> &vertices,
                const std::vector<Float> &normals,
                const std::vector<Float> &texCoords,
                const tinyobj::mesh_t &mesh);

        inline int getFaceCount() const { return m_faceCount; }

        void getNormal(const size_t faceIndex,
                const Float u, const Float v, Vector3f &normal) const;

        bool intersectRay(const Ray3f &ray, const size_t faceIndex,
                Vector3f &outIntersectionPoint, Float &u, Float &v) const;
    private:
        const size_t m_faceCount;
        const std::vector<tinyobj::index_t> &m_indices;
        const std::vector<Float> &m_vertices;
        const std::vector<Float> &m_normals;
        const std::vector<Float> &m_texCoords;
};
