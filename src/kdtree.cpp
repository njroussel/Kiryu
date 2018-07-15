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

KDTree::KDTree() { }


