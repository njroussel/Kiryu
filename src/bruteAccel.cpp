#include <kiryu/bruteAccel.h>

BruteAccel::BruteAccel(Scene &scene) : m_scene(scene) { }

void BruteAccel::intersectScene(Ray3f &ray, bool &intersection,
        Vector3f &intersectionPoint) {
    intersection = false;

    Float closestU, closestV;
    size_t closestFaceIndex;

    Vector3f tmpItsPoint;
    Float minIntersectionDistance = std::numeric_limits<float>::infinity();
    Float tmpU, tmpV;

    std::vector<std::reference_wrapper<Mesh>> meshes = m_scene.getMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        Mesh &mesh = meshes[i];

        for (size_t f = 0; f < mesh.getFaceCount(); f++) {
            Vector3f vertices[3];

            bool tmpIntersection = mesh.findIntersection(ray, f,
                    intersectionPoint, tmpU, tmpV);

            if (tmpIntersection) {
                Float distance = (intersectionPoint - ray.origin).norm();
                if (distance < minIntersectionDistance) {
                    minIntersectionDistance = distance;
                    intersectionPoint = tmpItsPoint;
                    closestU = tmpU;
                    closestV = tmpV;
                    closestFaceIndex = f;
                }
            }

            intersection |= tmpIntersection;
        }
    }
}
