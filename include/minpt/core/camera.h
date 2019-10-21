#pragma once

#include <minpt/math/matrix4.h>
#include <minpt/core/object.h>

namespace minpt {

struct CameraSample {
  Vector2f pFilm;
};

class Camera : public Object {
public:
  Camera(const Matrix4f& frame, const Vector2i& outputSize) noexcept
    : frame(frame), outputSize(outputSize)
  { }

  virtual ~Camera() = default;

  virtual Ray generateRay(const CameraSample& sample) const = 0;

  EClassType getClassType() const override {
    return ECamera;
  }

public:
  Matrix4f frame;
  Vector2i outputSize;
};

}
