#pragma once

#include <kiryu/scene.h>

struct Intersection {
    bool intersection = false;
    Mesh *mesh;
    size_t faceIndex;
    Vector3f p;
    Float u;
    Float v;
};

class Accel {
    public:
        Accel(Scene &scene);

        Scene& getScene() { return m_scene; }

        virtual void intersectScene(Ray3f &ray, Intersection &its) = 0;

    protected:
        Scene &m_scene;
};
