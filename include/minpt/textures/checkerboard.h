#pragma once

#include <minpt/core/texture.h>

namespace minpt {

template <typename T>
class CheckerboardTexture : public Texture<T> {
public:
  CheckerboardTexture(const PropertyList& props);

  std::string toString() const override;

  T eval(const Vector2f& uv) const override {
    auto value = Vector2i(floor(Vector2f(uv.x * scale.x + delta.x, uv.y * scale.y + delta.y)));
    return ((value.x % 2) ^ (value.y % 2)) ? value2 : value1;
  }

public:
  T value1;
  T value2;
  Vector2f delta;
  Vector2f scale;
};

template <>
CheckerboardTexture<float>::CheckerboardTexture(const PropertyList& props) {
  delta = props.getVector2f("delta", Vector2f(0.0f));
  scale = props.getVector2f("scale", Vector2f(1.0f));
  value1 = props.getFloat("value1", 0.0f);
  value2 = props.getFloat("value2", 1.0f);
}

template <>
CheckerboardTexture<Color3f>::CheckerboardTexture(const PropertyList& props) {
  delta = props.getVector2f("delta", Vector2f(0.0f));
  scale = props.getVector2f("scale", Vector2f(1.0f));
  value1 = props.getColor3f("value1", Color3f(0.0f));
  value2 = props.getColor3f("value2", Color3f(1.0f));
}

template <>
std::string CheckerboardTexture<float>::toString() const {
  return tfm::format(
    "CheckboardTexture[\n"
    "  delta = %s,\n"
    "  scale = %s,\n"
    "  value1 = %f,\n"
    "  value2 = %f\n"
    "]",
    delta.toString(), scale.toString(), value1, value2
  );
}

template <>
std::string CheckerboardTexture<Color3f>::toString() const {
  return tfm::format(
    "CheckboardTexture[\n"
    "  delta = %s,\n"
    "  scale = %s,\n"
    "  value1 = %s,\n"
    "  value2 = %s\n"
    "]",
    delta.toString(), scale.toString(), value1.toString(), value2.toString()
  );
}

}
