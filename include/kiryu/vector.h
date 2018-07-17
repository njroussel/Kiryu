#pragma once

#include <Eigen/Dense>

#include <kiryu/common.h>

template <typename ScalarType_, int Dimension_> struct VectorType :
    public Eigen::Matrix<ScalarType_, Dimension_, 1>
{
    enum {
        Dimension = Dimension_
    };

    typedef ScalarType_ ScalarType;
    typedef Eigen::Matrix<ScalarType, Dimension, 1> BaseType;

    VectorType(ScalarType x = (ScalarType) 0) { BaseType::setConstant(x); }

    VectorType(ScalarType x, ScalarType y) : BaseType(x, y) { }

    VectorType(ScalarType x, ScalarType y, ScalarType z) :
        BaseType(x, y, z) {
        }

    VectorType(ScalarType x, ScalarType y, ScalarType z, ScalarType w) :
        BaseType(x, y, z, w) { }

    template <typename Derived> VectorType(const Eigen::MatrixBase<Derived> &v)
        : BaseType(v) { }

    template <typename Derived> VectorType &operator=(
            const Eigen::MatrixBase<Derived> &v)
    {
        this->BaseType::operator=(v);
        return *this;
    }

    template <typename Derived> VectorType (const Derived *data)
        : BaseType(data) { }
};


template <typename VecType_> struct VectorTypeMap :
    public Eigen::Map<VecType_>
{
    typedef VecType_ VecType;
    typedef typename VecType::Scalar ScalarType;
    typedef Eigen::Map<VecType> EigenVecMap;

    VectorTypeMap(ScalarType *dataPtr) : EigenVecMap(dataPtr) { }

    VectorTypeMap(const ScalarType *dataPtr) : EigenVecMap(dataPtr) { }
};

typedef VectorType<Float, 3> Vector3f;
typedef VectorType<Float, 3> Color3f;

// TODO: Something nicer than this hack... Passing just Vector3f as a template
// argument fails as Eigen::Map will have a derived class as template argument
// and not the base Eigen::Matrix which it derives
typedef VectorTypeMap<const Vector3f> ConstVector3fMap;
typedef VectorTypeMap<Vector3f::BaseType> Vector3fMap;

typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic> MatrixXf;

struct Ray3f {
    Vector3f origin;
    Vector3f direction;
};
