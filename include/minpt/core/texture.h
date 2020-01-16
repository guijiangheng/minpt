#pragma once

#include <minpt/core/object.h>

namespace minpt {

class TextureMapping2D {
public:
  TextureMapping2D(Vector2f scale = Vector2f(1.0f), Vector2f delta = Vector2f(0.0f))
    : scale(scale), delta(delta)
  { }

  Vector2f map(const Vector2f& uv) const {
    return Vector2f(uv.x * scale.x, uv.y + scale.y) + delta;
  }

  std::string toString() const {
    return tfm::format(
      "TextureMapping2D[\n"
      "  scale = %s,\n"
      "  delta = %s\n"
      "]",
      scale.toString(), delta.toString()
    );
  }

public:
  Vector2f scale;
  Vector2f delta;
};

template <typename T>
class Texture : public Object {
public:
  virtual ~Texture() = default;

  virtual T eval(const Vector2f& isect) const = 0;

  EClassType getClassType() const override {
    return ETexture;
  }
};

}
