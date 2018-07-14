#pragma once

#include <kiryu/integrator.h>

class NormalIntegrator : public Integrator {
    public:
        NormalIntegrator(const Accel &accel);

        Color3f Li(const Ray3f &ray) const override;
};
