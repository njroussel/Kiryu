#pragma once

#include <kiryu/scene.h>

class QBVH {
    public:
        QBVH(Scene &scene);

    private:
        Scene &m_scene;
};
