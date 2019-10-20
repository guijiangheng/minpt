#pragma once

#include <cstring>
#include <Eigen/Core>
#include <minpt/core/ray.h>
#include <minpt/math/math.h>

namespace minpt {

class Matrix4f {
public:
  Matrix4f() noexcept = default;

  Matrix4f(const float m[4][4]) noexcept {
    std::memcpy(e, m, 16 * sizeof(float));
  }

  Matrix4f(
      float m00, float m01, float m02, float m03,
      float m10, float m11, float m12, float m13,
      float m20, float m21, float m22, float m23,
      float m30, float m31, float m32, float m33) noexcept : e {
    m00, m01, m02, m03,
    m10, m11, m12, m13,
    m20, m21, m22, m23,
    m30, m31, m32, m33
  } { }

  Matrix4f operator*(const Matrix4f& m) const {
    float r[4][4];
    for (auto i = 0; i < 4; ++i)
      for (auto j = 0; j < 4; ++j)
        r[i][j] = e[i][0] * m.e[0][j] + e[i][1] * m.e[1][j] +
                  e[i][2] * m.e[2][j] + e[i][3] * m.e[3][j];
    return Matrix4f(r);
  }

  Matrix4f& operator*=(const Matrix4f& m) {
    *this *= m;
    return *this;
  }

  Vector3f applyV(const Vector3f& v) const {
    return Vector3f(
      e[0][0] * v.x + e[0][1] * v.y + e[0][2] * v.z,
      e[1][0] * v.x + e[1][1] * v.y + e[1][2] * v.z,
      e[2][0] * v.x + e[2][1] * v.y + e[2][2] * v.z
    );
  }

  Vector3f applyN(const Vector3f& n) const {
    return Vector3f(
      e[0][0] * n.x + e[1][0] * n.y + e[2][0] * n.z,
      e[0][1] * n.x + e[1][1] * n.y + e[2][1] * n.z,
      e[0][2] * n.x + e[1][2] * n.y + e[2][2] * n.z
    );
  }

  Vector3f applyP(const Vector3f& p) const {
    auto x = e[0][0] * p.x + e[0][1] * p.y + e[0][2] * p.z + e[0][3];
    auto y = e[1][0] * p.x + e[1][1] * p.y + e[1][2] * p.z + e[1][3];
    auto z = e[2][0] * p.x + e[2][1] * p.y + e[2][2] * p.z + e[2][3];
    auto w = e[3][0] * p.x + e[3][1] * p.y + e[3][2] * p.z + e[3][3];
    if (w == 1)
      return Vector3f(x, y, z);
    return Vector3f(x, y, z) / w;
  }

  Ray operator()(const Ray& r) const {
    return Ray(applyP(r.o), applyV(r.d), r.tMax);
  }

  Matrix4f& inverse();

  Matrix4f& transpose();

  bool operator==(const Matrix4f& m) const {
    for (auto r = 0; r < 4; ++r)
      for (auto c = 0; c < 4; ++c)
        if (e[r][c] != m.e[r][c]) return false;
    return true;
  }

  bool operator!=(const Matrix4f& m) const {
    for (auto r = 0; r < 4; ++r)
      for (auto c = 0; c < 4; ++c)
        if (e[r][c] != m.e[r][c]) return true;
    return false;
  }

  std::string toString() const {
    std::ostringstream oss;
    Eigen::Matrix4f tmp;
      tmp <<
        e[0][0], e[0][1], e[0][2], e[0][3],
        e[1][0], e[1][1], e[1][2], e[1][3],
        e[2][0], e[2][1], e[2][2], e[2][3],
        e[3][0], e[3][1], e[3][2], e[3][3];
    oss << tmp.format(Eigen::IOFormat(4, 0, ", ", ",\n", "", "", "[", "]"));
    return oss.str();
  }

  static Matrix4f identity() {
    return Matrix4f(
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    );
  }

  static Matrix4f translate(float x, float y, float z) {
    return Matrix4f(
      1, 0, 0, x,
      0, 1, 0, y,
      0, 0, 1, z,
      0, 0, 0, 1
    );
  }

  static Matrix4f scale(float x, float y, float z) {
    return Matrix4f(
      x, 0, 0, 0,
      0, y, 0, 0,
      0, 0, z, 0,
      0, 0, 0, 1
    );
  }

  static Matrix4f rotateX(float angle) {
    auto s = std::sin(radians(angle));
    auto c = std::cos(radians(angle));
    return Matrix4f(
      1, 0,  0, 0,
      0, c, -s, 0,
      0, s,  c, 0,
      0, 0,  0, 1
    );
  }

  static Matrix4f rotateY(float angle) {
    auto s = std::sin(radians(angle));
    auto c = std::cos(radians(angle));
    return Matrix4f(
       c, 0, s, 0,
       0, 1, 0, 0,
      -s, 0, c, 0,
       0, 0, 0, 1
    );
  }

  static Matrix4f rotateZ(float angle) {
    auto s = std::sin(radians(angle));
    auto c = std::cos(radians(angle));
    return Matrix4f(
      c, -s, 0, 0,
      s,  c, 0, 0,
      0,  0, 1, 0,
      0,  0, 0, 1
    );
  }

  static Matrix4f rotate(const Vector3f& axis, float angle) {
    auto s = std::sin(radians(angle));
    auto c = std::cos(radians(angle));
    auto x = axis.x, y = axis.y, z = axis.z;
    auto xx = x * x, xy = x * y, xz = x * z, yy = y * y, yz = y * z, zz = z * z;
    return Matrix4f(
      xx + (1 - xx) * c, xy * (1 - c) - z * s, xz * (1 - c) + y * s, 0,
      xy * (1 - c) + z * s, yy + ( 1- yy) * c, yz * (1 - c) - x * s, 0,
      xz * (1 - c) - y * s, yz * (1 - c) + x * s, zz + (1 - zz) * c, 0,
      0, 0, 0, 1
    );
  }

  static Matrix4f lookAt(const Vector3f& origin, const Vector3f& target, const Vector3f& up) {
    auto z = normalize(target - origin);
    auto x = normalize(cross(up, z));
    auto y = cross(z, x);
    return Matrix4f(
      x.x, y.x, z.x, origin.x,
      x.y, y.y, z.y, origin.y,
      x.z, y.z, z.z, origin.z,
      0, 0, 0, 1
    );
  }

  static Matrix4f perspective(float fov, float near, float far) {
    auto inv = 1 / (far - near);
    auto d = 1 / std::tan(radians(fov / 2));
    return Matrix4f(
      d, 0, 0, 0,
      0, d, 0, 0,
      0, 0, far * inv, -near * far * inv,
      0, 0, 1, 0
    );
  }

public:
  float e[4][4];
};

inline Matrix4f inverse(const Matrix4f& m) {
  Matrix4f tmp = m;
  return tmp.inverse();
}

inline Matrix4f transpose(const Matrix4f& m) {
  return Matrix4f(
    m.e[0][0], m.e[1][0], m.e[2][0], m.e[3][0],
    m.e[0][1], m.e[1][1], m.e[2][1], m.e[3][1],
    m.e[0][2], m.e[1][2], m.e[2][2], m.e[3][2],
    m.e[0][3], m.e[1][3], m.e[2][3], m.e[3][3]
  );
}

}
