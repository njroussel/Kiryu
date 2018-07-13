#pragma once

#include <kiryu/scene.h>

class BruteAccel {
    public:
        BruteAccel(Scene &scene);

        void intersectScene(Ray3f &ray, bool &intersection,
                Vector3f &intersectionPoint);

    private:
        Scene &m_scene;
};
