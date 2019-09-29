#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <minpt/math/vector.h>

namespace minpt {

using Eigen::MatrixXf;
using MatrixXu = Eigen::Matrix<std::uint32_t, Eigen::Dynamic, Eigen::Dynamic>;

constexpr float Infinity    = std::numeric_limits<float>::infinity();
constexpr float Pi          = 3.14159265358979323846f;
constexpr float InvPi       = 0.31830988618379067154f;
constexpr float Inv2Pi      = 0.15915494309189533577f;
constexpr float Inv4Pi      = 0.07957747154594766788f;
constexpr float PiOver2     = 1.57079632679489661923f;
constexpr float PiOver4     = 0.78539816339744830961f;
constexpr float Sqrt2       = 1.41421356237309504880f;

constexpr float radians(float deg) {
  return Pi / 180 * deg;
}

constexpr float degrees(float rad) {
  return 180 / Pi * rad;
}

template <typename VectorType>
VectorType barycentric(const VectorType& a, const VectorType& b, const VectorType& c, const Vector2f& uv) {
  return (1 - uv.x() - uv.y()) * a + uv.x() * b + uv.y() * c;
}

}
