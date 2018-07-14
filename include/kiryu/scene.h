#pragma once

#include <kiryu/mesh.h>

class Scene {
    public:
        Scene();

        void addMesh(const Mesh &mesh);

        inline std::vector<std::reference_wrapper<const Mesh>> getMeshes() const {
            return m_meshes;
        }

    private:
        std::vector<std::reference_wrapper<const Mesh>> m_meshes;
};
