#include <kiryu/scene.h>

Scene::Scene() {}

void Scene::addMesh(const Mesh &mesh) {
    m_meshes.push_back(mesh);
}
