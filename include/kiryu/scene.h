#pragma once

#include<kiryu/mesh.h>

class Scene {
    public:
        Scene();

        void addMesh(Mesh &mesh);

        inline std::vector<std::reference_wrapper<Mesh>> getMeshes() {
            return m_meshes;
        }

    private:
        std::vector<std::reference_wrapper<Mesh>> m_meshes;

};
