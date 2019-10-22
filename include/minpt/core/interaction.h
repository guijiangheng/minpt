#pragma once

#include <minpt/math/frame.h>
#include <minpt/utils/utils.h>

namespace minpt {

class Mesh;

class Interaction {
public:
  Interaction() noexcept = default;

  Vector3f toLocal(const Vector3f& v) const {
    return shFrame.toLocal(v);
  }

  Vector3f toWorld(const Vector3f& v) const {
    return shFrame.toWorld(v);
  }

  Vector3f offsetRayOrigin(const Vector3f& w) const {
    return p + faceForward(n, w) * RayOriginOffsetEpsilon;
  }

  Ray spawnRay(const Vector3f& w) const {
    return Ray(offsetRayOrigin(w), w);
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
  static constexpr auto RayOriginOffsetEpsilon = 0.00001f;
  Vector3f p;
  Vector3f n;
  Vector2f uv;
  Vector3f wo;
  Frame shFrame;
  const Mesh* mesh;
};

}
