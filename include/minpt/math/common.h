#pragma once

#include <limits>

namespace minpt {

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

template <typename T, typename U, typename V>
constexpr T clamp(T val, U low, V high) {
  if (val < low) return low;
  if (val > high) return high;
  return val;
}

template <typename Vector>
Vector lerp(const Vector& a, const Vector& b, typename Vector::Scalar t) {
  return a + (b - a) * t;
}

template <typename Vector>
Vector barycentric(const Vector& a, const Vector& b, const Vector& c, const Vector2<typename Vector::Scalar>& uv) {
  return a * (1 - uv.x - uv.y) + b * uv.x + c * uv.y;
}

template <typename Vector>
Vector normalize(const Vector& v) {
  return v / v.length();
}

template <typename Vector>
typename Vector::Scalar distance(const Vector& a, const Vector& b) {
  return (a - b).length();
}

template <typename Vector>
typename Vector::Scalar distanceSquared(const Vector& a, const Vector& b) {
  return (a - b).lengthSquared();
}

}
