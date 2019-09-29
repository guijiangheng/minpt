#pragma once

#include <Eigen/LU>
#include <minpt/core/ray.h>
#include <minpt/math/matrix.h>

namespace minpt {

class Transform {
public:
  Transform() noexcept
    : m(Eigen::Matrix4f::Identity())
    , mInv(Eigen::Matrix4f::Identity())
  { }

  Transform(const Matrix4f& m) : m(m), mInv(m.inverse())
  { }

  Transform(const Matrix4f& m, const Matrix4f& mInv) : m(m), mInv(mInv)
  { }

  Transform inverse() const {
    return Transform(mInv, m);
  }

  Transform operator*(const Transform& t) const {
    return Transform(m * t.m, t.mInv * mInv);
  }

  Vector3f applyV(const Vector3f& v) const {
    return m.topLeftCorner<3, 3>() * v;
  }

  Vector3f applyN(const Vector3f& n) const {
    return m.topLeftCorner<3, 3>().transpose() * n;
  }

  Vector3f applyP(const Vector3f& p) const {
    auto result = m * Vector4f(p[0], p[1], p[2], 1);
    return result.head(3) / result.w();
  }

  Ray3f operator*(const Ray3f& ray) const {
    return Ray3f(
      applyP(ray.o),
      applyV(ray.d),
      ray.tMax
    );
  }

  std::string toString() const {
    return m.toString();
  }

public:
  Matrix4f m;
  Matrix4f mInv;
};

}
