#include <kiryu/kdtree.h>

void KDTreeNode::initLeafNode(const size_t faceCount_,
        const int *const  faceIndicesForLeaf,
        std::vector<int> *const allFaceIndices) {
    const size_t tmp = allFaceIndices->size();
    faceCount = (faceCount_ << 2) | 3;

    for (size_t i = 0; i < faceCount_; i++) {
        allFaceIndices->push_back(faceIndicesForLeaf[i]);
    }
}

void KDTreeNode::initTreeNode(const uint8_t axis, const Float axisSplitValue_, 
        const size_t rightChildIndex_) {
    axisSplitValue = axisSplitValue_;
    rightChildIndex = (rightChildIndex_ << 2) | axis;
}

size_t KDTreeNode::getRightChildIndex() const {
    return rightChildIndex >> 2;
}

size_t KDTreeNode::getFaceCount() const {
    return rightChildIndex >> 2;
}

uint8_t KDTreeNode::getSplitAxis() const {
    return rightChildIndex & 3;
}

bool KDTreeNode::isLeafNode() const {
    return (faceCount & 3) == 3;
}

KDTree::KDTree(const Scene &scene) : Accel(scene) {

    std::vector<std::reference_wrapper<const Mesh>> meshes = m_scene.getMeshes();

    for (size_t i = 0; i < meshes.size(); i++) {
        const Mesh &mesh = meshes[i];

        const std::vector<Float> &vertices = mesh.getVertices();
        const size_t vertexCount = vertices.size();

        for (int i = 0; i < vertexCount / 3; i++) {
            const ConstVector3fMap vertex(vertices.data() + i);
            m_aabb.expand(vertex);
        }
    }
}

void KDTree::intersectScene(const Ray3f &ray, Intersection &its) const {
    its.intersection = false;

    if (!m_aabb.intersectRay(ray)) {
        return;
    }

    Vector3f itsPoint;
    Float minIntersectionDistance = std::numeric_limits<Float>::infinity();
    Float u, v;

    std::vector<std::reference_wrapper<const Mesh>> meshes = m_scene.getMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        const Mesh &mesh = meshes[i];

        for (size_t f = 0; f < mesh.getFaceCount(); f++) {
            bool intersection = mesh.intersectRay(ray, f,
                    itsPoint, u, v);

            if (intersection) {
                its.intersection = true;
                Float distance = (itsPoint - ray.origin).norm();

                if (distance < minIntersectionDistance) {
                    minIntersectionDistance = distance;
                    its.p = itsPoint;
                    its.u = u;
                    its.v = v;
                    its.mesh = &mesh;
                    its.faceIndex = f;
                }
            }
        }
    }
}
