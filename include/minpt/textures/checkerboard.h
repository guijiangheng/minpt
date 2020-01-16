#pragma once

#include <minpt/core/texture.h>

namespace minpt {

template <typename T>
class CheckerboardTexture : public Texture<T> {
public:
  CheckerboardTexture(const PropertyList& props);

  std::string toString() const override;

  T eval(const Vector2f& uv) const override {
    auto value = Vector2i(floor(mapping.map(uv)));
    return ((value.x % 2) ^ (value.y % 2)) ? value2 : value1;
  }

public:
  T value1;
  T value2;
  TextureMapping2D mapping;
};

template <>
CheckerboardTexture<float>::CheckerboardTexture(const PropertyList& props)
  : value1(props.getFloat("value1", 0.0f))
  , value2(props.getFloat("value2", 1.0f))
  , mapping(
      props.getVector2f("scale", Vector2f(1.0f)),
      props.getVector2f("delta", Vector2f(0.0f)))
{ }

template <>
CheckerboardTexture<Color3f>::CheckerboardTexture(const PropertyList& props)
  : value1(props.getColor3f("value1", Color3f(0.0f)))
  , value2(props.getColor3f("value2", Color3f(1.0f)))
  , mapping(
        props.getVector2f("scale", Vector2f(1.0f)),
        props.getVector2f("delta", Vector2f(0.0f)))
{ }

template <>
std::string CheckerboardTexture<float>::toString() const {
  return tfm::format(
    "CheckboardTexture[\n"
    "  value1 = %f,\n"
    "  value2 = %f,\n"
    "  mapping = %s\n"
    "]",
    value1, value2, indent(mapping.toString())
  );
}

template <>
std::string CheckerboardTexture<Color3f>::toString() const {
  return tfm::format(
    "CheckboardTexture[\n"
    "  value1 = %s,\n"
    "  value2 = %s,\n"
    "  mapping = %s\n"
    "]",
    value1.toString(), value2.toString(), indent(mapping.toString())
  );
}

}
