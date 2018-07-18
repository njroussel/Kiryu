#include <iostream>

#include <kiryu/mesh.h>

Mesh::Mesh(const std::vector<tinyobj::index_t> &indices,
        std::vector<Float> &vertices,
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
        const Float u, const Float v, Vector3f &normal) const
{
    const int n0Index = m_indices[faceIndex * 3 + 0].normal_index * 3;
    const int n1Index = m_indices[faceIndex * 3 + 1].normal_index * 3;
    const int n2Index = m_indices[faceIndex * 3 + 2].normal_index * 3;

    const Float *normalsData = m_normals.data();

    const ConstVector3fMap n0(normalsData + n0Index);
    const ConstVector3fMap n1(normalsData + n1Index);
    const ConstVector3fMap n2(normalsData + n2Index);

    normal = ((1 - u - v) * n0 + u * n1 + v * n2).normalized();
}

bool Mesh::intersectRay(const Ray3f &ray, const size_t faceIndex,
        Vector3f &outIntersectionPoint, Float &u, Float &v) const
{
    const int v0Index = m_indices[faceIndex * 3 + 0].vertex_index * 3;
    const int v1Index = m_indices[faceIndex * 3 + 1].vertex_index * 3;
    const int v2Index = m_indices[faceIndex * 3 + 2].vertex_index * 3;

    const Float *verticesData = m_vertices.data();

    const ConstVector3fMap v0(verticesData + v0Index);
    const ConstVector3fMap v1(verticesData + v1Index);
    const ConstVector3fMap v2(verticesData + v2Index);

    const Vector3f edge1 = v1 - v0;
    const Vector3f edge2 = v2 - v0;
    const Vector3f h = ray.direction.cross(edge2);
    const Float a = edge1.dot(h);

    if (a > -KIRYU_EPSILON && a < KIRYU_EPSILON){
        return false;
    }

    const Float inv_a = 1 / a;
    const Vector3f s = ray.origin - v0;
    u = inv_a * (s.dot(h));
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    const Vector3f q = s.cross(edge1);
    v = inv_a * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    const Float t = inv_a * (edge2.dot(q));
    if (t > KIRYU_EPSILON) {
        outIntersectionPoint = ray.origin + ray.direction * t;
        return true;
    }

    return false;
}
