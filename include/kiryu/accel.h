#pragma once

#include <kiryu/scene.h>

struct Intersection {
    bool intersection = false;
    const Mesh *mesh;
    size_t faceIndex;
    Vector3f p;
    Float u;
    Float v;
};

class Accel {
    public:
        Accel(const Scene &scene);

        const Scene& getScene() const { return m_scene; }

        virtual void intersectScene(const Ray3f &ray, Intersection &its) const = 0;

    protected:
        const Scene &m_scene;
};
