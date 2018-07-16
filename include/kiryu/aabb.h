#pragma once

/**
 * Lightweight version of Eigen's AlignedBox specifically written for Kiryu
 */
/*
template <typename _Scalar, int _Dimension> struct AABB {
    enum {
        Dimension = _Dimension
    };

    typedef _Scalar Scalar;
    typedef Eigen::Matrix<Scalar, Dimension, 1> TVector;

    AABB(TVector &point) : min(point), max(point) { }

    AABB(TVector &min_, TVector &max_) : min(min_), max(max_) { }

    void expand(TVector &point) {
        min = min.cwiseMin(point);
        max = max.cwiseMax(point);
    }

    bool overlaps(TVector &point, bool strict = false) {
        return false;
    }

    Eigen::Matrix<Scalar, _Dimension, 1> min;
    Eigen::Matrix<Scalar, _Dimension, 1> max;
};
*/
