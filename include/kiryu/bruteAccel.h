#pragma once

#include <kiryu/accel.h>

class BruteAccel : public Accel {
    public:
        BruteAccel(Scene &scene);

        void intersectScene(Ray3f &ray, Intersection &its);
};
