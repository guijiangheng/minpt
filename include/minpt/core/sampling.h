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
  float sinTheta, cosTheta;
  sincos(theta, sinTheta, cosTheta);
  return Vector2f(r * sinTheta, r * cosTheta);
}

inline Vector3f uniformSampleHemisphere(const Vector2f& u) {
  auto z = u[0];
  auto r = std::sqrt(1.0f - z * z);
  auto phi = 2 * Pi * u[1];
  float sinPhi, cosPhi;
  sincos(phi, sinPhi, cosPhi);
  return Vector3f(r * cosPhi, r * sinPhi, z);
}

inline Vector3f cosineSampleHemisphere(const Vector2f& u) {
  auto p = uniformSampleDisk(u);
  return Vector3f(p.x, p.y, std::sqrt(1 - p.lengthSquared()));
}

inline Vector3f uniformSampleSphere(const Vector2f& u) {
  auto cosTheta = clamp(1.0f - u[0] * 2, -1.0f, 1.0f);
  auto sinTheta = std::sqrt(1 - cosTheta * cosTheta);
  return sphericalDirection(sinTheta, cosTheta, u[1] * Pi * 2);
}

inline float uniformSampleTrianglePdf(const Vector2f& v) {
  return v.x + v.y > 1.0f ? 0 : 2.0f;
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

inline float uniformSampleSpherePdf(const Vector3f& w) {
  return Inv4Pi;
}

}
