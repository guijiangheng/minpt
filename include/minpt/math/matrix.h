#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <minpt/core/ray.h>
#include <minpt/math/math.h>

namespace minpt {

template <typename Scalar>
class Matrix4 : public Eigen::Matrix<Scalar, 4, 4> {
public:
  using Base = Eigen::Matrix<Scalar, 4, 4>;
  using Vector3Type = Vector<Scalar, 3>;
  using Ray3Type = Ray<Vector3Type>;
  using Base::Base;
  using Base::operator*;

  Vector3Type applyV(const Vector3Type& v) const {
    return Base::template topLeftCorner<3, 3>() * v;
  }

  Vector3Type applyN(const Vector3Type& n) const {
    return Base::template topLeftCorner<3, 3>().transpose() * n;
  }

  Vector3Type applyP(const Vector3Type& p) const {
    auto result = Base::operator*(Vector4f(p[0], p[1], p[2], 1));
    return result.head(3) / result.w();
  }

  Ray3Type operator*(const Ray3Type& ray) const {
    return Ray3Type(
      applyP(ray.o),
      applyV(ray.d),
      ray.tMax
    );
  }

  static Matrix4 identity() {
    return Base::Identity();
  }

  static Matrix4 translate(Scalar x, Scalar y, Scalar z) {
    return Eigen::Affine3f(Eigen::Translation3f(x, y, z)).matrix();
  }

  static Matrix4 scale(Scalar x, Scalar y, Scalar z) {
    return Eigen::Affine3f(Eigen::DiagonalMatrix<Scalar, 3>(x, y, z)).matrix();
  }

  static Matrix4 rotateX(Scalar angle) {
    return Eigen::Affine3f(
      Eigen::AngleAxis<Scalar>(
        radians(angle),
        Vector3Type(1, 0, 0))
    ).matrix();
  }

  static Matrix4 rotateY(Scalar angle) {
    return Eigen::Affine3f(
      Eigen::AngleAxis<Scalar>(
        radians(angle),
        Vector3Type(0, 1, 0))
    ).matrix();
  }

  static Matrix4 rotateZ(Scalar angle) {
    return Eigen::Affine3f(
      Eigen::AngleAxis<Scalar>(
        radians(angle),
        Vector3Type(0, 0, 1))
    ).matrix();
  }

  static Matrix4 rotate(const Vector3Type& axis, Scalar angle) {
    return Eigen::Affine3f(
      Eigen::AngleAxis<Scalar>(radians(angle), axis)
    ).matrix();
  }

  static Matrix4 lookAt(const Vector3Type& origin, const Vector3Type& target, const Vector3Type& up) {
    auto z = (origin - target).normalized();
    auto x = up.cross(z).normalized();
    auto y = z.cross(x);
    Eigen::Matrix4f mat;
    mat << x, y, z, origin, 0, 0, 0, 1;
    return mat;
  }

  static Matrix4 perspective(Scalar fov, Scalar near, Scalar far) {
    auto recip = 1 / (far - near);
    auto cot = 1 / std::tan(radians(fov / 2));
    Eigen::Matrix4f mat;
    mat << cot,   0,   0,   0,
             0, cot,   0,   0,
             0,   0,   far * recip, -near * far * recip,
             0,   0,   1,   0;
    return mat;
  }

  std::string toString() const {
    std::ostringstream oss;
    oss << Base::format(Eigen::IOFormat(4, 0, ", ", ",\n", "", "", "[", "]"));
    return oss.str();
  }
};

using Matrix4f = Matrix4<float>;
using Matrix4d = Matrix4<double>;

}
