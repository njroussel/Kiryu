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
        BaseType(x, y, z) { }

    VectorT(ScalarType x, ScalarType y, ScalarType z, ScalarType w) :
        BaseType(x, y, z, w) { }

    VectorT(const ScalarType *dataPtr) : BaseType(dataPtr) { }

    template <typename Derived> VectorT(const Eigen::MatrixBase<Derived> &v)
        : BaseType(v) { }

    template <typename Derived> VectorT &operator=(
            const Eigen::MatrixBase<Derived> &v)
    {
        this->BaseType::operator=(v);
        return *this;
    }
};


// TODO: There most likely is a much nicer way of doing this
template <typename VectorType_> struct VectorTMap :
    public Eigen::Map<typename std::conditional<std::is_const<VectorType_>::value,
    const typename VectorType_::BaseType,
    typename VectorType_::BaseType>::type>
{
    typedef VectorType_ VectorType;
    typedef typename VectorType::Scalar ScalarType;
    typedef typename VectorType::BaseType VectorBaseType;
    typedef Eigen::Map<typename std::conditional<std::is_const<VectorType>::value,
            const VectorBaseType, VectorBaseType>::type> BaseType;

    VectorTMap(ScalarType *dataPtr) : BaseType(dataPtr) { }

    VectorTMap(const ScalarType *dataPtr) : BaseType(dataPtr) { }
};

typedef VectorT<Float, 3> Vector3f;
typedef VectorT<Float, 3> Color3f;

typedef VectorTMap<const Vector3f> ConstVector3fMap;
typedef VectorTMap<Vector3f> Vector3fMap;

typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic> MatrixXf;

struct Ray3f {
    Vector3f origin;
    Vector3f direction;
};
