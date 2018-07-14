#pragma once

#include <kiryu/vector.h>
#include <kiryu/ray.h>
#include <kiryu/accel.h>

class Integrator {
    public:
        Integrator(const Accel &accel);

        virtual Color3f Li(const Ray3f &ray) const = 0;

    protected:
        const Accel &m_accel;
};
