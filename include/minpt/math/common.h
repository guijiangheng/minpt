#pragma once

#include <cmath>
#include <limits>
#include <minpt/math/vector2.h>

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

#if defined(_GNU_SOURCE)
  inline void sincos(float theta, float& sin, float& cos) {
    ::sincosf(theta, &sin, &cos);
  }

  inline void sincos(double theta, double& sin, double& cos) {
    ::sincos(theta, &sin, &cos);
  }
#else
  inline void sincos(float theta, float& sin, float& cos) {
    sin = sinf(theta);
    cos = cosf(theta);
  }

  inline void sincos(double theta, double& sin, double& cos) {
    sin = ::sin(theta);
    cos = ::cos(theta);
  }
#endif

inline float safe_asin(float value) {
  return std::asin(std::min(1.0f, std::max(-1.0f, value)));
}

inline double safe_asin(double value) {
  return std::asin(std::min(1.0, std::max(-1.0, value)));
}

inline float safe_acos(float value) {
  return std::acos(std::min(1.0f, std::max(-1.0f, value)));
}

inline double safe_acos(double value) {
  return std::acos(std::min(1.0, std::max(-1.0, value)));
}

inline float safe_sqrt(float value) {
  return std::sqrt(std::max(0.0f, value));
}

inline double safe_sqrt(double value) {
  return std::sqrt(std::max(0.0, value));
}

inline float hypot2(float a, float b) {
  if (std::abs(a) > std::abs(b)) {
    auto r = b / a;
    return std::abs(a) * std::sqrt(1.0f + r * r);
  }
  if (b != 0.0f) {
    auto r = a / b;
    return std::abs(b) * std::sqrt(1.0f + r * r);
  }
  return 0.0f;
}

inline double hypot2(double a, double b) {
  if (std::abs(a) > std::abs(b)) {
    auto r = b / a;
    return std::abs(a) * std::sqrt(1.0 + r * r);
  }
  if (b != 0.0) {
    auto r = a / b;
    return std::abs(b) * std::sqrt(1.0 + r * r);
  }
  return 0.0;
}

template <typename T>
inline T mod(T a, T b) {
  auto val = a % b;
  return val < 0 ? val + b : val;
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
