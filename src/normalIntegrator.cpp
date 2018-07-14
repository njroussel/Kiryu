#include <kiryu/normalIntegrator.h>

NormalIntegrator::NormalIntegrator(const Accel &accel) : Integrator(accel) { }

Color3f NormalIntegrator::Li(const Ray3f &ray) const {

    Intersection its;
    m_accel.intersectScene(ray, its);

    if (its.intersection) {
        Vector3f normalAtIts;
        its.mesh->getNormal(its.faceIndex, its.u, its.v, normalAtIts);
        return normalAtIts.cwiseAbs();
    }

    return Color3f(0, 0, 0);
}
