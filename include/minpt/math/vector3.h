#pragma once

#include <minpt/math/vector2.h>

namespace minpt {

template <typename T>
class Vector3 {
public:
  Vector3() noexcept = default;

  explicit Vector3(T x) noexcept : x(x), y(x), z(x)
  { }

  Vector3(T x, T y, T z) noexcept : x(x), y(y), z(z)
  { }

  T& operator[](int index) {
    return (&x)[index];
  }

  T operator[](int index) const {
    return (&x)[index];
  }

  Vector3 operator+(const Vector3& v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
  }

  Vector3& operator+=(const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  Vector3 operator-(const Vector3& v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
  }

  Vector3& operator-=(const Vector3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  Vector3 operator*(T k) const {
    return Vector3(x * k, y * k, z * k);
  }

  Vector3& operator*=(T k) {
    x *= k;
    y *= k;
    z *= k;
    return *this;
  }

  Vector3 operator*(const Vector3& v) const {
    return Vector3(x * v.x, y * v.y, z * v.z);
  }

  Vector3& operator*=(const Vector3& v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  Vector3 operator/(T k) const {
    k = 1 / k;
    return Vector3(x * k, y * k, z * k);
  }

  Vector3 operator/=(T k) {
    k = 1 / k;
    x *= k;
    y *= k;
    z *= k;
    return *this;
  }

  Vector3 operator/(const Vector3& v) const {
    return Vector3(x / v.x, y / v.y, z / v.z);
  }

  Vector3& operator/=(const Vector3& v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }

  Vector3 operator-() const {
    return Vector3(-x, -y, -z);
  }

  T lengthSquared() const {
    return x * x + y * y + z * z;
  }

  T length() const {
    return std::sqrt(lengthSquared());
  }

  T maxComponent() const {
    return x > y ? (x > z ? x : z) : (y > z ? y : z);
  }

  Vector3& normalize() {
    *this /= length();
    return *this;
  }

  bool operator==(const Vector3& v) const {
    return x == v.x && y == v.y && z == v.z;
  }

  bool operator!=(const Vector3& v) const {
    return x != v.x || y != v.y || z != v.z;
  }

  std::string toString() const {
    return tfm::format(
      "[%s, %s, %s]",
      std::to_string(x),
      std::to_string(y),
      std::to_string(z)
    );
  }

public:
  T x, y, z;
  using Scalar = T;
};

using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;

template <typename T>
T dot(const Vector3<T>& a, const Vector3<T>& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
T absdot(const Vector3<T>& a, const Vector3<T>& b) {
  return std::abs(dot(a, b));
}

template <typename T>
Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
  return Vector3<T>(
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  );
}

template <typename T>
Vector3<T> abs(const Vector3<T>& v) {
  return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template <typename T>
Vector3<T> min(const Vector3<T>& a, const Vector3<T>& b) {
  return Vector3<T>(
    std::min(a.x, b.x),
    std::min(a.y, b.y),
    std::min(a.z, b.z)
  );
}

template <typename T>
Vector3<T> max(const Vector3<T>& a, const Vector3<T>& b) {
  return Vector3<T>(
    std::max(a.x, b.x),
    std::max(a.y, b.y),
    std::max(a.z, b.z)
  );
}

inline Vector3f faceForward(const Vector3f& n, const Vector3f& v) {
  return dot(n, v) > 0 ? n : -n;
}

inline Vector3f reflect(const Vector3f& w, const Vector3f& n) {
  return -w + n * dot(w, n) * 2;
}

inline void coordinateSystem(const Vector3f& a, Vector3f& b, Vector3f& c) {
  b = std::abs(a.x) > std::abs(a.y) ?
    Vector3f(-a.z, 0, a.x) / std::sqrt(a.x * a.x + a.z * a.z) :
    Vector3f(0, -a.z, a.y) / std::sqrt(a.y * a.y + a.z * a.z);
  c = cross(a, b);
}

}
