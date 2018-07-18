#pragma once

#include <Eigen/Dense>

#include <kiryu/common.h>

template <typename ScalarType_, int Dimension_> struct VectorT:
    public Eigen::Matrix<ScalarType_, Dimension_, 1>
{
    enum {
        Dimension = Dimension_
    };

    typedef ScalarType_ ScalarType;
    typedef Eigen::Matrix<ScalarType, Dimension, 1> BaseType;

    VectorT(ScalarType x = (ScalarType) 0) { BaseType::setConstant(x); }

    VectorT(ScalarType x, ScalarType y) : BaseType(x, y) { }

    VectorT(ScalarType x, ScalarType y, ScalarType z) :
        BaseType(x, y, z) {
        }

    VectorT(ScalarType x, ScalarType y, ScalarType z, ScalarType w) :
        BaseType(x, y, z, w) { }

    template <typename Derived> VectorT(const Eigen::MatrixBase<Derived> &v)
        : BaseType(v) { }

    template <typename Derived> VectorT &operator=(
            const Eigen::MatrixBase<Derived> &v)
    {
        this->BaseType::operator=(v);
        return *this;
    }
};


template <typename VectorType_> struct VectorTMap :
    public Eigen::Map<const Eigen::Matrix<typename VectorType_::ScalarType,
    VectorType_::Dimension, 1>>
{
    typedef VectorType_ VectorType;
    typedef typename VectorType::Scalar ScalarType;
    typedef typename VectorType::BaseType VectorBaseType;
    typedef Eigen::Map<const VectorBaseType> BaseType;

    VectorTMap(ScalarType *dataPtr) : BaseType(dataPtr) { }

    VectorTMap(const ScalarType *dataPtr) : BaseType(dataPtr) { }
};

typedef VectorT<Float, 3> Vector3f;
typedef VectorT<Float, 3> Color3f;

typedef VectorTMap<const Vector3f> Vector3fMap;
typedef VectorTMap<const Vector3f> Vector3fMap;

typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic> MatrixXf;

struct Ray3f {
    Vector3f origin;
    Vector3f direction;
};
