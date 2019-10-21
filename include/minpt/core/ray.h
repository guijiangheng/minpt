#pragma once

#include <minpt/math/vector3.h>

namespace minpt {

class Ray {
public:
  Ray(const Vector3f& o,
      const Vector3f& d,
      float tMax = std::numeric_limits<float>::infinity()) noexcept
    : o(o), d(d), tMax(tMax)
  { }

  Vector3f operator()(float t) const {
    return o + d * t;
  }

  std::string toString() const {
    return tfm::format(
      "Ray[\n"
      "  o = %s,\n"
      "  d = %s,\n"
      "  tMax = %f\n"
      "]", o.toString(), d.toString(), tMax
    );
  }

public:
  Vector3f o;
  Vector3f d;
  mutable float tMax;
};

}
