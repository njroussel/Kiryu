#pragma once

#include <kiryu/vector.h>
#include <iostream>

template <typename VectorType_> struct AABB {

    typedef VectorType_ VectorType;
    typedef typename VectorType::ScalarType ScalarType;

    AABB() :
        min(std::numeric_limits<ScalarType>::infinity()),
        max(-std::numeric_limits<ScalarType>::infinity()) { }

    template <typename Derived> AABB(const Derived &point) :
        min(point), max(point) { }

    template <typename Derived, typename OtherDerived> AABB(Derived &min_,
            OtherDerived &max_) : min(min_), max(max_) { }

    template <typename Derived> void expand(Derived &point) {
        std::cout << "MEME" << std::endl;
        point(0) = 0;
        min = min.cwiseMin(point);
        max = max.cwiseMax(point);
    }

    template <typename Derived> void expand(const Derived &point) {
        min = min.cwiseMin(point);
        max = max.cwiseMax(point);
    }

    template <typename Derived> bool overlaps(const Derived &point,
            bool strict = false) const
    {
        if (strict) {
            return (min.array() < point.array()).all() &&
                (point.array() < max.array()).all();
        }

        return (min.array() <= point.array()).all() &&
            (point.array() <= max.array()).all();
    }

    VectorType min;
    VectorType max;
};

typedef AABB<Vector3f> AABB3f;
