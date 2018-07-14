#pragma once

#include <kiryu/vector.h>
#include <kiryu/ray.h>
#include <kiryu/accel.h>

class Integrator {
    public:
        Integrator(Accel &accel);

        virtual Color3f Li(Ray3f &ray) = 0;

    protected:
        Accel &m_accel;
};
