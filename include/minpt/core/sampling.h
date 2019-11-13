#pragma once

#include <minpt/math/math.h>

namespace minpt {

inline Vector2f uniformSampleTriangle(const Vector2f& u) {
  auto x = std::sqrt(u[0]);
  return Vector2f(1 - x, u[1] * x);
}

inline Vector2f uniformSampleDisk(const Vector2f& u) {
  auto r = std::sqrt(u[0]);
  auto theta = 2 * Pi * u[1];
  return Vector2f(r * std::cos(theta), r * std::sin(theta));
}

inline Vector3f uniformSampleHemisphere(const Vector2f& u) {
  auto z = u[0];
  auto r = std::sqrt(std::max(0.0f, 1 - z * z));
  auto phi = 2 * Pi * u[1];
  return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

inline Vector3f cosineSampleHemisphere(const Vector2f& u) {
  auto p = uniformSampleDisk(u);
  return Vector3f(p.x, p.y, std::sqrt(std::max(0.0f, 1 - p.lengthSquared())));
}

inline float uniformSampleDiskPdf(const Vector2f& v) {
  return v.length() < 1.0f ? InvPi : 0;
}

inline float uniformSampleHemispherePdf(const Vector3f& v) {
  return v.z < 0 ? 0 : Inv2Pi;
}

inline float cosineSampleHemispherePdf(const Vector3f& v) {
  return v.z < 0 ? 0 : v.z * InvPi;
}

}
