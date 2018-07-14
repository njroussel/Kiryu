#include <kiryu/normalIntegrator.h>

NormalIntegrator::NormalIntegrator(Accel &accel) : Integrator(accel) { }

Color3f NormalIntegrator::Li(Ray3f &ray) {

    Intersection its;
    m_accel.intersectScene(ray, its);

    if (its.intersection) {
        Vector3f normalAtIts;
        its.mesh->getNormal(normalAtIts, its.faceIndex, its.u, its.v);
        return normalAtIts.cwiseAbs();
    }

    return Color3f(0, 0, 0);
}
