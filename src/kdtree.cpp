#include <kiryu/kdtree.h>

void KDTreeNode::initLeafNode(const size_t faceCount_,
        const size_t *const faceIndicesForLeaf,
        std::vector<size_t> *const allFaceIndices) {
    faceIndexOffset = allFaceIndices->size();
    faceCount = (faceCount_ << 2) | 3;

    for (size_t i = 0; i < faceCount_; i++) {
        allFaceIndices->push_back(faceIndicesForLeaf[i]);
    }
}

void KDTreeNode::initTreeNode(const uint8_t axis, const Float axisSplitValue_) {
    axisSplitValue = axisSplitValue_;
    rightChildIndex = axis;
}

size_t KDTreeNode::getRightChildIndex() const {
    return rightChildIndex >> 2;
}

void KDTreeNode::setRightChildIndex(const size_t rightChildIndex_) {
    rightChildIndex = (rightChildIndex_ << 2) | rightChildIndex;
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
    const size_t depth = 0;
    size_t totalFaceCount = 0;
    AABB3f aabb;

    const std::vector<std::reference_wrapper<const Mesh>> &meshes =
        *m_scene.getMeshes();

    for (size_t i = 0; i < meshes.size(); i++) {
        const Mesh &mesh = meshes[i];
        size_t faceCount = mesh.getFaceCount();
        m_cumFaceCountMeshes.push_back(faceCount);
        totalFaceCount += faceCount;

        for (size_t j = 0; j < faceCount; j++) {
            mesh.getAABB(j, aabb);
        }
    }

    m_aabbs.push_back(aabb);

    size_t *faceIndices = new size_t[totalFaceCount];
    for (size_t i = 0; i < totalFaceCount; i++) {
        faceIndices[i] = totalFaceCount;
    }

    buildTree(depth, totalFaceCount, faceIndices);
}

void KDTree::buildTree(size_t depth, size_t faceCount, size_t *faceIndices) {
    KDTreeNode node;

    if (depth >= kMaxTreeDepth || faceCount <= kMinFaceCount) {
        node.initLeafNode(faceCount, faceIndices, &m_allFaceIndices);
        delete[] faceIndices;

        m_nodes.push_back(node);
    } else {
        uint8_t axis = (depth % 3 + 3) % 3; // guarantee pos
        Float axisSplitValue = 0.5;

        node.initTreeNode(axis, axisSplitValue);
        m_nodes.push_back(node);

        const std::vector<std::reference_wrapper<const Mesh>> &meshes =
            *m_scene.getMeshes();

        size_t meshIndex = 0;
        size_t cumFaceCountOffset = 0;

        for (size_t i = 0; i < faceCount; i ++) {
            for (size_t j = meshIndex; j < m_cumFaceCountMeshes.size(); j++) {
                if (faceIndices[i] > m_cumFaceCountMeshes[j]) {
                    meshIndex = j + 1;
                    cumFaceCountOffset = m_cumFaceCountMeshes[j];
                } else {
                    break;
                }
            }

            AABB3f aabb;
            meshes[meshIndex].get().
                getAABB(faceIndices[i] - cumFaceCountOffset, aabb);
        }

        size_t faceCount0 = faceCount;
        size_t faceCount1 = faceCount;
        size_t *faceIndices0 = faceIndices;
        size_t *faceIndices1 = faceIndices;

        buildTree(depth + 1, faceCount0, faceIndices0);
        node.setRightChildIndex(m_nodes.size());
        buildTree(depth + 1, faceCount1, faceIndices1);
    }
}

void KDTree::intersectScene(const Ray3f &ray, Intersection &its) const {
    its.intersection = false;

    Vector3f itsPoint;
    Float minIntersectionDistance = std::numeric_limits<Float>::infinity();
    Float u, v;

    const std::vector<std::reference_wrapper<const Mesh>> &meshes =
        *m_scene.getMeshes();

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
