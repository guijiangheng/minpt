#pragma once

#include <minpt/math/common.h>
#include <minpt/math/vector3.h>

namespace minpt {

class Frame {
public:
  Frame() = default;

  Frame(const Vector3f& n) : n(n) {
    coordinateSystem(n, s, t);
  }

  Frame(const Vector3f& n, const Vector3f& s, const Vector3f& t) : n(n), s(s), t(t)
  { }

  Vector3f toLocal(const Vector3f& v) const {
    return Vector3f(dot(v, s), dot(v, t), dot(v, n));
  }

  Vector3f toWorld(const Vector3f& v) const {
    return s * v.x + t * v.y + n * v.z;
  }

  std::string toString() const {
    return tfm::format(
      "Frame[\n"
      "  s = %s,\n"
      "  t = %s,\n"
      "  n = %s\n"
      "]", s.toString(), t.toString(), n.toString()
    );
  }

public:
  Vector3f n, s, t;
};

float sinTheta(const Vector3f& w);
float cosTheta(const Vector3f& w);
float tanTheta(const Vector3f& w);
float sin2Theta(const Vector3f& w);
float cos2Theta(const Vector3f& w);
float tan2Theta(const Vector3f& w);

float sinPhi(const Vector3f& w);
float cosPhi(const Vector3f& w);
float sin2Phi(const Vector3f& w);
float cos2Phi(const Vector3f& w);

inline float sinTheta(const Vector3f& w) {
  return std::sqrt(sin2Theta(w));
}

inline float sin2Theta(const Vector3f& w) {
  return std::max(0.0f, 1.0f - cos2Theta(w));
}

inline float cosTheta(const Vector3f& w) {
  return w.z;
}

inline float cos2Theta(const Vector3f& w) {
  return w.z * w.z;
}

inline float absCosTheta(const Vector3f& w) {
  return std::abs(w.z);
}

inline float tanTheta(const Vector3f& w) {
  return sinTheta(w) / cosTheta(w);
}

inline float tan2Theta(const Vector3f& w) {
  return sin2Theta(w) / cos2Theta(w);
}

inline float sinPhi(const Vector3f& w) {
  auto sinTheta_ = sinTheta(w);
  return sinTheta_ == 0 ? 0 : clamp(w.y / sinTheta_, -1.0f, 1.0f);
}

inline float sin2Phi(const Vector3f& w) {
  auto sinPhi = minpt::sinPhi(w);
  return sinPhi * sinPhi;
}

inline float cosPhi(const Vector3f& w) {
  auto sinTheta_ = sinTheta(w);
  return sinTheta_ == 0 ? 0 : clamp(w.x / sinTheta_, -1.0f, 1.0f);
}

inline float cos2Phi(const Vector3f& w) {
  auto cosPhi_ = cosPhi(w);
  return cosPhi_ * cosPhi_;
}

inline bool sameHemisphere(const Vector3f& wo, const Vector3f& wi) {
  return wo.z * wi.z > 0;
}

inline float sphericalTheta(const Vector3f& w) {
  return std::acos(w.z);
}

inline float sphericalPhi(const Vector3f& w) {
  auto p = std::atan2(w.y, w.x);
  return p < 0 ? (p + 2 * Pi) : p;
}

inline Vector3f sphericalDirection(float sinTheta, float cosTheta, float phi) {
  float sinPhi, cosPhi;
  sincos(phi, sinPhi, cosPhi);
  return Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

inline Vector3f sphericalDirection(float theta, float phi) {
  float sinTheta, cosTheta;
  sincos(theta, sinTheta, cosTheta);
  return sphericalDirection(sinTheta, cosTheta, phi);
}

}
