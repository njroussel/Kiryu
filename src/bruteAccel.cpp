#include <kiryu/bruteAccel.h>

BruteAccel::BruteAccel(Scene &scene) : Accel(scene) { }

void BruteAccel::intersectScene(Ray3f &ray, Intersection &its) {
    its.intersection = false;

    Vector3f itsPoint;
    Float minIntersectionDistance = std::numeric_limits<float>::infinity();
    Float u, v;

    std::vector<std::reference_wrapper<Mesh>> meshes = m_scene.getMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        Mesh &mesh = meshes[i];

        for (size_t f = 0; f < mesh.getFaceCount(); f++) {
            bool intersection = mesh.intersectRay(ray, f,
                    itsPoint, u, v);

            if (intersection) {
                its.intersection = true;
                Float distance = (itsPoint - ray.origin).norm();

                if (distance < minIntersectionDistance) {
                    minIntersectionDistance = distance;
                    its.p = itsPoint;
                    its.u = u;
                    its.v = v;
                    its.mesh = &mesh;
                    its.faceIndex = f;
                }
            }
        }
    }
}
