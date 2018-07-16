#pragma once

#include <Eigen/Core>

#include <kiryu/common.h>

template <typename _ScalarType, int _Dimension> struct VectorType :
    public Eigen::Matrix<_ScalarType, _Dimension, 1>
{
    enum {
        Dimension = _Dimension
    };

    typedef _ScalarType ScalarType;
    typedef Eigen::Matrix<ScalarType, Dimension, 1> EigenVec;

    VectorType(ScalarType x = (ScalarType) 0) { EigenVec::setConstant(x); }

    VectorType(ScalarType x, ScalarType y) : EigenVec(x, y) { }

    VectorType(ScalarType x, ScalarType y, ScalarType z) :
        EigenVec(x, y, z) {
        }

    VectorType(ScalarType x, ScalarType y, ScalarType z, ScalarType w) :
        EigenVec(x, y, z, w) { }

    template <typename Derived> VectorType(const Eigen::MatrixBase<Derived>& p)
        : EigenVec(p) { }


    template <typename Derived> VectorType &operator=(
            const Eigen::MatrixBase<Derived>& p)
    {
        this->EigenVec::operator=(p);
        return *this;
    }

    template <typename Derived> VectorType(const Derived *dataPtr)
        : EigenVec(dataPtr) { }
};

typedef VectorType<Float, 3> Vector3f;
typedef VectorType<Float, 3> Color3f;

struct Ray3f {
    Vector3f origin;
    Vector3f direction;
};
