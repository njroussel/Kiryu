#pragma once

#include <kiryu/integrator.h>

class NormalIntegrator : public Integrator {
    public:
        NormalIntegrator(Accel &accel);

        Color3f Li(Ray3f &ray) override;
};
