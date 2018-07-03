#pragma once

#include <Eigen/Core>

#include <kiryu/common.h>

typedef Eigen::Matrix<uint16_t, 2, 1> Vector2h;
typedef Eigen::Matrix<uint16_t, 3, 1> Vector3h;

typedef Eigen::Matrix<int32_t, 2, 1> Vector2i;
typedef Eigen::Matrix<int32_t, 3, 1> Vector3i;

typedef Eigen::Matrix<Float, 2, 1> Vector2f;
typedef Eigen::Matrix<Float, 3, 1> Vector3f;
typedef Eigen::Matrix<Float, 4, 1> Vector4f;

typedef Eigen::Matrix<Float, 4, 4> Matrix4f;
