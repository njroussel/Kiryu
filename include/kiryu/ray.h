#pragma once

#include <kiryu/vector.h>

template <typename VectorType_> struct Ray {
    typedef VectorType_ VectorType;
    typedef typename VectorType::ScalarType ScalarType;

    Ray() :
        origin((ScalarType) 0), direction((ScalarType) 0),
        directionCwiseInv((ScalarType) 0), tMin(KIRYU_EPSILON),
        tMax(std::numeric_limits<ScalarType>::infinity()) { }

    Ray(VectorType origin_, VectorType direction_) :
        origin(origin_), direction(direction_),
        directionCwiseInv(direction_.cwiseInverse()), tMin(KIRYU_EPSILON),
        tMax(std::numeric_limits<ScalarType>::infinity()) { }

    const VectorType origin;
    const VectorType direction;
    const VectorType directionCwiseInv;

    const ScalarType tMin;
    const ScalarType tMax;
};

typedef Ray<Vector3f> Ray3f;
