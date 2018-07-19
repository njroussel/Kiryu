#pragma once

#include <kiryu/accel.h>

class BruteAccel : public Accel {
    public:
        BruteAccel(const Scene &scene);

        void intersectScene(const Ray3f &ray, Intersection &its) const override;
};
