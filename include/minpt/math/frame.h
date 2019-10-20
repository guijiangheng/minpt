#pragma once

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

}
