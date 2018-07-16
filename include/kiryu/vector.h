#pragma once

#include <Eigen/Core>

#include <kiryu/common.h>

template <typename ScalarType_, int Dimension_> struct VectorType :
    public Eigen::Matrix<ScalarType_, Dimension_, 1>
{
    enum {
        Dimension = Dimension_
    };

    typedef ScalarType_ ScalarType;
    typedef Eigen::Matrix<ScalarType, Dimension, 1> EigenVec;

    VectorType(ScalarType x = (ScalarType) 0) { EigenVec::setConstant(x); }

    VectorType(ScalarType x, ScalarType y) : EigenVec(x, y) { }

    VectorType(ScalarType x, ScalarType y, ScalarType z) :
        EigenVec(x, y, z) {
        }

    VectorType(ScalarType x, ScalarType y, ScalarType z, ScalarType w) :
        EigenVec(x, y, z, w) { }

    template <typename Derived> VectorType(const Eigen::MatrixBase<Derived> &v)
        : EigenVec(v) { }


    template <typename Derived> VectorType &operator=(
            const Eigen::MatrixBase<Derived> &v)
    {
        this->EigenVec::operator=(v);
        return *this;
    }
};

typedef VectorType<Float, 3> Vector3f;
typedef VectorType<Float, 3> Color3f;

struct Ray3f {
    Vector3f origin;
    Vector3f direction;
};
