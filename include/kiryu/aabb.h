#pragma once

#include <kiryu/ray.h>
#include <iostream>

template <typename VectorType_> struct AABB {
    enum {
        Dimension = VectorType_::Dimension
    };

    typedef VectorType_ VectorType;
    typedef typename VectorType::ScalarType ScalarType;

    AABB() :
        min(std::numeric_limits<ScalarType>::infinity()),
        max(-std::numeric_limits<ScalarType>::infinity()) { }

    template <typename Derived> AABB(const Derived &point) :
        min(point), max(point) { }

    template <typename Derived, typename OtherDerived> AABB(Derived &min_,
            OtherDerived &max_) : min(min_), max(max_) { }

    template <typename Derived> void expand(const Derived &point) {
        min = min.cwiseMin(point);
        max = max.cwiseMax(point);
    }

    void expand(const AABB<VectorType> &aabb) {
        min = min.cwiseMin(aabb.min);
        max = max.cwiseMax(aabb.max);
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

    template <typename Derived> bool intersectRay(
            const Ray<Derived> &ray) const
    {
        ScalarType tMin = -std::numeric_limits<ScalarType>::infinity();
        ScalarType tMax = std::numeric_limits<ScalarType>::infinity();

        for (size_t i = 0; i < Dimension; i++) {
            if (ray.direction(i) == (ScalarType) 0) {
                if (ray.origin(i) < min(i) || max(i) < ray.origin(i)) {
                    return false;
                }
            }

            ScalarType tMinAxis = (min(i) - ray.origin(i)) *
                ray.directionCwiseInv(i);
            ScalarType tMaxAxis = (max(i) - ray.origin(i)) *
                ray.directionCwiseInv(i);

            if (tMinAxis > tMaxAxis) {
                std::swap(tMinAxis, tMaxAxis);
            }

            tMin = std::max(tMin, tMinAxis);
            tMax = std::min(tMax, tMaxAxis);

            if (tMin > tMax) {
                return false;
            }
        }

        return ray.tMin <= tMax && ray.tMax >= tMin;
    }

    VectorType min;
    VectorType max;
};

typedef AABB<Vector3f> AABB3f;
