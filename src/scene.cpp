#include <kiryu/scene.h>

Scene::Scene() {}

void Scene::addMesh(Mesh &mesh) {
    m_meshes.push_back(mesh);
}
