#pragma once

#include <kiryu/mesh.h>

class Scene {
    public:
        Scene();

        void addMesh(const Mesh &mesh);

        inline const std::vector<Mesh>* getMeshes() const {
            return &m_meshes;
        }

    private:
        std::vector<Mesh> m_meshes;
};
