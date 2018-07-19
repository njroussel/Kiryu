#pragma once 

#include <vector>

#include <kiryu/vector.h>
#include <kiryu/accel.h>

/**
 * NOTE: This class was written after reading through pbrt-v3's implementation
 * of kd-trees. Even though this was written from scratch, it might still
 * be very similar to their implementation. Therefore their copyright notice
 * most likely still applies here, here it is:
 *
 *    pbrt source code is Copyright(c) 1998-2016
 *                       Matt Pharr, Greg Humphreys, and Wenzel Jakob.
 *
 *  This file is part of pbrt.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

struct KDTreeNode {
    union {
        /**
         * Used for tree nodes.
         *
         * The value of the split along the axis defined in the two lower bits
         * of KDTreeNode::rightChildIndex.
         */
        float axisSplitValue; 

        /**
         * Used for leaf nodes.
         *
         * The index offset of the first face that overlaps this leaf node.
         * All other indices for the KDTreeNode::faceCount - 1 faces for this
         * leaf node should follow this offset.
         */
        size_t faceIndexOffset;
    };

    union {
        /**
         * Used for tree nodes.
         *
         * The index to the "right" child of this tree node.
         * The "left" child is stored at the index following this node.
         * The two lower bits contain the axis of the split (x: 0, y: 1, z: 2).
         */
        size_t rightChildIndex; 

        /**
         * Used for leaf nodes.
         *
         * The number of faces overlapping this leaf node.
         * The two lower bits contain the value 3, to indicate that this is a
         * leaf node and not a tree node.
         */
        size_t faceCount;
    };

    size_t getRightChildIndex() const; 

    size_t getFaceCount() const;

    uint8_t getSplitAxis() const;

    bool isLeafNode() const;

    void initLeafNode(const size_t faceCount_,
            const int *const  faceIndicesForLeaf,
            std::vector<int> *const allFaceIndices);

    void initTreeNode(const uint8_t axis, const float axisSplitValue_,
            const size_t rightChildIndex_);

};

class KDTree : public Accel {
    public:
        static const size_t kMaxTreeDepth = 0;
        static const size_t kMinFaceCount = 10;

        KDTree(const Scene &scene);

        void intersectScene(const Ray3f &ray, Intersection &its) const override;

    private:
        void buildTree(size_t depth, size_t faceCount);

        AABB3f m_aabb;
        std::vector<KDTreeNode> m_nodes;
        std::vector<AABB3f> m_aabbs;
};
