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
            // Trick/Hack - actuall expands aabb for face j
            mesh.getAABB(j, aabb);
        }
    }

    m_aabbs.push_back(aabb);

    size_t *faceIndices = new size_t[totalFaceCount];
    for (size_t i = 0; i < totalFaceCount; i++) {
        faceIndices[i] = i;
    }

    buildTree(depth, totalFaceCount, faceIndices, 0);
}

void KDTree::buildTree(size_t depth, size_t faceCount, size_t *faceIndices,
        size_t nodeIndex)
{
    KDTreeNode node;

    if (depth >= kMaxTreeDepth || faceCount <= kMinFaceCount) {
        node.initLeafNode(faceCount, faceIndices, &m_allFaceIndices);
        delete[] faceIndices;

        m_nodes.push_back(node);
    } else {
        uint8_t axis = (depth % 3 + 3) % 3; // guarantee pos
        Float axisSplitValueRatio = 0.5;

        const AABB3f currentNodeAABB = m_aabbs[nodeIndex];
        Float axisSplitValue = currentNodeAABB.min(axis) +
            axisSplitValueRatio *
            (currentNodeAABB.max(axis) - currentNodeAABB.min(axis));


        node.initTreeNode(axis, axisSplitValue);
        m_nodes.push_back(node);

        const std::vector<std::reference_wrapper<const Mesh>> &meshes =
            *m_scene.getMeshes();

        size_t meshIndex = 0;
        size_t cumFaceCountOffset = 0;
        size_t faceCount0 = 0;
        size_t faceCount1 = 0;
        size_t *faceIndices0 =  new size_t[faceCount];
        size_t *faceIndices1 =  new size_t[faceCount];

        for (size_t i = 0; i < faceCount; i ++) {
            for (size_t j = meshIndex; j < m_cumFaceCountMeshes.size(); j++) {
                if (faceIndices[i] >= m_cumFaceCountMeshes[j]) {
                    meshIndex = j + 1;
                    cumFaceCountOffset = m_cumFaceCountMeshes[j];
                } else {
                    break;
                }
            }

            AABB3f aabb;
            meshes[meshIndex].get().
                getAABB(faceIndices[i] - cumFaceCountOffset, aabb);


            Float fMin = aabb.min(axis);
            Float fMax = aabb.max(axis);

            if (fMin <= axisSplitValue) {
                faceIndices0[faceCount0++] = faceIndices[i];
            }
            if (fMax >= axisSplitValue) {
                faceIndices1[faceCount1++] = faceIndices[i];
            }
        }

        delete[] faceIndices;

        Vector3f leftChildMax = currentNodeAABB.max;
        leftChildMax(axis) = axisSplitValue;
        m_aabbs.push_back(AABB3f(currentNodeAABB.min, leftChildMax));

        buildTree(depth + 1, faceCount0, faceIndices0, nodeIndex + 1);

        m_nodes[nodeIndex].setRightChildIndex(m_nodes.size());

        Vector3f rightChildMax = currentNodeAABB.min;
        rightChildMax(axis) = axisSplitValue;
        m_aabbs.push_back(AABB3f(rightChildMax, currentNodeAABB.max));
        buildTree(depth + 1, faceCount1, faceIndices1, m_nodes.size());
    }
}

void KDTree::intersectScene(const Ray3f &ray, Intersection &its) const {
    its.intersection = false;

    Vector3f itsPoint;
    Float minIntersectionDistance = std::numeric_limits<Float>::infinity();
    Float u, v;

    size_t nodeIndex = 0;
    recurseTraverse(nodeIndex, ray, its, itsPoint, minIntersectionDistance, u, v);
}

void KDTree::recurseTraverse(size_t nodeIndex, const Ray3f &ray,
        Intersection &its, Vector3f &itsPoint, Float &minIntersectionDistance,
        Float &u, Float &v) const
{
    const std::vector<std::reference_wrapper<const Mesh>> &meshes =
        *m_scene.getMeshes();

    if (m_nodes[nodeIndex].isLeafNode() &&
            m_nodes[nodeIndex].getFaceCount() == 0) {
        return;
    }

    if (m_aabbs[nodeIndex].intersectRay(ray)) {
        if (m_nodes[nodeIndex].isLeafNode()) {
            size_t faceIndexOffset = m_nodes[nodeIndex].faceIndexOffset;
            size_t faceCount = m_nodes[nodeIndex].getFaceCount();

            size_t meshIndex = 0;
            size_t cumFaceCountOffset = 0;
            for (size_t f = faceIndexOffset; f < faceIndexOffset + faceCount; f++) {
                for (size_t j = meshIndex; j < m_cumFaceCountMeshes.size(); j++) {
                    if (m_allFaceIndices[f] >= m_cumFaceCountMeshes[j]) {
                        meshIndex = j + 1;
                        cumFaceCountOffset = m_cumFaceCountMeshes[j];
                    } else {
                        break;
                    }
                }

                bool intersection = meshes[meshIndex].get().intersectRay(ray, 
                        m_allFaceIndices[f] - cumFaceCountOffset,
                        itsPoint, u, v);

                if (intersection) {
                    its.intersection = true;
                    Float distance = (itsPoint - ray.origin).norm();

                    if (distance < minIntersectionDistance) {
                        minIntersectionDistance = distance;
                        its.p = itsPoint;
                        its.u = u;
                        its.v = v;
                        its.mesh = &(meshes[meshIndex].get());
                        its.faceIndex = m_allFaceIndices[f] - cumFaceCountOffset;
                    }
                }
            }
        } else {
            recurseTraverse(nodeIndex + 1, ray, its, itsPoint,
                    minIntersectionDistance, u, v);

            recurseTraverse(m_nodes[nodeIndex].getRightChildIndex(), ray, its,
                    itsPoint, minIntersectionDistance, u, v);
        }
    }

}
