# pragma once

#include <tiny_obj_loader.h>

#include <kiryu/ray.h>
#include <kiryu/aabb.h>

class Mesh {
    public:
        Mesh(const std::vector<tinyobj::index_t> &indices,
                std::vector<Float> &vertices,
                const std::vector<Float> &normals,
                const std::vector<Float> &texCoords,
                const tinyobj::mesh_t &mesh);

        inline const size_t getFaceCount() const { return m_faceCount; }

        inline const std::vector<Float>& getVertices() const {
            return m_vertices;
        }

        void getNormal(const size_t faceIndex,
                const Float u, const Float v, Vector3f &normal) const;

        void getAABB(const size_t faceIndex, AABB3f &aabb) const;

        //TODO: Make this take an Intersection struct
        bool intersectRay(const Ray3f &ray, const size_t faceIndex,
                Vector3f &outIntersectionPoint, Float &u, Float &v) const;

    private:
        const size_t m_faceCount;
        const std::vector<tinyobj::index_t> m_indices;
        const std::vector<Float> m_vertices;
        const std::vector<Float> m_normals;
        const std::vector<Float> m_texCoords;
};
