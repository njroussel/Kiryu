#include <Eigen/Dense>
#include <iostream>

#include <kiryu/mesh.h>
#include <kiryu/vector.h>

Mesh::Mesh(const std::vector<tinyobj::index_t> &indices,
        const std::vector<Float> &vertices,
        const std::vector<Float> &normals,
        const std::vector<Float> &texCoords,
        const tinyobj::mesh_t &mesh) :
    m_faceCount(mesh.num_face_vertices.size()), m_indices(indices),
    m_vertices(vertices), m_normals(normals), m_texCoords(texCoords)
{
    for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
        if (mesh.num_face_vertices[f] != 3) {
            throw std::invalid_argument("Mesh contains a face which does "
                    "not have exactly 3 vertices!");
        }
    }
}

void Mesh::getNormal(const size_t faceIndex,
        const float u, const float v, Vector3f &normal) const
{
    int n0Index = m_indices[faceIndex * 3 + 0].normal_index * 3;
    int n1Index = m_indices[faceIndex * 3 + 1].normal_index * 3;
    int n2Index = m_indices[faceIndex * 3 + 2].normal_index * 3;

    const float *normalsData = m_normals.data();
    Eigen::Map<const Vector3f> n0(normalsData + n0Index);
    Eigen::Map<const Vector3f> n1(normalsData + n1Index);
    Eigen::Map<const Vector3f> n2(normalsData + n2Index);

    normal = ((1 - u - v) * n0 + u * n1 + v * n2).normalized();
}

bool Mesh::intersectRay(const Ray3f &ray, const size_t faceIndex,
        Vector3f &outIntersectionPoint, Float &u, Float &v) const
{
    int v0Index = m_indices[faceIndex * 3 + 0].vertex_index * 3;
    int v1Index = m_indices[faceIndex * 3 + 1].vertex_index * 3;
    int v2Index = m_indices[faceIndex * 3 + 2].vertex_index * 3;

    const float *verticesData = m_vertices.data();
    Eigen::Map<const Vector3f> v0(verticesData + v0Index);
    Eigen::Map<const Vector3f> v1(verticesData + v1Index);
    Eigen::Map<const Vector3f> v2(verticesData + v2Index);

    Vector3f edge1 = v1 - v0;
    Vector3f edge2 = v2 - v0;
    Vector3f h = ray.direction.cross(edge2);
    Float a = edge1.dot(h);
    if (a > -KIRYU_EPSILON && a < KIRYU_EPSILON){
        return false;
    }

    Float inv_a = 1 / a;
    Vector3f s = ray.origin - v0;
    u = inv_a * (s.dot(h));
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    Vector3f q = s.cross(edge1);
    v = inv_a * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    Float t = inv_a * (edge2.dot(q));
    if (t > KIRYU_EPSILON) {
        outIntersectionPoint = ray.origin + ray.direction * t;
        return true;
    }

    return false;
}
