#pragma once

#include <Eigen/Geometry>
#include <minpt/math/vector.h>

namespace minpt {

class Frame {
public:
  Frame() = default;

  Frame(const Vector3f& n) : n(n) {
    Frame::coordinateSystem(n, s, t);
  }

  Frame(const Vector3f& n, const Vector3f& s, const Vector3f& t) : n(n), s(s), t(t)
  { }

  Vector3f toLocal(const Vector3f& v) const {
    return Vector3f(v.dot(s), v.dot(t), v.dot(n));
  }

  Vector3f toWorld(const Vector3f& v) const {
    return s * v.x() + t * v.y() + n * v.z();
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

  static void coordinateSystem(const Vector3f& n, Vector3f& s, Vector3f& t) {
    s = std::abs(n.x()) > std::abs(n.y()) ?
      Vector3f(-n.z(), 0, n.x()) / std::sqrt(n.x() * n.x() + n.z() * n.z()) :
      Vector3f(0, -n.z(), n.y()) / std::sqrt(n.y() * n.y() + n.z() * n.z());
    t = n.cross(s);
  }

public:
  Vector3f n, s, t;
};

}
