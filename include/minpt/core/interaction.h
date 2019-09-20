#pragma once

#include <minpt/math/frame.h>
#include <minpt/common/common.h>

namespace minpt {

class Interaction {
public:
  Interaction() = default;

  Vector3f toLocal(const Vector3f& v) const {
    return shFrame.toLocal(v);
  }

  Vector3f toWorld(const Vector3f& v) const {
    return shFrame.toWorld(v);
  }

  std::string toString() const {
    return tfm::format(
      "Interaction[\n"
      "  p = %s,\n"
      "  n = %s,\n"
      "  uv = %s,\n"
      "  shFrame = %s\n"
      "]",
      p.toString(), n.toString(), uv.toString(), indent(shFrame.toString())
    );
  }

public:
  Vector3f p;
  Vector3f n;
  Vector2f uv;
  Frame shFrame;
};

}
