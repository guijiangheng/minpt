#pragma once

#include <minpt/math/matrix.h>

namespace minpt {

struct CameraSample {
  Vector2f pFilm;
};

class Camera {
public:
  Camera(const Matrix4f& frame, const Vector2i& outputSize) noexcept
    : frame(frame), outputSize(outputSize)
  { }

  virtual ~Camera() = default;

  virtual Ray3f generateRay(const CameraSample& sample) const = 0;

  virtual std::string toString() const = 0;

public:
  Matrix4f frame;
  Vector2i outputSize;
};

}
